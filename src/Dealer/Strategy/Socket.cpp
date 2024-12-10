#include <Dealer/Strategy/Socket.hpp>
#include <arpa/inet.h>
#include <boost/format.hpp>
#include <netinet/ether.h>
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket(const std::string &device_name)
{
    try
    {
        PrepareSocket(sockfd, if_idx, device, device_name);
    }
    catch (const std::exception &e)
    {
        spdlog::critical(e.what());
        spdlog::info("Root permission is required to use raw socket.");
    }
}
Socket::~Socket()
{
    close(sockfd);
}

int Socket::Sockfd() const
{
    return sockfd;
}

struct ifreq Socket::IfIdx() const
{
    return if_idx;
}

struct sockaddr_ll Socket::Device() const
{
    return device;
}

std::string Socket::DeviceName() const
{
    return device_name;
}

void Socket::PrepareSocket(int &sockfd, struct ifreq &if_idx, struct sockaddr_ll &device,
                           const std::string &device_name)
{
    // Create socket
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0)
    {
        throw std::runtime_error("Failed to create socket");
    }
    spdlog::debug("Socket created");

    // Get interface index
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, device_name.c_str(), IFNAMSIZ - 1);
    auto ioctl_result = ioctl(sockfd, SIOCGIFINDEX, &if_idx);
    if (ioctl_result < 0)
    {
        auto fmt = boost::format("ioctl failed with error: %1%");
        auto msg = boost::str(fmt % strerror(errno));
        throw std::runtime_error(msg);
    }

    // Set device
    memset(&device, 0, sizeof(device));
    device.sll_family = AF_PACKET;
    device.sll_ifindex = if_idx.ifr_ifindex;
    device.sll_protocol = htons(ETH_P_ALL);

    // Bind socket
    if (bind(sockfd, (struct sockaddr *)&device, sizeof(device)) < 0)
    {
        throw std::runtime_error("Failed to bind socket");
    }
}
