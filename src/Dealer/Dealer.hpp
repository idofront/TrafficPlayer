#ifndef DEALER_HPP
#define DEALER_HPP

#include <Dealer/DealReport.hpp>
#include <Queue/ThreadSafeQueue.hpp>
#include <Thread/Runnable.hpp>
#include <TrafficPlayer.hpp>
#include <TrafficRecord.hpp>
#include <arpa/inet.h>
#include <boost/format.hpp>
#include <linux/if_packet.h>
#include <memory>
#include <net/if.h>
#include <netinet/ether.h>
#include <spdlog/spdlog.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

class Dealer : public Thread::Runnable
{
  public:
    Dealer(std::shared_ptr<ThreadSafeQueue<TrafficRecord>> queue, const std::string &device_name);
    virtual ~Dealer();

    virtual void Task();
    std::shared_ptr<ThreadSafeQueue<DealReport>> ReportsPtr;

  private:
    std::shared_ptr<ThreadSafeQueue<TrafficRecord>> queue;

    int sockfd;
    struct ifreq if_idx;
    struct sockaddr_ll device;
    const std::string device_name;

    void Send(const std::vector<uint8_t> &data);
    void PrepareSocket();
};

#endif
