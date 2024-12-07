#include <Thread/Employer.hpp>

namespace Thread
{
Employer::Employer() : ThreadPool(std::thread::hardware_concurrency())
{
}
Employer::Employer(std::size_t threadCount) : ThreadPool(threadCount)
{
}
} // namespace Thread
