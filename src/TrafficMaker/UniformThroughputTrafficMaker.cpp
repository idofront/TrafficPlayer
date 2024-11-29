#include <TrafficMaker/UniformThroughputTrafficMaker.hpp>
#include <numeric>

namespace TrafficMaker
{
UniformTrafficMaker::UniformTrafficMaker(const std::filesystem::path &path, double expectedThroughputMbps)
    : AbstractTrafficMaker(path), _expectedThroughputMbps(expectedThroughputMbps)
{
}

UniformTrafficMaker::~UniformTrafficMaker()
{
}

double UniformTrafficMaker::CalculateSpeedScaleFactor(const std::vector<PcapRecord> &pcapRecords)
{
    auto expectedThroughputMbps = _expectedThroughputMbps;

    auto totalSize = std::accumulate(pcapRecords.begin(), pcapRecords.end(), 0,
                                     [](int sum, const PcapRecord &record) { return sum + record.Len(); });
    auto expectedDuration =
        std::chrono::nanoseconds(static_cast<uint64_t>(totalSize * 8 / (expectedThroughputMbps * 1e6) * 1e9));

    auto timestamps = GetTimestamps(pcapRecords);
    auto maxTimestamp = *std::max_element(timestamps.begin(), timestamps.end());
    auto minTimestamp = *std::min_element(timestamps.begin(), timestamps.end());
    auto actualDuration = maxTimestamp - minTimestamp;

    auto speedScaleFactor = actualDuration.count() / static_cast<double>(expectedDuration.count());

    return speedScaleFactor;
}
} // namespace TrafficMaker
