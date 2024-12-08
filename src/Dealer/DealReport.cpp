#include <Dealer/DealReport.hpp>

DealReport::DealReport(std::chrono::time_point<std::chrono::system_clock> ready_time,
                       std::chrono::time_point<std::chrono::system_clock> sent_time, const std::vector<uint8_t> &data)
    : _ReadyTime(ready_time), _SentTime(sent_time), _Data(data)
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

const std::vector<uint8_t> DealReport::Data() const
{
    return _Data;
}

std::size_t DealReport::PacketSize() const
{
    return _Data.size();
}

double DealReport::SentThroughput() const
{
    return PacketSize() * 8 / std::chrono::duration_cast<std::chrono::milliseconds>(_SentTime - _ReadyTime).count();
}
