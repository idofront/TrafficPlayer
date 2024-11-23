#include <Consumer.hpp>
#include <Producer.hpp>
#include <ThreadSafeQueue.hpp>
#include <TrafficPlayer.hpp>
#include <TrafficRecord.hpp>
#include <boost/format.hpp>
#include <filesystem>
#include <pcap.h>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

std::vector<std::vector<uint8_t>> ReadPcapFile(const std::filesystem::path &path)
{
    std::vector<std::vector<uint8_t>> packets;

    auto pcap = pcap_open_offline(path.c_str(), NULL);
    if (pcap == NULL)
    {
        throw std::runtime_error("Failed to open pcap file");
    }

    struct pcap_pkthdr header;
    const u_char *packet;
    while ((packet = pcap_next(pcap, &header)) != NULL)
    {
        std::vector<uint8_t> data(packet, packet + header.len);
        packets.push_back(data);
    }

    return packets;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        SPDLOG_ERROR("Usage: {} <interface> <pcap file>", argv[0]);
        return 1;
    }

    auto interface = std::string(argv[1]);
    auto pcapFile = std::filesystem::path(argv[2]);

    auto queuePtr = std::make_shared<TrafficPlayer::ThreadSafeQueue<TrafficRecord>>();

    auto producer = Producer(queuePtr);
    auto consumer = Consumer(queuePtr, interface);

    std::thread consumer_thread(consumer);

    // Read pcap file
    auto packets = ReadPcapFile(pcapFile);
    auto expectedThroughputMbps = 0.001;

    for (auto &packet : packets)
    {
        producer.Produce(packet, expectedThroughputMbps);
    }

    while (!queuePtr->empty())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    consumer_thread.detach();

    return 0;
}
