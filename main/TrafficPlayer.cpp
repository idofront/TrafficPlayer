#include <Dealer.hpp>
#include <ParseOptions.hpp>
#include <Producer.hpp>
#include <ThreadSafeQueue.hpp>
#include <TrafficMaker/UniformThroughputTrafficMaker.hpp>
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
    try
    {
        auto options = ParseOptions(argc, argv);

        spdlog::set_level(options.LogLevel());

        auto interface = options.InterfaceName();
        auto pcapFile = options.PcapFilePath();

        auto queuePtr = std::make_shared<TrafficPlayer::ThreadSafeQueue<TrafficRecord>>();

        auto trafficMakerPtr = std::shared_ptr<TrafficMaker::ITrafficMaker>(nullptr);
        if (options.Mode() == Mode::Throughput)
        {
            spdlog::info("Mode: Throughput");
            trafficMakerPtr = std::make_shared<TrafficMaker::UniformTrafficMaker>(pcapFile, options.ThroughputMbps());
        }
        else if (options.Mode() == Mode::Interval)
        {
            spdlog::info("Mode: Interval");
            throw std::runtime_error("Not implemented");
        }
        else if (options.Mode() == Mode::Duration)
        {
            spdlog::info("Mode: Duration");
            throw std::runtime_error("Not implemented");
        }
        else
        {
            throw std::runtime_error("Unknown mode");
        }

        auto producer = Producer(queuePtr);
        auto consumer = Dealer(queuePtr, interface);

        std::thread consumer_thread(consumer);

        // Read pcap file
        auto trafficRecords = trafficMakerPtr->Make();

        std::for_each(trafficRecords.begin(), trafficRecords.end(),
                      [&producer](const TrafficRecord &trafficRecord) { producer.Produce(trafficRecord); });

        while (!queuePtr->empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        consumer_thread.detach();
    }
    catch (const std::exception &e)
    {
        spdlog::critical(e.what());
        return 1;
    }
    catch (...)
    {
        spdlog::critical("Unknown error");
        return 1;
    }

    return 0;
}
