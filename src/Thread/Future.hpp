#ifndef THREAD__FUTURE_HPP
#define THREAD__FUTURE_HPP

#include <Thread/Runnable.hpp>
#include <memory>
#include <optional>

namespace Thread
{
class Future
{
  public:
    Future(std::shared_ptr<Runnable> runnablePtr) : _RunnablePtr(runnablePtr)
    {
    }
    /// @brief Wait for the thread to finish
    /// @param timeout
    /// @return If the thread is not finished after the timeout, return false.
    virtual bool Wait(std::chrono::milliseconds timeout)
    {
        // TODO: Implement the function
        return false;
    }

    /// @brief Try to terminate the thread
    virtual void TryTerminate()
    {
        _RunnablePtr->TryTerminate();
    }

    /// @brief Get the runnable object.
    /// @return The runnable object if the thread is finished, otherwise return nullopt.
    virtual std::optional<RunnableSharedPtr> Get(std::chrono::milliseconds timeout)
    {
        // Wait for the thread to finish
        auto wait = Wait(std::chrono::milliseconds(timeout));

        // If the thread is not finished after the timeout, return nullopt
        return wait ? std::make_optional(_RunnablePtr) : std::nullopt;
    }

  private:
    std::shared_ptr<Runnable> _RunnablePtr;
};
} // namespace Thread

#endif
