#ifndef DEALER_HPP
#define DEALER_HPP

#include <Dealer/DealReport.hpp>
#include <Dealer/IDealStrategy.hpp>
#include <Queue/ThreadSafeQueue.hpp>
#include <Thread/Runnable.hpp>
#include <TrafficRecord.hpp>
#include <mutex>

class Dealer : public Thread::Runnable
{
  public:
    Dealer(std::shared_ptr<ThreadSafeQueue<TrafficRecord>> queue, const std::string &device_name);

    virtual ~Dealer();

    virtual void Task();

    void RegisterDealedCallback(std::function<void(DealReportPtr)> callback);

    void DealStrategy(std::shared_ptr<IDealStrategy> dealStrategy);

  private:
    std::mutex _StrategyMutex;
    std::shared_ptr<ThreadSafeQueue<TrafficRecord>> queue;

    std::vector<std::function<void(DealReportPtr)>> _DealedCallbacks;
    void NotifyDealedCallbacks(DealReportPtr &report);

    std::shared_ptr<DealReport> Send(const std::vector<uint8_t> &data);
    std::shared_ptr<IDealStrategy> _DealStrategyPtr;
};

#endif
