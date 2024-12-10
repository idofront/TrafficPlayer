#ifndef DEALER__STRATEGY__BATCH_SOCKET_DEAL_STRATEGY_HPP
#define DEALER__STRATEGY__BATCH_SOCKET_DEAL_STRATEGY_HPP

#include <Dealer/Strategy/IDealStrategy.hpp>
#include <Dealer/Strategy/Socket.hpp>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class BatchSocketDealStrategy : public IDealStrategy
{
  public:
    BatchSocketDealStrategy(const std::string &device_name) : _Socket(Socket(device_name)), device_name(device_name)
    {
    }

    ~BatchSocketDealStrategy()
    {
    }

    virtual DealReportPtr Deal(const Packet &data) override
    {
        throw std::runtime_error("Not implemented");
    }

    virtual std::vector<DealReportPtr> Deal(const std::vector<Packet> &data) override
    {
        throw std::runtime_error("Not implemented");
    }

  private:
    Socket _Socket;
    const std::string device_name;
};

#endif
