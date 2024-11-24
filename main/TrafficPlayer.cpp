#include <Dealer.hpp>
#include <ParseOptions.hpp>
#include <Producer.hpp>
#include <ThreadSafeQueue.hpp>
#include <TrafficMaker/SpeedScaledReplayTrafficMaker.hpp>
#include <TrafficMaker/UniformThroughputTrafficMaker.hpp>
#include <TrafficPlayer.hpp>
#include <TrafficRecord.hpp>
#include <boost/format.hpp>
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
        else if (options.Mode() == Mode::SpeedScale)
        {
            spdlog::info("Mode: SpeedScale");
            trafficMakerPtr =
                std::make_shared<TrafficMaker::SpeedScaleRepalyTrafficMaker>(pcapFile, options.SpeedScaleFactor());
        }
        else if (options.Mode() == Mode::Duration)
        {
            spdlog::info("Mode: Custom duration");
            throw std::runtime_error("Not implemented");
        }
        else
        {
            throw std::runtime_error("Unknown mode");
        }

        auto producer = Producer(queuePtr);
        auto dealer = Dealer(queuePtr, interface);

        auto dealer_thread = std::thread(dealer);

        // Read pcap file
        auto trafficRecords = trafficMakerPtr->Make();

        std::for_each(trafficRecords.begin(), trafficRecords.end(),
                      [&producer](const TrafficRecord &trafficRecord) { producer.Produce(trafficRecord); });

        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } while (!queuePtr->empty());

        // TODO: Wait for all packets to be sent.
        // This implementation is forceful and may cause packet loss.
        dealer_thread.detach();
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
