#ifndef TRAFFIC_RECORD_HPP
#define TRAFFIC_RECORD_HPP

#include <chrono>
#include <cstddef>
#include <vector>

class TrafficRecord
{
  public:
    TrafficRecord(std::vector<uint8_t> data, std::chrono::nanoseconds shouldSpendTimeSending)
        : _Data(data), _ShouldSpendTimeSending(shouldSpendTimeSending)
    {
        _PacketSize = data.size();
        _ShouldSpendTimeSending = shouldSpendTimeSending;
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
    /// @brief Packet size in bytes
    std::size_t _PacketSize;

    /// @brief Time to spend sending the packet
    std::chrono::nanoseconds _ShouldSpendTimeSending;

    /// @brief Data to send
    std::vector<uint8_t> _Data;
};

#endif
