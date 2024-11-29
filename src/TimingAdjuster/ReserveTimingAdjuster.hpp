#ifndef TIMING_ADJUSTER__RESERVE_TIMING_ADJUSTER_HPP
#define TIMING_ADJUSTER__RESERVE_TIMING_ADJUSTER_HPP

#include <Queue/BoundedThreadSafeQueue.hpp>
#include <Queue/ThreadSafeQueue.hpp>
#include <TimingAdjuster/ReserveTimeRecord.hpp>
#include <TrafficPlayer.hpp>
#include <TrafficRecord.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>

class ReserveTimingAdjuster
{
  public:
    ReserveTimingAdjuster(std::shared_ptr<ThreadSafeQueue<TrafficRecord>> queue);
    virtual void Produce(const TrafficRecord &trafficRecord);
    virtual void Run();
    virtual void TryTerminate();

  private:
    std::shared_ptr<ThreadSafeQueue<TrafficRecord>> _Queue;
    std::shared_ptr<BoundedThreadSafeQueue<ReserveTimeRecord>> _ReserveQueue;
    std::chrono::time_point<std::chrono::system_clock> _LatestReservationTime;

    /// @brief Thread pool to send data
    std::vector<std::thread> _ThreadPool;

    /// @brief Thread function to send data
    std::function<void(std::shared_ptr<ThreadSafeQueue<TrafficRecord>>)> MakeTask(
        const ReserveTimeRecord &reserveTimeRecord);

    bool _IsRequestedToTerminate;
};

#endif
