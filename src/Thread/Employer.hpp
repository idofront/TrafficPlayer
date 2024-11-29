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

  private:
    ThreadPool _ThreadPool;
};
} // namespace Thread

#endif
