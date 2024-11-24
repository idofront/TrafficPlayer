#include <Dealer/DealReport.hpp>

DealReport::DealReport(std::chrono::time_point<std::chrono::system_clock> ready_time,
                       std::chrono::time_point<std::chrono::system_clock> sent_time, std::size_t packet_size)
    : _ReadyTime(ready_time), _SentTime(sent_time), _PacketSize(packet_size)
{
}

std::chrono::time_point<std::chrono::system_clock> DealReport::ReadyTime() const
{
    return _ReadyTime;
}

std::chrono::time_point<std::chrono::system_clock> DealReport::SentTime() const
{
    return _SentTime;
}

std::size_t DealReport::PacketSize() const
{
    return _PacketSize;
}

double DealReport::SentThroughput() const
{
    return _PacketSize * 8 / std::chrono::duration_cast<std::chrono::milliseconds>(_SentTime - _ReadyTime).count();
}
