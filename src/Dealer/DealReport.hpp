#ifndef DEALER__DEAL_REPORT_HPP
#define DEALER__DEAL_REPORT_HPP

#include <chrono>
#include <memory>
#include <vector>

class DealReport
{
  public:
    DealReport(std::chrono::time_point<std::chrono::system_clock> ready_time,
               std::chrono::time_point<std::chrono::system_clock> sent_time, const std::vector<uint8_t> &data);
    std::chrono::time_point<std::chrono::system_clock> ReadyTime() const;
    std::chrono::time_point<std::chrono::system_clock> SentTime() const;
    const std::vector<uint8_t> Data() const;
    std::size_t PacketSize() const;
    double SentThroughput() const;

  private:
    std::chrono::time_point<std::chrono::system_clock> _ReadyTime;
    std::chrono::time_point<std::chrono::system_clock> _SentTime;
    std::vector<uint8_t> _Data;
};

using DealReportPtr = std::shared_ptr<DealReport>;

#endif
