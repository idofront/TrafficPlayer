#ifndef TRAFFIC_MAKER__ABSTRACT_TRAFFIC_MAKER_HPP
#define TRAFFIC_MAKER__ABSTRACT_TRAFFIC_MAKER_HPP

#include <TrafficMaker/PcapRecord.hpp>
#include <TrafficRecord.hpp>
#include <filesystem>
#include <pcap.h>
#include <vector>

namespace TrafficMaker
{
class AbstractTrafficMaker
{
  public:
    AbstractTrafficMaker(const std::filesystem::path &path);
    virtual ~AbstractTrafficMaker() = 0;
    virtual std::vector<TrafficRecord> Make() = 0;

  protected:
    std::filesystem::path Path() const;
    virtual std::vector<PcapRecord> ReadPcapFile(const std::filesystem::path &path) final;

  private:
    std::filesystem::path _path;
};
} // namespace TrafficMaker

#endif
