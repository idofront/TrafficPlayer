#ifndef THREAD__RUNNABLE_HPP
#define THREAD__RUNNABLE_HPP

#include <chrono>
#include <condition_variable>
#include <memory>

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

    /// @brief Join the thread
    virtual void Join();

  protected:
    /// @brief Sleep for a while.
    /// @param duration The duration to sleep.
    /// @note The sleep can be interrupted if requested to terminate. In the implementation of Task(), this function
    /// should be called instead of std::this_thread::sleep_for().
    virtual void Sleep(std::chrono::milliseconds duration);

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
    bool IsContinue();
    bool ShouldBeTerminated();

    std::atomic<bool> _IsRequestedToTerminate;
    std::thread _Thread;
    std::mutex _Mutex;
    std::condition_variable _SleepCondition;
};

using RunnableSharedPtr = std::shared_ptr<Runnable>;
} // namespace Thread

#endif
