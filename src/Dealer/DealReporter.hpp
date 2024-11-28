#ifndef DEALER__DEAL_REPORTER_HPP
#define DEALER__DEAL_REPORTER_HPP

#include <Dealer/Dealer.hpp>
#include <Queue/ThreadSafeQueue.hpp>
#include <UnitConverter/BinaryPrefixConversion.hpp>
#include <UnitConverter/NoConversion.hpp>
#include <UnitConverter/SiPrefixConversion.hpp>
#include <UnitConverter/UnitConverter.hpp>
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
                            std::chrono::time_point<std::chrono::system_clock> rangeEnd,
                            const UnitConverter &_UnitConverter);
    UnitConverter _UnitConverter;
};

#endif
