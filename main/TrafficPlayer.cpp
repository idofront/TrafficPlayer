#include <Dealer/DealReporter.hpp>
#include <Dealer/Dealer.hpp>
#include <ParseOptions.hpp>
#include <Producer/Producer.hpp>
#include <Queue/ThreadSafeQueue.hpp>
#include <Thread/Employer.hpp>
#include <TimingAdjuster/ReserveTimingAdjuster.hpp>
#include <TrafficMaker/CustomDurationReplayTrafficMaker.hpp>
#include <TrafficMaker/PacketsPerSecondTrafficMaker.hpp>
#include <TrafficMaker/SpeedScaledReplayTrafficMaker.hpp>
#include <TrafficMaker/UniformThroughputTrafficMaker.hpp>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[])
{
    try
    {
        auto options = ParseOptions(argc, argv);

        spdlog::set_level(options.LogLevel());

        auto interface = options.InterfaceName();
        auto pcapFile = options.PcapFilePath();
        auto reportIntervalMsec =
            std::chrono::milliseconds(static_cast<long long>(options.ReportIntervalNsec().count() / 1e6));

        auto queuePtr = std::make_shared<ThreadSafeQueue<TrafficRecord>>();

        auto trafficMakerPtr = std::shared_ptr<TrafficMaker::AbstractTrafficMaker>(nullptr);
        if (options.Mode() == Mode::Throughput)
        {
            spdlog::info("Mode: Throughput");
            trafficMakerPtr = std::make_shared<TrafficMaker::UniformTrafficMaker>(pcapFile, options.ThroughputMbps());
        }
        else if (options.Mode() == Mode::SpeedScale)
        {
            spdlog::info("Mode: SpeedScale");
            trafficMakerPtr =
                std::make_shared<TrafficMaker::SpeedScaleReplayTrafficMaker>(pcapFile, options.SpeedScaleFactor());
        }
        else if (options.Mode() == Mode::Duration)
        {
            spdlog::info("Mode: Custom duration");
            trafficMakerPtr = std::make_shared<TrafficMaker::CustomDurationReplayTrafficMaker>(
                pcapFile, std::chrono::milliseconds(static_cast<long long>(options.DurationTime() * 1e3)));
        }
        else if (options.Mode() == Mode::PacketsPerSecond)
        {
            spdlog::info("Mode: Packets Per Second");
            trafficMakerPtr =
                std::make_shared<TrafficMaker::PacketsPerSecondTrafficMaker>(pcapFile, options.PacketsPerSecond());
        }
        else
        {
            throw std::runtime_error("Unknown mode");
        }

        const auto NUM_OF_DEALERS = 1;
        const auto NUM_OF_REPORTERS = 1;
        const auto NUM_OF_PRODUCERS = 1;
        const auto NUM_OF_THREADS = NUM_OF_DEALERS + NUM_OF_REPORTERS + NUM_OF_PRODUCERS;
        auto employer = Thread::Employer(NUM_OF_THREADS);

        auto dealerPtr = std::make_shared<Dealer>(queuePtr, interface);
        auto dealerFuturePtr = employer.Submit(dealerPtr);
        auto reporterFuturePtr = employer.Submit(std::make_shared<DealReporter>(*dealerPtr, reportIntervalMsec));

        auto reserveTimeQueuePtr = std::make_shared<BoundedThreadSafeQueue<ReserveTimeRecord>>(1024);

        // Create reserve timing adjuster
        auto reserveTimingAdjuster = ReserveTimingAdjuster(reserveTimeQueuePtr);

        // Create producers
        auto producerFuturePtrs = std::vector<std::shared_ptr<Thread::Future>>();
        for (auto i = 0; i < NUM_OF_PRODUCERS; i++)
        {
            auto producerFuturePtr = employer.Submit(std::make_shared<Producer>(reserveTimeQueuePtr, queuePtr));
            producerFuturePtrs.push_back(producerFuturePtr);
        }

        auto repeatCount = options.RepeatCount();
        uint64_t repeat = 0;

        // Read pcap file and make traffic records to be reserved and sent later by the producer.
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
                    spdlog::trace("Cycle: {}/{}", repeat, repeatCount);
                }
            }
            else
            {
                spdlog::trace("Cycle: {}", repeat);
            }

            std::for_each(trafficRecords.begin(), trafficRecords.end(),
                          [&reserveTimingAdjuster](const TrafficRecord &trafficRecord) {
                              reserveTimingAdjuster.Adjust(trafficRecord);
                          });
        }

        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } while (!queuePtr->Empty());

        // TODO: Wait for all packets to be sent.
        // This implementation is forceful and may cause packet loss.
        try
        {
            employer.TryTerminate();
            employer.Wait();
            spdlog::info("All threads are terminated.");
        }
        catch (const std::exception &e)
        {
            spdlog::critical(e.what());
        }
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
