#ifndef DEALER__STRATEGY__SOCKET_HPP
#define DEALER__STRATEGY__SOCKET_HPP

#include <linux/if_packet.h>
#include <net/if.h>
#include <string>

class Socket
{
  public:
    Socket(const std::string &device_name);
    ~Socket();

    // Delete copy constructor and copy assignment operator
    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;

    /// @brief socket file descriptor
    int Sockfd() const;

    /// @brief device index
    struct ifreq IfIdx() const;

    /// @brief device
    struct sockaddr_ll Device() const;

    /// @brief device name
    std::string DeviceName() const;

    void PrepareSocket(int &sockfd, struct ifreq &if_idx, struct sockaddr_ll &device, const std::string &device_name);
    int sockfd;
    struct ifreq if_idx;
    struct sockaddr_ll device;
    const std::string device_name;
};

#endif
