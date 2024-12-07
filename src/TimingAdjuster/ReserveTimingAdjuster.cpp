#include <TimingAdjuster/ReserveTimingAdjuster.hpp>

ReserveTimingAdjuster::ReserveTimingAdjuster(std::shared_ptr<IThreadSafeQueue<ReserveTimeRecord>> queue)
    : _ReserveQueue(queue), _LatestReservationTime(std::chrono::system_clock::now())
{
}

void ReserveTimingAdjuster::Adjust(const TrafficRecord &trafficRecord)
{
    auto now = std::chrono::system_clock::now();
    if (_LatestReservationTime > now)
    {
    }
    else
    {
        _LatestReservationTime = now;
        spdlog::debug("Latest reservation time is in the past. Reset to now.");
    }

    auto reserveSendTime = _LatestReservationTime + trafficRecord.ShouldSpendTimeSending();
    auto reserveTimeRecord = ReserveTimeRecord(trafficRecord.Data(), reserveSendTime);

    _ReserveQueue->Enqueue(reserveTimeRecord);
    _LatestReservationTime = reserveSendTime;
}
