#ifndef TRAFFIC_MAKER__SCALED_INTERVAL_TRAFFIC_MAKER_HPP
#define TRAFFIC_MAKER__SCALED_INTERVAL_TRAFFIC_MAKER_HPP

#include <TrafficMaker/ITrafficMaker.hpp>
#include <spdlog/spdlog.h>

namespace TrafficMaker
{
class ScaledIntervalTrafficMaker : public ITrafficMaker
{
  public:
    ScaledIntervalTrafficMaker(const std::string &pcapFilePath, double intervalScaleFactor);
    std::vector<TrafficRecord> Make() override;

  private:
    double _intervalScaleFactor;
};
} // namespace TrafficMaker

#endif
