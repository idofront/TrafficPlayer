#include <TrafficMaker/AbstractTrafficMaker.hpp>

namespace TrafficMaker
{
AbstractTrafficMaker::AbstractTrafficMaker(const std::filesystem::path &path) : _path(path)
{
}

AbstractTrafficMaker::~AbstractTrafficMaker()
{
}

std::filesystem::path AbstractTrafficMaker::Path() const
{
    return _path;
}

std::vector<PcapRecord> AbstractTrafficMaker::ReadPcapFile(const std::filesystem::path &path)
{
    auto pcapRecords = std::vector<PcapRecord>();

    auto pcap = pcap_open_offline(path.c_str(), NULL);
    if (pcap == NULL)
    {
        throw std::runtime_error("Failed to open pcap file");
    }

    struct pcap_pkthdr header;
    const u_char *packet;
    while ((packet = pcap_next(pcap, &header)) != NULL)
    {
        std::vector<uint8_t> data(packet, packet + header.len);
        auto record = PcapRecord(header, data);
        pcapRecords.push_back(record);
    }

    pcap_close(pcap);

    return pcapRecords;
}

std::vector<std::chrono::nanoseconds> AbstractTrafficMaker::NormalizeTimestamps(
    const std::vector<std::chrono::nanoseconds> &records, double speedScaleFactor)
{
    auto normalizedTimestamps = std::vector<std::chrono::nanoseconds>();
    std::transform(records.begin(), records.end(), std::back_inserter(normalizedTimestamps),
                   [speedScaleFactor](const std::chrono::nanoseconds &timestamp) {
                       return std::chrono::nanoseconds(static_cast<long long>(timestamp.count() / speedScaleFactor));
                   });
    return normalizedTimestamps;
}

std::vector<std::chrono::nanoseconds> AbstractTrafficMaker::RelativizeTimestamps(
    const std::vector<std::chrono::nanoseconds> &timestamps)
{
    if (timestamps.empty())
    {
        return {};
    }

    auto relativeTimestamps = std::vector<std::chrono::nanoseconds>{std::chrono::nanoseconds(0)};
    relativeTimestamps.reserve(timestamps.size());
    for (auto i = 1; i < timestamps.size(); i++)
    {
        relativeTimestamps.push_back(timestamps[i] - timestamps[i - 1]);
    }
    return relativeTimestamps;
}

std::vector<TrafficRecord> AbstractTrafficMaker::Make()
{
    auto pcapRecords = ReadPcapFile(Path());
    auto speedScaleFactor = CalculateSpeedScaleFactor(pcapRecords);
    return MakeTrafficRecords(pcapRecords, speedScaleFactor);
}
std::vector<TrafficRecord> AbstractTrafficMaker::MakeTrafficRecords(const std::vector<PcapRecord> &pcapRecords,
                                                                    const double speedScaleFactor)
{
    auto trafficRecords = std::vector<TrafficRecord>();

    auto nsecTimestamps = std::vector<std::chrono::nanoseconds>();
    std::transform(pcapRecords.begin(), pcapRecords.end(), std::back_inserter(nsecTimestamps),
                   [](const PcapRecord &record) {
                       auto nsec = (uint64_t)(record.Ts().tv_sec * 1e9 + record.Ts().tv_usec * 1e3);
                       return std::chrono::nanoseconds(nsec);
                   });

    auto relativizedTimestamps = RelativizeTimestamps(nsecTimestamps);
    auto normalizedTimestamps = NormalizeTimestamps(relativizedTimestamps, speedScaleFactor);

    for (auto i = 0; i < pcapRecords.size(); i++)
    {
        auto record = pcapRecords[i];
        auto timestamp = normalizedTimestamps[i];
        auto trafficRecord = TrafficRecord(record.Data(), timestamp);
        trafficRecords.push_back(trafficRecord);
    }

    return trafficRecords;
}

std::vector<std::chrono::nanoseconds> AbstractTrafficMaker::GetTimestamps(std::vector<PcapRecord> pcapRecords)
{
    auto nsecTimestamps = std::vector<std::chrono::nanoseconds>();
    std::transform(pcapRecords.begin(), pcapRecords.end(), std::back_inserter(nsecTimestamps),
                   [](const PcapRecord &record) {
                       auto nsec = static_cast<uint64_t>(record.Ts().tv_sec * 1e9 + record.Ts().tv_usec * 1e3);
                       return std::chrono::nanoseconds(nsec);
                   });
    return nsecTimestamps;
}
} // namespace TrafficMaker
