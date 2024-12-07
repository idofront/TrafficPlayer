#ifndef TRAFFIC_PLAYER_HPP
#define TRAFFIC_PLAYER_HPP

#include <TrafficMaker/CustomDurationReplayTrafficMaker.hpp>
#include <TrafficMaker/PacketsPerSecondTrafficMaker.hpp>
#include <TrafficMaker/SpeedScaledReplayTrafficMaker.hpp>
#include <TrafficMaker/UniformThroughputTrafficMaker.hpp>
#include <memory>

static const auto NUM_OF_DEALERS = std::size_t(1);
static const auto NUM_OF_REPORTERS = std::size_t(1);
static const auto NUM_OF_PRODUCERS = std::size_t(1);
static const auto NUM_OF_THREADS = std::size_t(NUM_OF_DEALERS + NUM_OF_REPORTERS + NUM_OF_PRODUCERS);

inline std::shared_ptr<TrafficMaker::AbstractTrafficMaker> GetTrafficMaker(const ParseOptions &options)
{
    switch (options.Mode())
    {
    case Mode::Throughput:
        return std::make_shared<TrafficMaker::UniformTrafficMaker>(options.PcapFilePath(), options.ThroughputMbps());
    case Mode::SpeedScale:
        return std::make_shared<TrafficMaker::SpeedScaleReplayTrafficMaker>(options.PcapFilePath(),
                                                                            options.SpeedScaleFactor());
    case Mode::Duration:
        return std::make_shared<TrafficMaker::CustomDurationReplayTrafficMaker>(
            options.PcapFilePath(), std::chrono::milliseconds(static_cast<long long>(options.DurationTime() * 1e3)));
    case Mode::PacketsPerSecond:
        return std::make_shared<TrafficMaker::PacketsPerSecondTrafficMaker>(options.PcapFilePath(),
                                                                            options.PacketsPerSecond());
    default:
        throw std::runtime_error("Unknown mode");
    }
}

inline void AdjustTrafficRecords(const ParseOptions &options,
                                 std::shared_ptr<BoundedThreadSafeQueue<ReserveTimeRecord>> &reserveTimeQueuePtr)
{
    // Create reserve timing adjuster
    auto reserveTimingAdjuster = ReserveTimingAdjuster(reserveTimeQueuePtr);

    auto repeatCount = options.RepeatCount();
    uint64_t repeat = 0;

    // Read pcap file and make traffic records to be reserved and sent later by the producer.
    auto trafficMakerPtr = GetTrafficMaker(options);
    auto trafficRecords = trafficMakerPtr->Make();
    while (repeatCount == 0 || (repeatCount > repeat++))
    {
        std::for_each(trafficRecords.begin(), trafficRecords.end(),
                      [&reserveTimingAdjuster](const TrafficRecord &trafficRecord) {
                          reserveTimingAdjuster.Adjust(trafficRecord);
                      });
    }
}

inline std::vector<std::shared_ptr<Thread::Future>> CreateProducers(Thread::Employer &employer,
    std::shared_ptr<BoundedThreadSafeQueue<ReserveTimeRecord>> &reserveTimeQueuePtr,
    std::shared_ptr<ThreadSafeQueue<TrafficRecord>> &queuePtr)
{
    auto producerFuturePtrs = std::vector<std::shared_ptr<Thread::Future>>();
    for (auto i = 0; i < NUM_OF_PRODUCERS; i++)
    {
        auto producerFuturePtr = employer.Submit(std::make_shared<Producer>(reserveTimeQueuePtr, queuePtr));
        producerFuturePtrs.push_back(producerFuturePtr);
    }
    return producerFuturePtrs;
}

#endif
