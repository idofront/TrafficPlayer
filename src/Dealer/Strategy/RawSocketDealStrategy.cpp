#include <Dealer/Strategy/RawSocketDealStrategy.hpp>
#include <boost/format.hpp>

RawSocketDealStrategy::RawSocketDealStrategy(const std::string &device_name)
    : _Socket(Socket(device_name)), device_name(device_name)
{
}

RawSocketDealStrategy::~RawSocketDealStrategy()
{
}

DealReportPtr RawSocketDealStrategy::Deal(const std::vector<uint8_t> &data)
{
    auto ready_time = std::chrono::system_clock::now();
    auto sent_time = std::chrono::system_clock::from_time_t(0);
    auto sockfd = _Socket.Sockfd();
    auto device = _Socket.Device();

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
