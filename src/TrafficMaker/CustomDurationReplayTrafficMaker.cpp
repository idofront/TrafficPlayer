#include <TrafficMaker/CustomDurationReplayTrafficMaker.hpp>
#include <algorithm>
#include <numeric>
#include <spdlog/spdlog.h>

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
    auto speedScaleFactor = (maxTimestamp - minTimestamp) / durationAsNsec;

    return speedScaleFactor;
}

} // namespace TrafficMaker
