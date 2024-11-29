#include <TrafficMaker/SpeedScaledReplayTrafficMaker.hpp>

namespace TrafficMaker
{
SpeedScaleReplayTrafficMaker::SpeedScaleReplayTrafficMaker(const std::string &pcapFilePath, double speedScaleFactor)
    : AbstractTrafficMaker(pcapFilePath), _SpeedScaleFactor(speedScaleFactor)
{
}

double SpeedScaleReplayTrafficMaker::CalculateSpeedScaleFactor(const std::vector<PcapRecord> &pcapRecords)
{
    return _SpeedScaleFactor;
}

} // namespace TrafficMaker
