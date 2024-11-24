#include <TrafficMaker/UniformThroughputTrafficMaker.hpp>
#include <iostream>

namespace TrafficMaker
{
UniformTrafficMaker::UniformTrafficMaker(const std::filesystem::path &path, double expectedThroughputMbps)
    : ITrafficMaker(path), _expectedThroughputMbps(expectedThroughputMbps)
{
}

UniformTrafficMaker::~UniformTrafficMaker()
{
}

std::vector<TrafficRecord> UniformTrafficMaker::Make()
{
    auto pcapRecords = ReadPcapFile(Path());
    auto trafficRecords = std::vector<TrafficRecord>();
    auto expectedThroughputMbps = _expectedThroughputMbps;

    std::transform(pcapRecords.begin(), pcapRecords.end(), std::back_inserter(trafficRecords),
                   [expectedThroughputMbps](const PcapRecord &record) {
                       auto data = record.Data();
                       return TrafficRecord(data, expectedThroughputMbps);
                   });

    return trafficRecords;
}
} // namespace TrafficMaker
