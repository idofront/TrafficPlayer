#include <TransmissionTimingAdjuster.hpp>

TransmissionTimingAdjuster::TransmissionTimingAdjuster(std::shared_ptr<ThreadSafeQueue<TrafficRecord>> queue)
    : queue(queue)
{
}

void TransmissionTimingAdjuster::Produce(const TrafficRecord &trafficRecord)
{
    auto shouldWait = trafficRecord.ShouldSpendTimeSending();
    auto shouldWaitUntil = std::chrono::system_clock::now() + shouldWait;

    if (!queue->empty())
    {
        spdlog::warn("Queue is not empty. Probably consumer is not consuming fast enough.");
    }

    queue->enqueue(trafficRecord);
    spdlog::trace("Produced: {}", TrafficPlayer::HexString(trafficRecord.Data()));

    auto shouldWaitMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(shouldWait);
    spdlog::debug("Should wait for {} milliseconds", shouldWaitMilliseconds.count());
    std::this_thread::sleep_until(shouldWaitUntil);
}

void TransmissionTimingAdjuster::operator()()
{
}
