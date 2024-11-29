#include <TimingAdjuster/ReserveTimingAdjuster.hpp>

ReserveTimingAdjuster::ReserveTimingAdjuster(std::shared_ptr<ThreadSafeQueue<TrafficRecord>> queue)
    : _Queue(queue), _ReserveQueue(std::make_shared<BoundedThreadSafeQueue<ReserveTimeRecord>>(128)),
      _LatestReservationTime(std::chrono::system_clock::now()), _ThreadPool(), _IsRequestedToTerminate(false)
{
}

void ReserveTimingAdjuster::Produce(const TrafficRecord &trafficRecord)
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

void ReserveTimingAdjuster::Run()
{
    auto threadCount = std::thread::hardware_concurrency();

    for (size_t i = 0; i < threadCount; i++)
    {
        _ThreadPool.push_back(std::thread([this]() {
            while (_IsRequestedToTerminate == false)
            {
                auto dequeueTimeout = std::chrono::milliseconds(1);
                auto reserveTimeRecord = _ReserveQueue->Dequeue(dequeueTimeout);
                if (!reserveTimeRecord.has_value())
                {
                    continue;
                }
                auto task = MakeTask(reserveTimeRecord.value());
                task(_Queue);
            }
        }));
    }

    for (auto &thread : _ThreadPool)
    {
        thread.join();
    }

    spdlog::debug("Reserve timing adjuster is terminated.");
}

void ReserveTimingAdjuster::TryTerminate()
{
    spdlog::debug("Reserve timing adjuster is requested to terminate.");
    _IsRequestedToTerminate = true;
}

std::function<void(std::shared_ptr<ThreadSafeQueue<TrafficRecord>>)> ReserveTimingAdjuster::MakeTask(
    const ReserveTimeRecord &reserveTimeRecord)
{
    return [reserveTimeRecord](std::shared_ptr<ThreadSafeQueue<TrafficRecord>> _Queue) {
        auto reservationTime = reserveTimeRecord.ReservationTime();
        std::this_thread::sleep_until(reservationTime);
        auto trafficRecord = TrafficRecord(reserveTimeRecord.Data(), std::chrono::nanoseconds(0));
        _Queue->Enqueue(trafficRecord);
    };
}
