#ifndef TRAFFIC_MAKER__CUSTOM_DURATION_REPLAY_TRAFFIC_MAKER_HPP
#define TRAFFIC_MAKER__CUSTOM_DURATION_REPLAY_TRAFFIC_MAKER_HPP

#include <TrafficMaker/ITrafficMaker.hpp>
#include <chrono>

namespace TrafficMaker
{
class CustomDurationReplayTrafficMaker : public ITrafficMaker
{
  public:
    CustomDurationReplayTrafficMaker(const std::string &pcapFilePath, std::chrono::milliseconds durationTime);
    std::vector<TrafficRecord> Make() override;

  private:
    std::chrono::milliseconds _DurationTime;
};
} // namespace TrafficMaker

#endif
