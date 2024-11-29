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
    virtual std::vector<TrafficRecord> Make();

  protected:
    std::filesystem::path Path() const;
    virtual std::vector<PcapRecord> ReadPcapFile(const std::filesystem::path &path) final;
    virtual double CalculateSpeedScaleFactor(const std::vector<PcapRecord> &pcapRecords) = 0;
    static std::vector<std::chrono::nanoseconds> NormalizeTimestamps(
        const std::vector<std::chrono::nanoseconds> &records, double speedScaleFactor);
    static std::vector<std::chrono::nanoseconds> RelativizeTimestamps(
        const std::vector<std::chrono::nanoseconds> &timestamps);
    static std::vector<TrafficRecord> MakeTrafficRecords(const std::vector<PcapRecord> &pcapRecords,
                                                         const double speedScaleFactor);
    static std::vector<std::chrono::nanoseconds> GetTimestamps(std::vector<PcapRecord> pcapRecords);

  private:
    std::filesystem::path _path;
};
} // namespace TrafficMaker

#endif
