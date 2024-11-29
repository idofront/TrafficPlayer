#ifndef TIMING_ADJUSTER__RESERVE_TIME_RECORD_HPP
#define TIMING_ADJUSTER__RESERVE_TIME_RECORD_HPP

#include <chrono>
#include <vector>

class ReserveTimeRecord
{
  public:
    ReserveTimeRecord(std::vector<uint8_t> data, std::chrono::time_point<std::chrono::system_clock> reservationTime);
    const std::vector<uint8_t> &Data() const;
    const std::chrono::time_point<std::chrono::system_clock> &ReservationTime() const;
  private:
    std::chrono::time_point<std::chrono::system_clock> _ReservationTime;

    /// @brief Data to send
    std::vector<uint8_t> _Data;
};

#endif
