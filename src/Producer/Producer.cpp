#include <Producer/Producer.hpp>

Producer::Producer(std::shared_ptr<IThreadSafeQueue<ReserveTimeRecord>> inputQueue,
                   std::shared_ptr<IThreadSafeQueue<TrafficRecord>> outputQueue)
    : _InputQueue(inputQueue), _OutputQueue(outputQueue)
{
}

void Producer::Task()
{
    while (IsContinue())
    {
        auto dequeueTimeout = std::chrono::milliseconds(1);
        auto reserveTimeRecordOpt = _InputQueue->Dequeue(dequeueTimeout);
        if (!reserveTimeRecordOpt.has_value())
        {
            continue;
        }

        {
            auto reserveTimeRecord = reserveTimeRecordOpt.value();
            auto reservationTime = reserveTimeRecord.ReservationTime();
            SleepUntil(reservationTime);
            // When woken up, check this task should continue.
            if (!IsContinue())
            {
                continue;
            }

            auto trafficRecord = TrafficRecord(reserveTimeRecord.Data(), std::chrono::nanoseconds(0));
            _OutputQueue->Enqueue(trafficRecord);
        }
    }
}
