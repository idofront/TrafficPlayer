#ifndef TRAFFIC_RECORD_HPP
#define TRAFFIC_RECORD_HPP

#include <chrono>
#include <cstddef>
#include <vector>

class TrafficRecord
{
  public:
    TrafficRecord(std::vector<uint8_t> data, double expectedThroughput)
        : _ExpextedThroughput(expectedThroughput), _Data(data)
    {
        _PacketSize = data.size();
        _ShouldSpendTimeSending =
            std::chrono::nanoseconds(static_cast<long long>(_PacketSize * 8 / _ExpextedThroughput * 1e3));
    }

    const std::vector<uint8_t> &Data() const
    {
        return _Data;
    }

    const std::chrono::nanoseconds &ShouldSpendTimeSending() const
    {
        return _ShouldSpendTimeSending;
    }

  private:
    /// @brief Expected throughput in Mbps
    double _ExpextedThroughput;

    /// @brief Packet size in bytes
    std::size_t _PacketSize;

    /// @brief Time to spend sending the packet
    std::chrono::nanoseconds _ShouldSpendTimeSending;

    /// @brief Data to send
    std::vector<uint8_t> _Data;
};

#endif
