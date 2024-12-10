#include <Dealer/Strategy/DryDealStrategy.hpp>

DealReportPtr DryDealStrategy::Deal(const std::vector<uint8_t> &data)
{
    auto ready_time = std::chrono::system_clock::now();
    auto dealed_time = std::chrono::system_clock::from_time_t(0);
    return std::make_shared<DealReport>(ready_time, dealed_time, data);
}
