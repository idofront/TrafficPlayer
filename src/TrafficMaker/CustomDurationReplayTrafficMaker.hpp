#ifndef TRAFFIC_MAKER__CUSTOM_DURATION_REPLAY_TRAFFIC_MAKER_HPP
#define TRAFFIC_MAKER__CUSTOM_DURATION_REPLAY_TRAFFIC_MAKER_HPP

#include <TrafficMaker/AbstractTrafficMaker.hpp>
#include <chrono>

namespace TrafficMaker
{
class CustomDurationReplayTrafficMaker : public AbstractTrafficMaker
{
  public:
    CustomDurationReplayTrafficMaker(const std::string &pcapFilePath, std::chrono::milliseconds durationTime);

  protected:
    double CalculateSpeedScaleFactor(const std::vector<PcapRecord> &pcapRecords) override;

  private:
    std::chrono::milliseconds _DurationTime;
};
} // namespace TrafficMaker

#endif
