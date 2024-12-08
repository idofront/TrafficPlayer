#ifndef DEALER__DEAL_REPORTER_HPP
#define DEALER__DEAL_REPORTER_HPP

#include <Dealer/Dealer.hpp>
#include <Thread/Runnable.hpp>
#include <UnitConverter/UnitConverter.hpp>
#include <vector>

class DealReporter : public Thread::Runnable
{
  public:
    DealReporter(std::chrono::milliseconds interval);
    virtual ~DealReporter() = default;
    virtual void PreTask() override;
    virtual void Task() override;
    virtual void PostTask() override;
    virtual void RegisterDealer(std::shared_ptr<Dealer> dealer);

  private:
    std::chrono::milliseconds _Interval;
    static void ShowReports(const std::vector<DealReportPtr> &reports,
                            std::chrono::time_point<std::chrono::system_clock> rangeStart,
                            std::chrono::time_point<std::chrono::system_clock> rangeEnd,
                            const UnitConverter &_UnitConverter);
    UnitConverter _UnitConverter;
    bool _IsRequestedToTerminate;
    std::chrono::system_clock::time_point _ShowReportsReservation;
    std::shared_ptr<ThreadSafeQueue<DealReportPtr>> _Reports;
};

#endif
