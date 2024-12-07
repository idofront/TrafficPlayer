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
    ReserveTimingAdjuster(std::shared_ptr<IThreadSafeQueue<ReserveTimeRecord>> queue);
    void Adjust(const TrafficRecord &trafficRecord);

  private:
    std::shared_ptr<IThreadSafeQueue<ReserveTimeRecord>> _ReserveQueue;
    std::chrono::time_point<std::chrono::system_clock> _LatestReservationTime;
};

#endif
