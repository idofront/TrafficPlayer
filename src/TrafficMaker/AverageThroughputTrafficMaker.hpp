#ifndef TRAFFIC_MAKER__UNIFORM_TRAFFIC_MAKER_HPP
#define TRAFFIC_MAKER__UNIFORM_TRAFFIC_MAKER_HPP

#include <TrafficMaker/AbstractTrafficMaker.hpp>

namespace TrafficMaker
{
class AverageThroughputTrafficMaker : public AbstractTrafficMaker
{
  public:
    AverageThroughputTrafficMaker(const std::filesystem::path &path, double expectedThroughputMbps);
    virtual ~AverageThroughputTrafficMaker();

  protected:
    double CalculateSpeedScaleFactor(const std::vector<PcapRecord> &pcapRecords) override;

  private:
    double _expectedThroughputMbps;
};
} // namespace TrafficMaker

#endif
