#ifndef THREAD__EMPLOYER_HPP
#define THREAD__EMPLOYER_HPP

#include <Thread/ThreadPool.hpp>
#include <thread>

namespace Thread
{
class Employer
{
  public:
    Employer() : _ThreadPool(std::thread::hardware_concurrency())
    {
    }

    virtual ~Employer() = default;

    virtual std::optional<Future> Submit(std::unique_ptr<Runnable> runnablePtr)
    {
        return _ThreadPool.Submit(std::move(runnablePtr));
    }

    virtual std::size_t ActiveThreadCount() const
    {
        return _ThreadPool.ActiveThreadCount();
    }

    virtual std::size_t ThreadCount() const
    {
        return _ThreadPool.ThreadCount();
    }

  private:
    ThreadPool _ThreadPool;
};
} // namespace Thread

#endif
