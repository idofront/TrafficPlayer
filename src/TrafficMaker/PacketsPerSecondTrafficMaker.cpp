#include <TrafficMaker/PacketsPerSecondTrafficMaker.hpp>

namespace TrafficMaker
{
PacketsPerSecondTrafficMaker::PacketsPerSecondTrafficMaker(std::string pcapFilePath, double packetsPerSecond)
    : AbstractTrafficMaker(pcapFilePath), _PacketsPerSecond(packetsPerSecond)
{
}

double PacketsPerSecondTrafficMaker::CalculateSpeedScaleFactor(const std::vector<PcapRecord> &pcapRecords)
{
    auto packetsPerSecond = _PacketsPerSecond;

    auto timestamps = GetTimestamps(pcapRecords);
    auto maxTimestamp = *std::max_element(timestamps.begin(), timestamps.end());
    auto minTimestamp = *std::min_element(timestamps.begin(), timestamps.end());

    auto packetNumber = pcapRecords.size();
    auto actualDuration = maxTimestamp - minTimestamp;
    auto expectedDuration = (packetNumber / packetsPerSecond) * 1e9;
    auto speedScaleFactor = actualDuration.count() / expectedDuration;

    return speedScaleFactor;
}
} // namespace TrafficMaker
