#include <Thread/Runnable.hpp>

namespace Thread
{
Runnable::Runnable() : _IsRequestedToTerminate(false)
{
}

void Runnable::Run()
{
    PreTask();
    while (IsContinue())
    {
        // In the task, it may call Sleep() to sleep for a while.
        Task();
    }
    PostTask();
}

void Runnable::TryTerminate()
{
    // Change flag to terminate
    _IsRequestedToTerminate = true;

    // Interrupt the sleep if the thread is sleeping
    _SleepCondition.notify_all();
}

void Runnable::Join()
{
    if (_Thread.joinable())
    {
        _Thread.join();
    }
}

void Runnable::Sleep(std::chrono::milliseconds duration)
{
    // Sleep for the given duration, but should be interrupted if requested to terminate
    std::unique_lock<std::mutex> lock(_Mutex);
    _SleepCondition.wait_for(lock, duration, [this] { return ShouldBeTerminated(); });
}

void Runnable::PreTask()
{
    // Do nothing by default
}

void Runnable::PostTask()
{
    // Do nothing by default
}

bool Runnable::IsContinue()
{
    return !ShouldBeTerminated();
}

bool Runnable::ShouldBeTerminated()
{
    return _IsRequestedToTerminate.load();
}
}; // namespace Thread
