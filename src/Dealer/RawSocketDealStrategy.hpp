#ifndef DEAL__RAW_SOCKET_DEAL_STRATEGY_HPP
#define DEAL__RAW_SOCKET_DEAL_STRATEGY_HPP

#include <Dealer/IDealStrategy.hpp>
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
    void PrepareSocket();

  private:
    int sockfd;
    struct ifreq if_idx;
    struct sockaddr_ll device;
    const std::string device_name;
};

#endif
