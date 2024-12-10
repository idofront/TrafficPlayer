#ifndef DEALER__STRATEGY__DEAL_STRATEGY_HPP
#define DEALER__STRATEGY__DEAL_STRATEGY_HPP

#include <Dealer/DealReport.hpp>
#include <vector>

using Packet = std::vector<uint8_t>;
class IDealStrategy
{
  public:
    virtual ~IDealStrategy() = default;
    virtual DealReportPtr Deal(const Packet &data) = 0;
    virtual std::vector<DealReportPtr> Deal(const std::vector<Packet> &data)
    {
        auto reports = std::vector<DealReportPtr>();
        std::transform(data.begin(), data.end(), std::back_inserter(reports),
                       [this](const Packet &packet) { return Deal(packet); });
        return reports;
    }
};

#endif
