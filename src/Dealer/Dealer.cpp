#include <Dealer/Dealer.hpp>
#include <Dealer/DryDealStrategy.hpp>
#include <numeric>
#include <spdlog/spdlog.h>

Dealer::Dealer(std::shared_ptr<ThreadSafeQueue<TrafficRecord>> queue, const std::string &device_name)
    : queue(queue), _DealStrategyPtr(std::make_shared<DryDealStrategy>())
{
}

Dealer::~Dealer()
{
}

void Dealer::Task()
{
    while (IsContinue())
    {
        auto timeout = std::chrono::milliseconds(1000);
        auto result = queue->Dequeue(timeout);

        if (result.has_value())
        {
            auto report = Send(result.value().Data());

            // Notify callbacks
            // TODO Callbacks should be executed in another thread because we don't know how long it will take
            NotifyDealedCallbacks(report);
        }
        else
        {
            spdlog::debug("Timeout: No value to consume.");
        }
    }

    spdlog::debug("Dealer is terminated.");
}

void Dealer::RegisterDealedCallback(std::function<void(DealReportPtr)> callback)
{
    _DealedCallbacks.push_back(callback);
}

void Dealer::NotifyDealedCallbacks(DealReportPtr &report)
{
    std::for_each(_DealedCallbacks.begin(), _DealedCallbacks.end(),
                  [&report](const std::function<void(DealReportPtr)> &callback) { callback(report); });
}

void Dealer::DealStrategy(std::shared_ptr<IDealStrategy> dealStrategy)
{
    auto lock = std::lock_guard(_StrategyMutex);
    _DealStrategyPtr = dealStrategy;
}

std::shared_ptr<DealReport> Dealer::Send(const std::vector<uint8_t> &data)
{
    auto lock = std::lock_guard(_StrategyMutex);
    return _DealStrategyPtr->Deal(data);
}
