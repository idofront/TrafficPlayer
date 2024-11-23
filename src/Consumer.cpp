#include <Consumer.hpp>

Consumer::Consumer(std::shared_ptr<TrafficPlayer::ThreadSafeQueue<TrafficRecord>> queue, const std::string &device_name)
    : queue(queue), device_name(device_name)
{
    PrepareSocket();
}

Consumer::~Consumer()
{
    close(sockfd);
}

void Consumer::operator()()
{
    while (true)
    {
        std::chrono::milliseconds timeout(1000);
        auto result = queue->dequeue(timeout);

        if (result.has_value())
        {
            auto str = TrafficPlayer::HexString(result.value().Data());
            SPDLOG_INFO("Consumed: {}", str);
            try
            {
                Send(result.value().Data());
            }
            catch (const std::exception &e)
            {
                SPDLOG_ERROR(e.what());
            }
        }
        else
        {
            SPDLOG_INFO("Timeout: No value to consume.");
        }
    }
}

void Consumer::Send(const std::vector<uint8_t> &data)
{
    // データ送信
    if (sendto(sockfd, data.data(), data.size(), 0, (struct sockaddr *)&device, sizeof(device)) < 0)
    {
        auto error = strerror(errno);
        auto fmt = boost::format("Failed to send data: %1%");
        throw std::runtime_error(boost::str(fmt % error));
    }
}

void Consumer::PrepareSocket()
{
    // Create socket
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0)
    {
        throw std::runtime_error("Failed to create socket");
    }
    SPDLOG_INFO("Socket created");

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
}
