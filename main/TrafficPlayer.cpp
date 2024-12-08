#include <Dealer/DealReporter.hpp>
#include <Dealer/Dealer.hpp>
#include <ParseOptions.hpp>
#include <Producer/Producer.hpp>
#include <Queue/ThreadSafeQueue.hpp>
#include <Thread/Employer.hpp>
#include <TimingAdjuster/ReserveTimingAdjuster.hpp>
#include <TrafficPlayer.hpp>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[])
{
    try
    {
        auto options = ParseOptions(argc, argv);
        spdlog::set_level(options.LogLevel());

        auto interface = options.InterfaceName();
        auto reportIntervalMsec =
            std::chrono::milliseconds(static_cast<uint64_t>(options.ReportIntervalNsec().count() / 1e6));

        auto queuePtr = std::make_shared<ThreadSafeQueue<TrafficRecord>>();
        auto reserveTimeQueuePtr = std::make_shared<BoundedThreadSafeQueue<ReserveTimeRecord>>(1024);

        auto employer = Thread::Employer(NUM_OF_THREADS);
        auto dealerPtr = std::make_shared<Dealer>(queuePtr, interface);
        auto dealerFuturePtr = employer.Submit(dealerPtr);
        auto reporterPtr = std::make_shared<DealReporter>(reportIntervalMsec);
        reporterPtr->RegisterDealer(dealerPtr);
        auto reporterFuturePtr = employer.Submit(reporterPtr);

        // Create producers
        auto producerFuturePtrs = CreateProducers(employer, reserveTimeQueuePtr, queuePtr);

        // Adjust and push to the queue that is wating for the producer to send.
        AdjustTrafficRecords(options, reserveTimeQueuePtr);

        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } while (!queuePtr->Empty());

        // TODO: Wait for all packets to be sent.
        // This implementation is forceful and may cause packet loss.
        try
        {
            employer.TryTerminate();
            employer.Wait();
            spdlog::info("All threads are terminated.");
        }
        catch (const std::exception &e)
        {
            throw;
        }
    }
    catch (const std::exception &e)
    {
        spdlog::critical(e.what());
        return 1;
    }
    catch (...)
    {
        spdlog::critical("Unknown error");
        return 1;
    }

    return 0;
}
