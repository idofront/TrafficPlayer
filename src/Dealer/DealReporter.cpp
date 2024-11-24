#include <Dealer/DealReporter.hpp>
#include <numeric>

DealReporter::DealReporter(Dealer &dealer, std::chrono::milliseconds interval) : _Dealer(dealer), _Interval(interval)
{
}

void DealReporter::operator()()
{
    auto showReportsReservation = std::chrono::system_clock::now() + _Interval;

    while (true)
    {
        auto rangeStart = std::chrono::system_clock::now();
        std::this_thread::sleep_until(showReportsReservation);

        // Reserve the next show time before processing the reports to avoid the delay.
        showReportsReservation = std::chrono::system_clock::now() + _Interval;

        auto reports = std::vector<DealReport>();
        while (!_Dealer.ReportsPtr->empty())
        {
            auto timeout = std::chrono::milliseconds(1000);
            auto report = _Dealer.ReportsPtr->dequeue(timeout);
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

        ShowReports(reports, rangeStart, rangeEnd);
    }
}

void DealReporter::ShowReports(const std::vector<DealReport> &reports,
                               std::chrono::time_point<std::chrono::system_clock> rangeStart,
                               std::chrono::time_point<std::chrono::system_clock> rangeEnd)
{
    auto range = std::chrono::duration_cast<std::chrono::milliseconds>(rangeEnd - rangeStart).count();
    auto rangeSecondsAsDouble = range / 1000.0;
    auto totalSize = std::accumulate(reports.begin(), reports.end(), 0,
                                     [](int sum, const DealReport &report) { return sum + report.PacketSize(); });
    auto totalSizeKiloBits = totalSize * 8 / 1000.0;
    auto throughput = totalSizeKiloBits / rangeSecondsAsDouble;
    auto packetPerSecond = reports.size() / rangeSecondsAsDouble;

    auto fmt = boost::format("Total: %d bytes, Throughput: %.3f Kbps, %.3f packets/s");
    spdlog::info(boost::str(fmt % totalSize % throughput % packetPerSecond));
}
