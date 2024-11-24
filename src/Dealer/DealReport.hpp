#ifndef DEALER__DEAL_REPORT_HPP
#define DEALER__DEAL_REPORT_HPP

#include <chrono>

class DealReport
{
  public:
    DealReport(std::chrono::time_point<std::chrono::system_clock> ready_time,
               std::chrono::time_point<std::chrono::system_clock> sent_time, std::size_t packet_size);
    std::chrono::time_point<std::chrono::system_clock> ReadyTime() const;
    std::chrono::time_point<std::chrono::system_clock> SentTime() const;
    std::size_t PacketSize() const;
    double SentThroughput() const;

  private:
    std::chrono::time_point<std::chrono::system_clock> _ReadyTime;
    std::chrono::time_point<std::chrono::system_clock> _SentTime;
    std::size_t _PacketSize;
};

#endif
