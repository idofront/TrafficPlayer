#ifndef DEALER__DEAL_REPORT_ASCII_VIEWER_HPP
#define DEALER__DEAL_REPORT_ASCII_VIEWER_HPP

#include <Dealer/IDealReportViewer.hpp>
#include <boost/algorithm/string/join.hpp>
#include <numeric>
#include <spdlog/spdlog.h>

class DealReportAsciiViewer : public IDealReportViewer
{
  public:
    DealReportAsciiViewer() = default;
    virtual ~DealReportAsciiViewer() = default;

  protected:
    virtual void Show(const DealReportPtr reportPtr) override
    {
        auto hex = std::vector<uint8_t>();
        auto data = reportPtr->Data();
        std::transform(data.begin(), data.end(), std::back_inserter(hex),
                       [this](const uint8_t &data) { return RestrictPrintableCharacter(data); });

        auto sentDate = std::chrono::system_clock::to_time_t(reportPtr->SentTime());
        auto hexLines = std::vector<std::string>{"Size: " + std::to_string(data.size())};
        auto hexLine = std::string();
        for (auto i = 0; i < hex.size(); i++)
        {
            hexLine += hex[i];
            if ((i + 1) % _BytesOnLine == 0)
            {
                hexLines.push_back(hexLine);
                hexLine.clear();
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
    static const std::size_t _BytesOnLine = 16;
    static const uint8_t _DefaultChar = '.';
    static bool IsPrintableCharacter(const uint8_t &data)
    {
        return data >= 0x20 && data <= 0x7E;
    }
    static uint8_t RestrictPrintableCharacter(const uint8_t &data)
    {
        return IsPrintableCharacter(data) ? data : _DefaultChar;
    }
};

#endif
