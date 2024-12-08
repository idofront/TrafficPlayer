#include <Dealer/DealReporter.hpp>
#include <UnitConverter/SiPrefixConversion.hpp>
#include <boost/format.hpp>
#include <numeric>
#include <spdlog/spdlog.h>

DealReporter::DealReporter(std::chrono::milliseconds interval)
    : _Interval(interval), _UnitConverter(std::make_unique<SiPrefixConversion>()), _IsRequestedToTerminate(false),
      _Reports(std::make_shared<ThreadSafeQueue<DealReportPtr>>())
{
}

void DealReporter::PreTask()
{
    spdlog::debug("Deal reporter is started.");
    auto _ShowReportsReservation = std::chrono::system_clock::now() + _Interval;
}

void DealReporter::Task()
{
    if (_Interval.count() <= 0)
    {
        spdlog::info("A deal reporter is disabled. The interval is zero or negative.");
        return;
    }

    auto rangeStart = std::chrono::system_clock::now();
    this->SleepUntil(_ShowReportsReservation);

    // Reserve the next show time before processing the reports to avoid the delay.
    _ShowReportsReservation = std::chrono::system_clock::now() + _Interval;

    auto reports = std::vector<DealReportPtr>();
    while (!_Reports->Empty())
    {
        auto timeout = std::chrono::milliseconds(1000);
        auto report = _Reports->Dequeue(timeout);
        if (report.has_value())
        {
            reports.push_back(report.value());
        }
        else
        {
            spdlog::warn("Deal reporter timeout. No value to consume, but the dealer's report queue is not empty.");
            continue;
        }
    }
    auto rangeEnd = std::chrono::system_clock::now();

    ShowReports(reports, rangeStart, rangeEnd, _UnitConverter);
}

void DealReporter::PostTask()
{
    spdlog::debug("Deal reporter is terminated.");
}

void DealReporter::RegisterDealer(std::shared_ptr<Dealer> dealer)
{
    dealer->RegisterDealedCallback([this](DealReportPtr report) { this->_Reports->Enqueue(report); });
}

void DealReporter::ShowReports(const std::vector<DealReportPtr> &reports,
                               std::chrono::time_point<std::chrono::system_clock> rangeStart,
                               std::chrono::time_point<std::chrono::system_clock> rangeEnd,
                               const UnitConverter &_UnitConverter)
{
    auto range = std::chrono::duration_cast<std::chrono::nanoseconds>(rangeEnd - rangeStart).count();
    auto rangeSecondsAsDouble = range / 1'000'000'000.0;
    auto totalSize = std::accumulate(reports.begin(), reports.end(), 0,
                                     [](int sum, const DealReportPtr &report) { return sum + report->PacketSize(); });
    auto totalSizeBits = totalSize * 8;
    auto throughput = totalSizeBits / rangeSecondsAsDouble;
    auto packetPerSecond = reports.size() / rangeSecondsAsDouble;

    auto totalSizeConversionResult = _UnitConverter.Convert(totalSize);
    auto throughputConversionResult = _UnitConverter.Convert(throughput);
    auto packetPerSecondConversionResult = _UnitConverter.Convert(packetPerSecond);

    auto fmt = boost::format("[Report] Total: %7.3f %sB, Throughput: %7.3f %sbps, %7.3f %spackets/s");
    spdlog::info(boost::str(fmt % totalSizeConversionResult.ConvertedValue % totalSizeConversionResult.Unit %
                            throughputConversionResult.ConvertedValue % throughputConversionResult.Unit %
                            packetPerSecondConversionResult.ConvertedValue % packetPerSecondConversionResult.Unit));
}
