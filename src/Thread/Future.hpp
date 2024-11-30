#ifndef THREAD__FUTURE_HPP
#define THREAD__FUTURE_HPP

#include <Thread/Runnable.hpp>
#include <functional>
#include <memory>
#include <optional>

namespace Thread
{
/// @brief Future class for managing Runnable tasks
class Future
{
  public:
    Future(std::shared_ptr<Runnable> runnablePtr);

    /// @brief Wait for the thread to finish
    /// @param timeout
    /// @return If the thread is not finished after the timeout, return false.
    virtual bool Wait(std::chrono::milliseconds timeout);

    /// @brief Try to terminate the thread
    virtual void TryTerminate();

    /// @brief Get the runnable object.
    /// @return The runnable object if the thread is finished, otherwise return nullopt.
    virtual std::optional<std::shared_ptr<Runnable>> Get(std::chrono::milliseconds timeout);

    /// @brief Register the callback function to be called when the thread is finished.
    /// @param callback The callback function to be called.
    virtual void RegisterCallback(std::function<void()> callback);

  private:
    std::shared_ptr<Runnable> _RunnablePtr;
    std::mutex _Mutex;
    std::condition_variable _ConditionVar;

    /// @brief Callback functions to be called when the thread is finished.
    std::vector<std::function<void()>> _Callbacks;

    /// @brief Notify the callback functions
    void NotifyCallbacks();
};
} // namespace Thread

#endif
