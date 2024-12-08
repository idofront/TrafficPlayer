#ifndef DEALER__DEAL_REPORT_HEX_VIEWER_HPP
#define DEALER__DEAL_REPORT_HEX_VIEWER_HPP

#include <Dealer/IDealReportViewer.hpp>
#include <boost/algorithm/string/join.hpp>
#include <numeric>
#include <spdlog/spdlog.h>

class DealReportHexViewer : public IDealReportViewer
{
  public:
    DealReportHexViewer() = default;
    virtual ~DealReportHexViewer() = default;

  protected:
    virtual void Show(const DealReportPtr reportPtr) override
    {
        auto hex = std::vector<std::string>();
        auto data = reportPtr->Data();
        std::transform(data.begin(), data.end(), std::back_inserter(hex),
                       [this](const uint8_t &data) { return FromByteToHexString(data, ""); });

        auto bytesOnLine = 16;
        auto sentDate = std::chrono::system_clock::to_time_t(reportPtr->SentTime());
        auto hexLines = std::vector<std::string>{"Size: " + std::to_string(data.size())};
        auto hexLine = std::string();
        for (auto i = 0; i < hex.size(); i++)
        {
            hexLine += hex[i];
            if ((i + 1) % bytesOnLine == 0)
            {
                hexLines.push_back(hexLine);
                hexLine.clear();
            }
            else
            {
                hexLine += " ";
            }
        }
        // last line if not filled
        if (!hexLine.empty())
        {
            hexLines.push_back(hexLine);
        }

        auto resultString = boost::algorithm::join(hexLines, "\n");
        spdlog::info(resultString);
    }

  private:
    std::string FromByteToHexString(const uint8_t &data, const std::string prefix = "0x") const
    {
        static const char hex[] = "0123456789ABCDEF";
        auto highFourBits = hex[(data & 0xF0) >> 4];
        auto LowFourBits = hex[data & 0x0F];
        return prefix + highFourBits + LowFourBits;
    }
};

#endif
