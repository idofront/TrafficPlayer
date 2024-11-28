#ifndef TIMING_ADJUSTER__RESERVE_TIMING_ADJUSTER_HPP
#define TIMING_ADJUSTER__RESERVE_TIMING_ADJUSTER_HPP

#include <Queue/BoundedThreadSafeQueue.hpp>
#include <Queue/ThreadSafeQueue.hpp>
#include <TrafficPlayer.hpp>
#include <TrafficRecord.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>

class ReserveTimeRecord
{
  public:
    ReserveTimeRecord(std::vector<uint8_t> data, std::chrono::time_point<std::chrono::system_clock> reservationTime)
        : _Data(data), _ReservationTime(reservationTime)
    {
    }

    const std::vector<uint8_t> &Data() const
    {
        return _Data;
    }

    const std::chrono::time_point<std::chrono::system_clock> &ReservationTime() const
    {
        return _ReservationTime;
    }

  private:
    std::chrono::time_point<std::chrono::system_clock> _ReservationTime;

    /// @brief Data to send
    std::vector<uint8_t> _Data;
};

class ReserveTimingAdjuster
{
  public:
    ReserveTimingAdjuster(std::shared_ptr<ThreadSafeQueue<TrafficRecord>> queue);

    void Produce(const TrafficRecord &trafficRecord);

    void operator()();

  private:
    std::shared_ptr<ThreadSafeQueue<TrafficRecord>> _Queue;
    std::shared_ptr<BoundedThreadSafeQueue<ReserveTimeRecord>> _ReserveQueue;
    std::chrono::time_point<std::chrono::system_clock> _LatestReservationTime;

    /// @brief Thread pool to send data
    std::vector<std::thread> _ThreadPool;

    /// @brief Thread function to send data
    std::function<void(std::shared_ptr<ThreadSafeQueue<TrafficRecord>>)> MakeTask(
        const ReserveTimeRecord &reserveTimeRecord);
};

#endif
