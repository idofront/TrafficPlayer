#ifndef THREAD__RUNNABLE_HPP
#define THREAD__RUNNABLE_HPP

#include <Queue/IThreadSafeQueue.hpp>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <vector>

namespace Thread
{
/// @brief Runnable interface
class Runnable
{
  public:
    Runnable();

    /// @brief Run the task
    virtual void Run() final;

    /// @brief Try to terminate the task gracefully.
    virtual void TryTerminate() final;

    /// @brief Check if the task should continue
    /// @return If the task should continue, return true.
    bool IsContinue();

    /// @brief Register the callback function to be called when the task is finished.
    /// @param callback The callback function to be called.
    virtual void RegisterCallback(std::function<void()> callback) final;

    bool IsTerminated() const;

  protected:
    /// @brief Sleep for a while.
    /// @param duration The duration to sleep.
    /// @note The sleep can be interrupted if requested to terminate. In the implementation of Task(), this function
    /// should be called instead of std::this_thread::sleep_for().
    virtual void Sleep(std::chrono::milliseconds duration);

    /// @brief Sleep until the given time point.
    /// @param timePoint The time point to sleep until.
    /// @note The sleep can be interrupted if requested to terminate. In the implementation of Task(), this function
    /// should be called instead of std::this_thread::sleep_until().
    virtual void SleepUntil(std::chrono::time_point<std::chrono::system_clock> timePoint);

    /// @brief Pre-task
    /// @note This function is called before the task is executed.
    virtual void PreTask();

    /// @brief The task to be executed
    /// @note This function should be implemented by the derived class. If the task needs to wait for a while, use
    /// Sleep() to sleep.
    virtual void Task() = 0;

    /// @brief Post-task
    /// @note This function is called after the task is executed.
    virtual void PostTask();

  private:
    bool ShouldBeTerminated();

    std::atomic<bool> _IsRequestedToTerminate;
    std::mutex _Mutex;
    std::condition_variable _SleepCondition;

    /// @brief Notify the callback functions
    void NotifyCallbacks();

    /// @brief Callback functions to be called when the task is finished.
    std::vector<std::function<void()>> _Callbacks;

    bool _IsTerminated;
};

using RunnablePtr = std::shared_ptr<Runnable>;
using RunnableQueue = IThreadSafeQueue<RunnablePtr>;
using RunnableQueuePtr = std::shared_ptr<RunnableQueue>;
} // namespace Thread

#endif
