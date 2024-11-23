#ifndef TRAFFIC_MAKER__I_TRAFFIC_MAKER_HPP
#define TRAFFIC_MAKER__I_TRAFFIC_MAKER_HPP

#include <TrafficMaker/PcapRecord.hpp>
#include <TrafficRecord.hpp>
#include <filesystem>
#include <pcap.h>
#include <vector>

namespace TrafficMaker
{
class ITrafficMaker
{
  public:
    ITrafficMaker(const std::filesystem::path &path);
    virtual ~ITrafficMaker() = 0;
    virtual std::vector<TrafficRecord> Make() = 0;

  protected:
    std::filesystem::path Path() const
    {
        return _path;
    }

    virtual std::vector<PcapRecord> ReadPcapFile(const std::filesystem::path &path) final
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

        return pcapRecords;
    }

  private:
    std::filesystem::path _path;
};
} // namespace TrafficMaker

#endif
