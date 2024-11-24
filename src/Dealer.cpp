#include <Dealer.hpp>
#include <numeric>

Dealer::Dealer(std::shared_ptr<ThreadSafeQueue<TrafficRecord>> queue, const std::string &device_name)
    : queue(queue), device_name(device_name), ReportsPtr(std::make_shared<ThreadSafeQueue<DealReport>>())
{
    PrepareSocket();
}

Dealer::~Dealer()
{
    close(sockfd);
}

void Dealer::operator()()
{
    while (true)
    {
        auto timeout = std::chrono::milliseconds(1000);
        auto result = queue->dequeue(timeout);

        if (result.has_value())
        {
            auto str = TrafficPlayer::HexString(result.value().Data());
            spdlog::trace("Deal: {}", str);
            try
            {
                Send(result.value().Data());
            }
            catch (const std::exception &e)
            {
                spdlog::error(e.what());
            }
        }
        else
        {
            spdlog::debug("Timeout: No value to consume.");
        }
    }
}

void Dealer::Send(const std::vector<uint8_t> &data)
{
    auto ready_time = std::chrono::system_clock::now();
    // データ送信
    if (sendto(sockfd, data.data(), data.size(), 0, (struct sockaddr *)&device, sizeof(device)) < 0)
    {
        auto error = strerror(errno);
        auto fmt = boost::format("Failed to send data: %1%");
        throw std::runtime_error(boost::str(fmt % error));
    }
    auto sent_time = std::chrono::system_clock::now();
    auto report = DealReport(ready_time, sent_time, data.size());
    ReportsPtr->enqueue(report);
}

void Dealer::PrepareSocket()
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

    // Set promiscuous mode
    struct packet_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.mr_ifindex = if_idx.ifr_ifindex;
    mreq.mr_type = PACKET_MR_PROMISC;
    if (setsockopt(sockfd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        throw std::runtime_error("Failed to set socket promiscuous mode");
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
