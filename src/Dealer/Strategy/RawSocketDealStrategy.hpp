#ifndef DEALER__STRATEGY__RAW_SOCKET_DEAL_STRATEGY_HPP
#define DEALER__STRATEGY__RAW_SOCKET_DEAL_STRATEGY_HPP

#include <Dealer/Strategy/IDealStrategy.hpp>
#include <Dealer/Strategy/Socket.hpp>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <spdlog/spdlog.h>
#include <string.h>
#include <sys/ioctl.h>

class RawSocketDealStrategy : public IDealStrategy
{
  public:
    RawSocketDealStrategy(const std::string &device_name);
    virtual ~RawSocketDealStrategy();
    virtual DealReportPtr Deal(const std::vector<uint8_t> &data) override;

  private:
    Socket _Socket;
    const std::string device_name;
};

#endif
