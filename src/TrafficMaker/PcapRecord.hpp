#ifndef TRAFFIC_MAKER__PCAP_RECORD_HPP
#define TRAFFIC_MAKER__PCAP_RECORD_HPP

#include <pcap.h>
#include <vector>

namespace TrafficMaker
{
class PcapRecord
{
  public:
    PcapRecord(const pcap_pkthdr &header, std::vector<uint8_t> &data)
        : _data(data), _caplen(header.caplen), _len(header.len), _ts(header.ts)
    {
    }

    const std::vector<uint8_t> Data() const
    {
        return _data;
    }

    uint32_t Caplen() const
    {
        return _caplen;
    }

    uint32_t Len() const
    {
        return _len;
    }

    struct timeval Ts() const
    {
        return _ts;
    }

  private:
    std::vector<uint8_t> _data;
    uint32_t _caplen;
    uint32_t _len;
    struct timeval _ts;
};

} // namespace TrafficMaker
#endif
