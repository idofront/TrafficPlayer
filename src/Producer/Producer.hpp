#ifndef PRODUCER__PRODUCER_HPP
#define PRODUCER__PRODUCER_HPP

#include <Thread/Runnable.hpp>
#include <TimingAdjuster/ReserveTimeRecord.hpp>
#include <TrafficRecord.hpp>

class Producer : public Thread::Runnable
{
  public:
    Producer(std::shared_ptr<IThreadSafeQueue<ReserveTimeRecord>> inputQueue,
             std::shared_ptr<IThreadSafeQueue<TrafficRecord>> outputQueue);
    virtual void Task() override;

  private:
    std::shared_ptr<IThreadSafeQueue<ReserveTimeRecord>> _InputQueue;
    std::shared_ptr<IThreadSafeQueue<TrafficRecord>> _OutputQueue;
};

#endif
