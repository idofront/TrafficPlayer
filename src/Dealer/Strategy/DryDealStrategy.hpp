#ifndef DEALER__STRATEGY__DRY_DEAL_STRATEGY_HPP
#define DEALER__STRATEGY__DRY_DEAL_STRATEGY_HPP

#include <Dealer/Strategy/IDealStrategy.hpp>
#include <spdlog/spdlog.h>

class DryDealStrategy : public IDealStrategy
{
  public:
    DryDealStrategy() = default;
    virtual ~DryDealStrategy() = default;
    virtual DealReportPtr Deal(const std::vector<uint8_t> &data) override;
};

#endif
