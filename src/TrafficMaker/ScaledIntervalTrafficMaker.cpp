#include <TrafficMaker/ScaledIntervalTrafficMaker.hpp>

namespace TrafficMaker
{
ScaledIntervalTrafficMaker::ScaledIntervalTrafficMaker(const std::string &pcapFilePath, double intervalScaleFactor)
    : ITrafficMaker(pcapFilePath), _intervalScaleFactor(intervalScaleFactor)
{
}

std::vector<TrafficRecord> ScaledIntervalTrafficMaker::Make()
{
    auto pcapRecords = ReadPcapFile(Path());
    auto trafficRecords = std::vector<TrafficRecord>();
    auto intervalScaleFactor = _intervalScaleFactor;

    auto usecTimestamps = std::vector<uint64_t>();
    std::transform(pcapRecords.begin(), pcapRecords.end(), std::back_inserter(usecTimestamps),
                   [](const PcapRecord &record) { return record.Ts().tv_sec * 1e6 + record.Ts().tv_usec; });

    auto maxTimestamp = *std::max_element(usecTimestamps.begin(), usecTimestamps.end());
    auto minTimestamp = *std::min_element(usecTimestamps.begin(), usecTimestamps.end());

    std::transform(pcapRecords.begin(), pcapRecords.end(), std::back_inserter(trafficRecords),
                   [minTimestamp, maxTimestamp, intervalScaleFactor](const PcapRecord &record) {
                       auto data = record.Data();
                       auto usecTimeval = record.Ts().tv_sec * 1e6 + record.Ts().tv_usec;

                       auto relativeUsecTimestamp = usecTimeval - minTimestamp;
                       auto expectedInterval = relativeUsecTimestamp * intervalScaleFactor;

                       auto scaledChronoNano = std::chrono::nanoseconds(static_cast<long long>(expectedInterval * 1e3));
                       return TrafficRecord(data, scaledChronoNano);
                   });

    return trafficRecords;
}
} // namespace TrafficMaker
