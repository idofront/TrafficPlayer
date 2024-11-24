#ifndef TRAFFIC_PLAYER_HPP
#define TRAFFIC_PLAYER_HPP

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <vector>

namespace TrafficPlayer
{
inline std::string HexString(const std::vector<uint8_t> &data)
{
    std::stringstream ss;
    for (size_t i = 0; i < data.size(); i++)
    {
        if (i % 16 == 0)
        {
            ss << std::endl;
        }
        ss << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(data[i]) << " ";
    }
    return ss.str();
}
} // namespace TrafficPlayer

#endif
