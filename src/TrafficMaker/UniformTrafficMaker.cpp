#include <TrafficMaker/UniformTrafficMaker.hpp>
#include <iostream>

namespace TrafficMaker
{
UniformTrafficMaker::UniformTrafficMaker(const std::filesystem::path &path) : ITrafficMaker(path)
{
}

UniformTrafficMaker::~UniformTrafficMaker()
{
}

std::vector<TrafficRecord> UniformTrafficMaker::Make()
{
    auto pcapRecords = ReadPcapFile(Path());
    auto trafficRecords = std::vector<TrafficRecord>();

    std::transform(pcapRecords.begin(), pcapRecords.end(), std::back_inserter(trafficRecords),
                   [](const PcapRecord &record) {
                       auto data = record.Data();
                       auto expectedThroughputMbps = 0.001;
                       return TrafficRecord(data, expectedThroughputMbps);
                   });

    return trafficRecords;
}
} // namespace TrafficMaker
