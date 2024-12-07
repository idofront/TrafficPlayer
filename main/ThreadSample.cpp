#include <Thread/Employer.hpp>
#include <Thread/Runnable.hpp>
#include <spdlog/spdlog.h>

namespace Thread
{
class SampleRunner : public Runnable
{
  public:
    SampleRunner() : _Counter(0)
    {
    }

    virtual void PreTask() override
    {
        spdlog::info("PreTask");
    }

    virtual void Task() override
    {
        Sleep(std::chrono::milliseconds(100));
        spdlog::info("Task: {}", _Counter++);
    }

    virtual void PostTask() override
    {
        spdlog::info("PostTask");
    }

    int Counter() const
    {
        return _Counter;
    }

  private:
    std::uint64_t _Counter;
};
} // namespace Thread

int main()
{
    Thread::Employer employer;
    auto futures = {employer.Submit(std::make_shared<Thread::SampleRunner>()),
                    employer.Submit(std::make_shared<Thread::SampleRunner>())};

    // Check the submission
    for (auto &future : futures)
    {
        if (!future)
        {
            spdlog::error("Failed to submit the task");
            return 1;
        }
    }

    // Wait for a while
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Get the result
    for (auto &future : futures)
    {
        future->TryTerminate();
        auto result = future->Get(std::chrono::milliseconds(1000));

        if (result.has_value())
        {
            // In this implementation, could not get the result.
        }
        else
        {
            spdlog::error("Task did not finish");
        }
    }

    auto activeThreadcount = employer.ActiveThreadCount();
    spdlog::info("Active thread count: {}", activeThreadcount);

    spdlog::info("Try to terminate all threads");
    employer.TryTerminate();

    spdlog::info("Wait for all threads to finish");
    employer.Wait();

    return 0;
}
