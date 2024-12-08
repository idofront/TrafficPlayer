#include <Dealer/RawSocketDealStrategy.hpp>
#include <boost/format.hpp>

RawSocketDealStrategy::RawSocketDealStrategy(const std::string &device_name) : device_name(device_name)
{
    try
    {
        PrepareSocket();
    }
    catch (const std::exception &e)
    {
        spdlog::critical(e.what());
        spdlog::info("Root permission is required to use raw socket.");
    }
}

RawSocketDealStrategy::~RawSocketDealStrategy()
{
    close(sockfd);
}

DealReportPtr RawSocketDealStrategy::Deal(const std::vector<uint8_t> &data)
{
    auto ready_time = std::chrono::system_clock::now();
    auto sent_time = std::chrono::system_clock::from_time_t(0);
    // Send data
    if (sendto(sockfd, data.data(), data.size(), 0, (struct sockaddr *)&device, sizeof(device)) < 0)
    {
        auto error = strerror(errno);
        auto fmt = boost::format("Failed to send data: %1%");
        spdlog::warn(boost::str(fmt % error));
    }
    else
    {
        sent_time = std::chrono::system_clock::now();
    }
    return std::make_shared<DealReport>(ready_time, sent_time, data);
}

void RawSocketDealStrategy::PrepareSocket()
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
