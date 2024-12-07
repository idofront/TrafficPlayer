#ifndef THREAD__EMPLOYER_HPP
#define THREAD__EMPLOYER_HPP

#include <Thread/ThreadPool.hpp>
#include <thread>

namespace Thread
{
/// @brief Employer class to manage the thread pool and interface for submitting tasks
class Employer : public ThreadPool
{
  public:
    Employer();
    Employer(std::size_t threadCount);
};
} // namespace Thread

#endif
