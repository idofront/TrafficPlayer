#include <TrafficMaker/ITrafficMaker.hpp>

namespace TrafficMaker
{
ITrafficMaker::ITrafficMaker(const std::filesystem::path &path) : _path(path)
{
}
ITrafficMaker::~ITrafficMaker()

{
}
} // namespace TrafficMaker
