#include <Thread/Employer.hpp>

namespace Thread
{
Employer::Employer() : ThreadPool(std::thread::hardware_concurrency())
{
}
} // namespace Thread
