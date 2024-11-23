#ifndef TRAFFIC_MAKER__UNIFORM_TRAFFIC_MAKER_HPP
#define TRAFFIC_MAKER__UNIFORM_TRAFFIC_MAKER_HPP

#include <TrafficMaker/ITrafficMaker.hpp>

namespace TrafficMaker
{
class UniformTrafficMaker : public ITrafficMaker
{
  public:
    UniformTrafficMaker(const std::filesystem::path &path);
    virtual ~UniformTrafficMaker();
    virtual std::vector<TrafficRecord> Make();
};
} // namespace TrafficMaker

#endif
