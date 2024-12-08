#ifndef DEALER__DEAL_STRATEGY_HPP
#define DEALER__DEAL_STRATEGY_HPP

#include <Dealer/DealReport.hpp>
#include <vector>

class IDealStrategy
{
  public:
    virtual ~IDealStrategy() = default;
    virtual DealReportPtr Deal(const std::vector<uint8_t> &data) = 0;
};

#endif
