#include <TrafficMaker/UniformThroughputTrafficMaker.hpp>
#include <iostream>
#include <numeric>
#include <spdlog/spdlog.h>
namespace TrafficMaker
{
UniformTrafficMaker::UniformTrafficMaker(const std::filesystem::path &path, double expectedThroughputMbps)
    : AbstractTrafficMaker(path), _expectedThroughputMbps(expectedThroughputMbps)
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

    auto totalSize = std::accumulate(pcapRecords.begin(), pcapRecords.end(), 0,
                                     [](int sum, const PcapRecord &record) { return sum + record.Len(); });
    auto expectedDuration = totalSize * 8 / (expectedThroughputMbps * 1e6);

    auto usecTimestamps = std::vector<uint64_t>();
    std::transform(pcapRecords.begin(), pcapRecords.end(), std::back_inserter(usecTimestamps),
                   [](const PcapRecord &record) { return record.Ts().tv_sec * 1e6 + record.Ts().tv_usec; });
    auto maxTimestamp = *std::max_element(usecTimestamps.begin(), usecTimestamps.end());
    auto minTimestamp = *std::min_element(usecTimestamps.begin(), usecTimestamps.end());
    auto totalDuration = (maxTimestamp - minTimestamp) / 1e6;

    auto speedScaleFactor = totalDuration / expectedDuration;

    for (auto i = 0; i < pcapRecords.size(); i++)
    {
        auto targetRecord = pcapRecords[i];
        auto targetUsecTimestamp = targetRecord.Ts().tv_sec * 1e6 + targetRecord.Ts().tv_usec;
        auto scaledTargetUsecTimestamp = targetUsecTimestamp / speedScaleFactor;
        auto nextUsecTimeStamp = (i + 1 < pcapRecords.size())
                                     ? pcapRecords[i + 1].Ts().tv_sec * 1e6 + pcapRecords[i + 1].Ts().tv_usec
                                     : maxTimestamp;
        auto scaledNextUsecTimestamp = nextUsecTimeStamp / speedScaleFactor;

        auto scaledChronoNano = std::chrono::nanoseconds(
            static_cast<long long>((scaledNextUsecTimestamp - scaledTargetUsecTimestamp) * 1e3));

        auto trafficRecord = TrafficRecord(targetRecord.Data(), scaledChronoNano);
        trafficRecords.push_back(trafficRecord);
    }

    return trafficRecords;
}
} // namespace TrafficMaker
