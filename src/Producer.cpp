#include <Producer.hpp>

Producer::Producer(std::shared_ptr<TrafficPlayer::ThreadSafeQueue<TrafficRecord>> queue) : queue(queue)
{
}

void Producer::Produce(const TrafficRecord &trafficRecord)
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

void Producer::operator()()
{
}
