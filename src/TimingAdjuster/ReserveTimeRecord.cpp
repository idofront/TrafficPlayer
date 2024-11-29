#include <TimingAdjuster/ReserveTimeRecord.hpp>

ReserveTimeRecord::ReserveTimeRecord(std::vector<uint8_t> data,
                                     std::chrono::time_point<std::chrono::system_clock> reservationTime)
    : _Data(data), _ReservationTime(reservationTime)
{
}

const std::vector<uint8_t> &ReserveTimeRecord::Data() const
{
    return _Data;
}

const std::chrono::time_point<std::chrono::system_clock> &ReserveTimeRecord::ReservationTime() const
{
    return _ReservationTime;
}
