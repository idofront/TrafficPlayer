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

    void Produce(const std::vector<uint8_t> &data, double expectedThroughput);

    void operator()();

  private:
    std::shared_ptr<TrafficPlayer::ThreadSafeQueue<TrafficRecord>> queue;
};

#endif
