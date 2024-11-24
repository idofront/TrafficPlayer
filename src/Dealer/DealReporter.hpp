#ifndef DEALER__DEAL_REPORTER_HPP
#define DEALER__DEAL_REPORTER_HPP

#include <Dealer/Dealer.hpp>
#include <ThreadSafeQueue.hpp>
#include <chrono>
#include <memory>

class DealReporter
{
  public:
    DealReporter(Dealer &dealer, std::chrono::milliseconds interval);
    virtual ~DealReporter() = default;
    void operator()();

  private:
    Dealer &_Dealer;
    std::chrono::milliseconds _Interval;
    static void ShowReports(const std::vector<DealReport> &reports,
                            std::chrono::time_point<std::chrono::system_clock> rangeStart,
                            std::chrono::time_point<std::chrono::system_clock> rangeEnd);
};

#endif
