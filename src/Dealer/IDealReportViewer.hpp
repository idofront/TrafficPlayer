#ifndef DEALER__IDEAL_REPORT_VIEWER_HPP
#define DEALER__IDEAL_REPORT_VIEWER_HPP

#include <Dealer/Dealer.hpp>
#include <memory>

class IDealReportViewer
{
  public:
    virtual ~IDealReportViewer() = default;
    void RegisterDealer(std::shared_ptr<Dealer> dealer)
    {
        dealer->RegisterDealedCallback([this](DealReportPtr reportPtr) {
            std::lock_guard<std::mutex> lock(_Mutex);
            this->Show(reportPtr);
        });
    }

  protected:
    std::mutex _Mutex;
    virtual void Show(const DealReportPtr reportPtr) = 0;
};

#endif
