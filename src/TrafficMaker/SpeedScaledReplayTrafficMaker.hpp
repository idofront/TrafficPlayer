#ifndef TRAFFIC_MAKER__SPEED_SCALED_REPLAY_TRAFFIC_MAKER_HPP
#define TRAFFIC_MAKER__SPEED_SCALED_REPLAY_TRAFFIC_MAKER_HPP

#include <TrafficMaker/ITrafficMaker.hpp>
#include <spdlog/spdlog.h>

namespace TrafficMaker
{
class SpeedScaleRepalyTrafficMaker : public ITrafficMaker
{
  public:
    SpeedScaleRepalyTrafficMaker(const std::string &pcapFilePath, double speedScaleFactor);
    std::vector<TrafficRecord> Make() override;

  private:
    double _SpeedScaleFactor;
};
} // namespace TrafficMaker

#endif
