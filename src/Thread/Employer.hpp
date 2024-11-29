#ifndef THREAD__EMPLOYER_HPP
#define THREAD__EMPLOYER_HPP

#include <Thread/ThreadPool.hpp>
#include <thread>

namespace Thread
{
/// @brief Employer class to manage the thread pool and interface for submitting tasks
class Employer
{
  public:
    Employer() : _ThreadPool(std::thread::hardware_concurrency())
    {
    }

    virtual ~Employer() = default;

    /// @brief Submit a runnable to the thread pool
    /// @param runnablePtr The runnable to be submitted
    /// @return Future object to control the thread. If no thread is available, return nullopt.
    virtual std::optional<Future> Submit(std::unique_ptr<Runnable> runnablePtr)
    {
        return _ThreadPool.Submit(std::move(runnablePtr));
    }

    /// @brief Get the number of active threads
    /// @return The number of active threads
    virtual std::size_t ActiveThreadCount() const
    {
        return _ThreadPool.ActiveThreadCount();
    }

    /// @brief Get the number of threads in the thread pool
    /// @return The number of threads in the thread pool
    virtual std::size_t ThreadCount() const
    {
        return _ThreadPool.ThreadCount();
    }

  private:
    ThreadPool _ThreadPool;
};
} // namespace Thread

#endif
