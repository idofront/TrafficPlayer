#ifndef TRAFFIC_MAKER__PACKETS_PER_SECOND_TRAFFIC_MAKER_HPP
#define TRAFFIC_MAKER__PACKETS_PER_SECOND_TRAFFIC_MAKER_HPP

#include <TrafficMaker/AbstractTrafficMaker.hpp>

namespace TrafficMaker
{
class PacketsPerSecondTrafficMaker : public AbstractTrafficMaker
{
  public:
    PacketsPerSecondTrafficMaker(std::string pcapFilePath, double packetsPerSecond);

  protected:
    double CalculateSpeedScaleFactor(const std::vector<PcapRecord> &pcapRecords) override;

  private:
    std::string _PcapFilePath;
    double _PacketsPerSecond;
};
} // namespace TrafficMaker

#endif
