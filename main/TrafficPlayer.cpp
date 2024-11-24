#include <Dealer/DealReporter.hpp>
#include <Dealer/Dealer.hpp>
#include <ParseOptions.hpp>
#include <ThreadSafeQueue.hpp>
#include <TrafficMaker/CustomDurationReplayTrafficMaker.hpp>
#include <TrafficMaker/SpeedScaledReplayTrafficMaker.hpp>
#include <TrafficMaker/UniformThroughputTrafficMaker.hpp>
#include <TrafficPlayer.hpp>
#include <TrafficRecord.hpp>
#include <TransmissionTimingAdjuster.hpp>
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
        auto reportInterval = options.ReportIntervalUsec();

        auto queuePtr = std::make_shared<ThreadSafeQueue<TrafficRecord>>();

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
            trafficMakerPtr = std::make_shared<TrafficMaker::CustomDurationReplayTrafficMaker>(
                pcapFile, std::chrono::milliseconds(static_cast<long long>(options.DurationTime() * 1e3)));
        }
        else
        {
            throw std::runtime_error("Unknown mode");
        }

        auto producer = TransmissionTimingAdjuster(queuePtr);
        auto dealer = Dealer(queuePtr, interface);
        auto dealReporter = DealReporter(dealer, reportInterval);

        auto dealerThread = std::thread(dealer);
        auto dealReporterThread = std::thread(dealReporter);

        auto repeatCount = options.RepeatCount();
        uint64_t repeat = 0;

        // Read pcap file
        auto trafficRecords = trafficMakerPtr->Make();
        while (++repeat)
        {
            if (repeatCount)
            {
                if (repeatCount < repeat)
                {
                    break;
                }
                if (repeatCount > 1)
                {
                    spdlog::info("Cycle: {}/{}", repeat, repeatCount);
                }
            }
            else
            {
                spdlog::info("Cycle: {}", repeat);
            }

            std::for_each(trafficRecords.begin(), trafficRecords.end(),
                          [&producer](const TrafficRecord &trafficRecord) { producer.Produce(trafficRecord); });
        }

        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } while (!queuePtr->empty());

        // TODO: Wait for all packets to be sent.
        // This implementation is forceful and may cause packet loss.
        dealReporterThread.detach();
        dealerThread.detach();
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
