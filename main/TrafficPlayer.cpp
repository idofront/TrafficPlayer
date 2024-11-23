#include <Dealer.hpp>
#include <ParseOptions.hpp>
#include <Producer.hpp>
#include <ThreadSafeQueue.hpp>
#include <TrafficMaker/UniformTrafficMaker.hpp>
#include <TrafficPlayer.hpp>
#include <TrafficRecord.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <filesystem>
#include <iostream>
#include <pcap.h>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    auto options = ParseOptions(argc, argv);

    auto interface = options.InterfaceName();
    auto pcapFile = options.PcapFilePath();

    auto queuePtr = std::make_shared<TrafficPlayer::ThreadSafeQueue<TrafficRecord>>();

    auto producer = Producer(queuePtr);
    auto consumer = Dealer(queuePtr, interface);

    std::thread consumer_thread(consumer);

    // Read pcap file
    auto TrafficMaker = TrafficMaker::UniformTrafficMaker(pcapFile, 0.001);
    auto trafficRecords = TrafficMaker.Make();

    std::for_each(trafficRecords.begin(), trafficRecords.end(),
                  [&producer](const TrafficRecord &trafficRecord) { producer.Produce(trafficRecord); });

    while (!queuePtr->empty())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    consumer_thread.detach();

    return 0;
}
