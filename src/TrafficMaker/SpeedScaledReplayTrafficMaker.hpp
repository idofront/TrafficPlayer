#ifndef TRAFFIC_MAKER__SPEED_SCALED_REPLAY_TRAFFIC_MAKER_HPP
#define TRAFFIC_MAKER__SPEED_SCALED_REPLAY_TRAFFIC_MAKER_HPP

#include <TrafficMaker/AbstractTrafficMaker.hpp>
#include <spdlog/spdlog.h>

namespace TrafficMaker
{
class SpeedScaleReplayTrafficMaker : public AbstractTrafficMaker
{
  public:
    SpeedScaleReplayTrafficMaker(const std::string &pcapFilePath, double speedScaleFactor);

  protected:
    double CalculateSpeedScaleFactor(const std::vector<PcapRecord> &pcapRecords) override;

  private:
    double _SpeedScaleFactor;
};
} // namespace TrafficMaker

#endif
