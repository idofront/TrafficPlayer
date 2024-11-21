#ifndef PRODUCER_HPP
#define PRODUCER_HPP

#include <ThreadSafeQueue.hpp>
#include <TrafficPlayer.hpp>
#include <TrafficRecord.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>

class Producer
{
  public:
    Producer(std::shared_ptr<TrafficPlayer::ThreadSafeQueue<TrafficRecord>> queue);

    void Produce(const std::vector<uint8_t> &data, double expectedThroughput)
    {
        auto trafficRecord = TrafficRecord(data, expectedThroughput);

        auto shouldWait = trafficRecord.ShouldSpendTimeSending();
        auto shouldWaitUntil = std::chrono::system_clock::now() + shouldWait;

        if (!queue->empty())
        {
            SPDLOG_WARN("Queue is not empty. Probably consumer is not consuming fast enough.");
        }

        queue->enqueue(trafficRecord);
        SPDLOG_INFO("Produced: {}", TrafficPlayer::HexString(trafficRecord.Data()));

        auto shouldWaitMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(shouldWait);
        SPDLOG_INFO("Should wait for {} milliseconds", shouldWaitMilliseconds.count());
        std::this_thread::sleep_until(shouldWaitUntil);
    }

    void operator()()
    {
    }

  private:
    std::shared_ptr<TrafficPlayer::ThreadSafeQueue<TrafficRecord>> queue;
};

#endif
