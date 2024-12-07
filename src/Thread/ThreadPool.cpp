#include <Queue/ThreadSafeQueue.hpp>
#include <Thread/ThreadPool.hpp>
#include <spdlog/spdlog.h>

namespace Thread
{
ThreadPool::ThreadPool(std::size_t threadCount) : _RunnableQueuePtr(std::make_shared<ThreadSafeQueue<RunnablePtr>>())
{
    for (std::size_t i = 0; i < threadCount; i++)
    {
        auto worker = std::make_shared<Worker>(_RunnableQueuePtr);
        auto thread = std::thread([worker]() { worker->Run(); });
        _Workers.push_back(worker);
        _Threads.push_back(std::move(thread));
    }
}

ThreadPool::~ThreadPool() = default;

std::shared_ptr<Future> ThreadPool::Submit(std::shared_ptr<Runnable> runnablePtr)
{
    if (!runnablePtr)
    {
        return nullptr;
    }

    // A future object must be made before enqueuing the task.
    auto futurePtr = std::make_shared<Future>(runnablePtr);
    _Futures.push_back(futurePtr);
    futurePtr->RegisterCallback([this, futurePtr]() {
        auto futuresLock = std::lock_guard(_FuturesMutex);
        _Futures.erase(std::remove(_Futures.begin(), _Futures.end(), futurePtr), _Futures.end());
        _TerminatedCondition.notify_all();
    });

    _RunnableQueuePtr->Enqueue(runnablePtr);

    return futurePtr;
}

std::size_t ThreadPool::ActiveThreadCount() const
{
    return std::count_if(_Workers.begin(), _Workers.end(),
                         [](const WorkerPtr &worker) { return !worker->IsTerminated(); });
}

std::size_t ThreadPool::ThreadCount() const
{
    return _Threads.size();
}

void ThreadPool::TryTerminate()
{
    // Get lock for the thread pool
    {
        auto futuresLock = std::lock_guard(_FuturesMutex);
        std::for_each(_Futures.begin(), _Futures.end(),
                      [](const std::shared_ptr<Future> &future) { future->TryTerminate(); });
    }

    // Wait for all threads to finish
    {
        auto lock = std::unique_lock(_Mutex);
        _TerminatedCondition.wait(lock, [this] { return _Futures.empty(); });
    }

    auto lock = std::lock_guard(_Mutex);

    // Terminate all workers
    for (auto &worker : _Workers)
    {
        worker->TryTerminate();
    }

    // Terminate all threads
    for (auto &thread : _Threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}
void ThreadPool::Wait()
{
    while (ActiveThreadCount())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
} // namespace Thread
