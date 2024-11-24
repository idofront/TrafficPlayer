#include <TrafficMaker/SpeedScaledReplayTrafficMaker.hpp>

namespace TrafficMaker
{
SpeedScaleRepalyTrafficMaker::SpeedScaleRepalyTrafficMaker(const std::string &pcapFilePath, double speedScaleFactor)
    : ITrafficMaker(pcapFilePath), _SpeedScaleFactor(speedScaleFactor)
{
}

std::vector<TrafficRecord> SpeedScaleRepalyTrafficMaker::Make()
{
    auto pcapRecords = ReadPcapFile(Path());
    auto trafficRecords = std::vector<TrafficRecord>();
    auto speedScaleFactor = _SpeedScaleFactor;

    auto usecTimestamps = std::vector<uint64_t>();
    std::transform(pcapRecords.begin(), pcapRecords.end(), std::back_inserter(usecTimestamps),
                   [](const PcapRecord &record) { return record.Ts().tv_sec * 1e6 + record.Ts().tv_usec; });

    auto maxTimestamp = *std::max_element(usecTimestamps.begin(), usecTimestamps.end());
    auto minTimestamp = *std::min_element(usecTimestamps.begin(), usecTimestamps.end());

    std::transform(pcapRecords.begin(), pcapRecords.end(), std::back_inserter(trafficRecords),
                   [minTimestamp, maxTimestamp, speedScaleFactor](const PcapRecord &record) {
                       auto data = record.Data();
                       auto usecTimeval = record.Ts().tv_sec * 1e6 + record.Ts().tv_usec;

                       auto relativeUsecTimestamp = usecTimeval - minTimestamp;
                       auto expectedInterval = relativeUsecTimestamp / speedScaleFactor;

                       auto scaledChronoNano = std::chrono::nanoseconds(static_cast<long long>(expectedInterval * 1e3));
                       return TrafficRecord(data, scaledChronoNano);
                   });

    return trafficRecords;
}
} // namespace TrafficMaker
