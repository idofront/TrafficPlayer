#include <Dealer/DealReporter.hpp>
#include <numeric>

DealReporter::DealReporter(Dealer &dealer, std::chrono::milliseconds interval)
    : _Dealer(dealer), _Interval(interval), _UnitConverter(std::make_unique<SiPrefixConversion>()),
      _IsRequestedToTerminate(false)
{
}

void DealReporter::Run()
{
    if (_Interval.count() <= 0)
    {
        spdlog::info("A deal reporter is disabled. The interval is zero or negative.");
        return;
    }

    auto showReportsReservation = std::chrono::system_clock::now() + _Interval;

    while (_IsRequestedToTerminate == false)
    {
        auto rangeStart = std::chrono::system_clock::now();
        std::this_thread::sleep_until(showReportsReservation);

        // Reserve the next show time before processing the reports to avoid the delay.
        showReportsReservation = std::chrono::system_clock::now() + _Interval;

        auto reports = std::vector<DealReport>();
        while (!_Dealer.ReportsPtr->Empty())
        {
            auto timeout = std::chrono::milliseconds(1000);
            auto report = _Dealer.ReportsPtr->Dequeue(timeout);
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

    spdlog::debug("Deal reporter is terminated.");
}

void DealReporter::TryTerminate()
{
    spdlog::debug("Deal reporter is requested to terminate.");
    _IsRequestedToTerminate = true;
}

void DealReporter::ShowReports(const std::vector<DealReport> &reports,
                               std::chrono::time_point<std::chrono::system_clock> rangeStart,
                               std::chrono::time_point<std::chrono::system_clock> rangeEnd,
                               const UnitConverter &_UnitConverter)
{
    auto range = std::chrono::duration_cast<std::chrono::nanoseconds>(rangeEnd - rangeStart).count();
    auto rangeSecondsAsDouble = range / 1'000'000'000.0;
    auto totalSize = std::accumulate(reports.begin(), reports.end(), 0,
                                     [](int sum, const DealReport &report) { return sum + report.PacketSize(); });
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
