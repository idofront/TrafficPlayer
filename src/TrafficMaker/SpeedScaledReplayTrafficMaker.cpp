#include <TrafficMaker/SpeedScaledReplayTrafficMaker.hpp>

namespace TrafficMaker
{
SpeedScaleRepalyTrafficMaker::SpeedScaleRepalyTrafficMaker(const std::string &pcapFilePath, double speedScaleFactor)
    : AbstractTrafficMaker(pcapFilePath), _SpeedScaleFactor(speedScaleFactor)
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
