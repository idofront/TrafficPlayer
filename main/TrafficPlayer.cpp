#include <Producer.hpp>
#include <ThreadSafeQueue.hpp>
#include <TrafficPlayer.hpp>
#include <TrafficRecord.hpp>
#include <arpa/inet.h>
#include <boost/format.hpp>
#include <filesystem>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <pcap.h>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

class Consumer
{
  public:
    Consumer(std::shared_ptr<TrafficPlayer::ThreadSafeQueue<TrafficRecord>> queue, const std::string &device_name)
        : queue(queue), device_name(device_name)
    {
        PrepareSocket();
    }
    virtual ~Consumer()
    {
        close(sockfd);
    }

    void operator()()
    {
        while (true)
        {
            std::chrono::milliseconds timeout(1000);
            auto result = queue->dequeue(timeout);

            if (result.has_value())
            {
                auto str = TrafficPlayer::HexString(result.value().Data());
                try
                {
                    Send(result.value().Data());
                }
                catch (const std::exception &e)
                {
                    SPDLOG_ERROR(e.what());
                }
                SPDLOG_INFO("Consumed: {}", str);
            }
            else
            {
                SPDLOG_INFO("Timeout: No value to consume.");
            }
        }
    }

  private:
    std::shared_ptr<TrafficPlayer::ThreadSafeQueue<TrafficRecord>> queue;

    int sockfd;
    struct ifreq if_idx;
    struct sockaddr_ll device;
    const std::string device_name;

    void Send(const std::vector<uint8_t> &data)
    {
        // データ送信
        if (sendto(sockfd, data.data(), data.size(), 0, (struct sockaddr *)&device, sizeof(device)) < 0)
        {
            auto error = strerror(errno);
            auto fmt = boost::format("Failed to send data: %1%");
            throw std::runtime_error(boost::str(fmt % error));
        }
    }

    void PrepareSocket()
    {
        // Create socket
        sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if (sockfd < 0)
        {
            throw std::runtime_error("Failed to create socket");
        }
        SPDLOG_INFO("Socket created");

        // Get interface index
        memset(&if_idx, 0, sizeof(struct ifreq));
        strncpy(if_idx.ifr_name, device_name.c_str(), IFNAMSIZ - 1);
        auto ioctl_result = ioctl(sockfd, SIOCGIFINDEX, &if_idx);
        if (ioctl_result < 0)
        {
            auto fmt = boost::format("ioctl failed with error: %1%");
            auto msg = boost::str(fmt % strerror(errno));
            throw std::runtime_error(msg);
        }

        // Set promiscuous mode
        struct packet_mreq mreq;
        memset(&mreq, 0, sizeof(mreq));
        mreq.mr_ifindex = if_idx.ifr_ifindex;
        mreq.mr_type = PACKET_MR_PROMISC;
        if (setsockopt(sockfd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
        {
            throw std::runtime_error("Failed to set socket promiscuous mode");
        }

        // Set device
        memset(&device, 0, sizeof(device));
        device.sll_family = AF_PACKET;
        device.sll_ifindex = if_idx.ifr_ifindex;
        device.sll_protocol = htons(ETH_P_ALL);

        // Bind socket
        if (bind(sockfd, (struct sockaddr *)&device, sizeof(device)) < 0)
        {
            throw std::runtime_error("Failed to bind socket");
        }
    }
};

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
