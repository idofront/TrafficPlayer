#ifndef THREAD__THREAD_POOL_HPP
#define THREAD__THREAD_POOL_HPP

#include <Thread/Future.hpp>
#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>

namespace Thread
{
class ThreadPool
{
  public:
    ThreadPool(std::size_t threadCount);
    virtual ~ThreadPool();

    /// @brief Submit a runnable to the thread pool
    /// @param runnablePtr
    /// @return Future object to control the thread. If no thread is available, return nullopt.
    virtual std::optional<Future> Submit(std::shared_ptr<Runnable> runnablePtr);

    /// @brief Get the number of active threads
    /// @return The number of active threads
    virtual std::size_t ActiveThreadCount() const;

    /// @brief Get the number of threads in the thread pool
    virtual std::size_t ThreadCount() const;

  private:
    std::vector<std::optional<std::thread>> _Threads;
    mutable std::mutex _Mutex;
};
} // namespace Thread

#endif
