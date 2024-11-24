#include <TrafficMaker/ITrafficMaker.hpp>

namespace TrafficMaker
{
ITrafficMaker::ITrafficMaker(const std::filesystem::path &path) : _path(path)
{
}

ITrafficMaker::~ITrafficMaker()
{
}

std::filesystem::path ITrafficMaker::Path() const
{
    return _path;
}

std::vector<PcapRecord> ITrafficMaker::ReadPcapFile(const std::filesystem::path &path)
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
} // namespace TrafficMaker
