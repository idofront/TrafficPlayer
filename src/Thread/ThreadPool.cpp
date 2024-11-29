#include <Thread/ThreadPool.hpp>

namespace Thread
{
ThreadPool::ThreadPool(std::size_t threadCount)
{
    // Thread pool initialization
    for (std::size_t i = 0; i < threadCount; i++)
    {
        _Threads.push_back(std::nullopt);
    }
}

ThreadPool::~ThreadPool() = default;

std::optional<Future> ThreadPool::Submit(std::unique_ptr<Runnable> runnablePtr)
{
    if (!runnablePtr)
    {
        return std::nullopt;
    }

    // Get lock for the thread pool
    auto lock = std::lock_guard(_Mutex);

    // Move the runnable to the local variable
    auto moveRunnablePtr = std::move(runnablePtr);
    runnablePtr = nullptr;

    // Convert to shared pointer
    auto sharedRunnablePtr = std::shared_ptr<Runnable>(moveRunnablePtr.release());

    // Find an available thread
    auto availableThreadIter = std::find_if(
        _Threads.begin(), _Threads.end(), [](const std::optional<std::thread> &thread) { return !thread.has_value(); });

    // No available thread
    if (availableThreadIter == _Threads.end())
    {
        // Move the shared pointer back to the unique pointer
        runnablePtr.reset(sharedRunnablePtr.get());
        return std::nullopt;
    }

    // Assign the runnable to the thread
    auto function = std::bind(&Runnable::Run, sharedRunnablePtr);
    *availableThreadIter = std::thread(function);

    // Return the future object
    return Future(sharedRunnablePtr);
}

std::size_t ThreadPool::ActiveThreadCount() const
{
    // Get lock for the thread pool
    auto lock = std::lock_guard(_Mutex);

    // Count the number of active threads
    auto count = std::count_if(_Threads.begin(), _Threads.end(),
                               [](const std::optional<std::thread> &thread) { return thread.has_value(); });

    return count;
}

std::size_t ThreadPool::ThreadCount() const
{
    return _Threads.size();
}
}; // namespace Thread
