#ifndef DEALER_HPP
#define DEALER_HPP

#include <Dealer/DealReport.hpp>
#include <Queue/ThreadSafeQueue.hpp>
#include <Thread/Runnable.hpp>
#include <TrafficRecord.hpp>
#include <linux/if_packet.h>
#include <net/if.h>

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
