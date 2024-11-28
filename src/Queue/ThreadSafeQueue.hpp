#ifndef QUEUE__THREAD_SAFE_QUEUE_HPP
#define QUEUE__THREAD_SAFE_QUEUE_HPP

#include <Queue/IThreadSafeQueue.hpp>
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T> class ThreadSafeQueue : public IThreadSafeQueue<T>
{
  public:
    virtual void Enqueue(T value) override
    {
        std::lock_guard<std::mutex> lock(_Mutex);
        _Queue.push(std::move(value));
        _ConditionVariable.notify_one();
    }

    virtual bool TryEnqueue(T value, std::chrono::milliseconds timeout) override
    {
        this->Enqueue(std::move(value));
        return true;
    }

    virtual std::optional<T> Dequeue(std::chrono::milliseconds timeout) override
    {
        std::unique_lock<std::mutex> lock(_Mutex);
        if (_ConditionVariable.wait_for(lock, timeout, [this] { return !_Queue.empty(); }))
        {
            T value = std::move(_Queue.front());
            _Queue.pop();
            return value;
        }
        else
        {
            return std::nullopt;
        }
    }

    virtual bool TryDequeue(T &value) override
    {
        std::lock_guard<std::mutex> lock(_Mutex);
        if (_Queue.empty())
        {
            return false;
        }
        value = std::move(_Queue.front());
        _Queue.pop();
        return true;
    }

    virtual bool Empty() const override
    {
        std::lock_guard<std::mutex> lock(_Mutex);
        return _Queue.empty();
    }

  private:
    std::queue<T> _Queue;
    mutable std::mutex _Mutex;
    std::condition_variable _ConditionVariable;
};

#endif
