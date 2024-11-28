#ifndef PRODUCER_HPP
#define PRODUCER_HPP

#include <Queue/ThreadSafeQueue.hpp>
#include <TrafficPlayer.hpp>
#include <TrafficRecord.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>

class TransmissionTimingAdjuster
{
  public:
    TransmissionTimingAdjuster(std::shared_ptr<ThreadSafeQueue<TrafficRecord>> queue);

    void Produce(const TrafficRecord &trafficRecord);

    void operator()();

  private:
    std::shared_ptr<ThreadSafeQueue<TrafficRecord>> queue;
};

#endif
