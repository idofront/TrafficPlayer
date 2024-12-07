#ifndef THREAD__THREAD_POOL_HPP
#define THREAD__THREAD_POOL_HPP

#include <Thread/Future.hpp>
#include <Thread/Worker.hpp>
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
    virtual std::shared_ptr<Future> Submit(std::shared_ptr<Runnable> runnablePtr);

    /// @brief Get the number of active threads
    /// @return The number of active threads
    virtual std::size_t ActiveThreadCount() const;

    /// @brief Get the number of threads in the thread pool
    virtual std::size_t ThreadCount() const;

    /// @brief Try to terminate all threads
    virtual void TryTerminate();

    /// @brief Wait for all threads to finish
    virtual void Wait();

  private:
    std::vector<WorkerPtr> _Workers;
    std::vector<std::thread> _Threads;
    std::vector<std::shared_ptr<Future>> _Futures;
    mutable std::mutex _Mutex;
    mutable std::mutex _FuturesMutex;
    std::condition_variable _TerminatedCondition;
    RunnableQueuePtr _RunnableQueuePtr;
};
} // namespace Thread

#endif
