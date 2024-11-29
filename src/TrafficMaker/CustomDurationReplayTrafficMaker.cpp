#include <TrafficMaker/CustomDurationReplayTrafficMaker.hpp>

namespace TrafficMaker
{

CustomDurationReplayTrafficMaker::CustomDurationReplayTrafficMaker(const std::string &pcapFilePath,
                                                                   std::chrono::milliseconds durationTime)
    : AbstractTrafficMaker(pcapFilePath), _DurationTime(durationTime)
{
}

double CustomDurationReplayTrafficMaker::CalculateSpeedScaleFactor(const std::vector<PcapRecord> &pcapRecords)
{
    auto timestamps = GetTimestamps(pcapRecords);
    auto maxTimestamp = *std::max_element(timestamps.begin(), timestamps.end());
    auto minTimestamp = *std::min_element(timestamps.begin(), timestamps.end());
    auto durationAsNsec = std::chrono::duration_cast<std::chrono::nanoseconds>(_DurationTime);
    auto speedScaleFactor = (maxTimestamp - minTimestamp).count() / static_cast<double>(durationAsNsec.count());

    return speedScaleFactor;
}

} // namespace TrafficMaker
