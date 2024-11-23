#ifndef CONSUMER_HPP
#define CONSUMER_HPP

#include <ThreadSafeQueue.hpp>
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

class Consumer
{
  public:
    Consumer(std::shared_ptr<TrafficPlayer::ThreadSafeQueue<TrafficRecord>> queue, const std::string &device_name);
    virtual ~Consumer();

    void operator()();

  private:
    std::shared_ptr<TrafficPlayer::ThreadSafeQueue<TrafficRecord>> queue;

    int sockfd;
    struct ifreq if_idx;
    struct sockaddr_ll device;
    const std::string device_name;

    void Send(const std::vector<uint8_t> &data);
    void PrepareSocket();
};

#endif
