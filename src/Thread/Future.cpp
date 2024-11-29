#include <Thread/Future.hpp>

namespace Thread
{
Future::Future(std::shared_ptr<Runnable> runnablePtr) : _RunnablePtr(runnablePtr)
{
}

bool Future::Wait(std::chrono::milliseconds timeout)
{
    std::unique_lock<std::mutex> lock(_Mutex);
    return _ConditionVar.wait_for(lock, timeout, [this] { return !_RunnablePtr || !_RunnablePtr->IsContinue(); });
}

void Future::TryTerminate()
{
    if (_RunnablePtr)
    {
        _RunnablePtr->TryTerminate();
    }
}

std::optional<std::shared_ptr<Runnable>> Future::Get(std::chrono::milliseconds timeout)
{
    // Wait for the thread to finish
    bool wait = Wait(timeout);

    // If the thread is not finished after the timeout, return nullopt
    return wait ? std::make_optional(_RunnablePtr) : std::nullopt;
}

void Future::RegisterCallback(std::function<void()> callback)
{
    std::unique_lock<std::mutex> lock(_Mutex);
    _Callbacks.push_back(callback);
}

void Future::NotifyCallbacks()
{
    std::for_each(_Callbacks.begin(), _Callbacks.end(), [](const std::function<void()> &callback) { callback(); });
}
} // namespace Thread
