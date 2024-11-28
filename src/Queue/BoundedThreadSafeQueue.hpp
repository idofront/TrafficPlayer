#ifndef QUEUE__BOUNDED_THREAD_SAFE_QUEUE_HPP
#define QUEUE__BOUNDED_THREAD_SAFE_QUEUE_HPP

#include <Queue/IThreadSafeQueue.hpp>
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T> class BoundedThreadSafeQueue : public IThreadSafeQueue<T>
{
  public:
    explicit BoundedThreadSafeQueue(size_t max_size) : _MaxSize(max_size)
    {
    }

    virtual void Enqueue(T value) override
    {
        std::unique_lock<std::mutex> lock(this->_Mutex);
        _ConditionVariableFullQueue.wait(lock, [this] { return this->_Queue.size() < _MaxSize; });
        this->_Queue.push(std::move(value));
        this->_ConditionVariable.notify_one();
    }

    virtual bool TryEnqueue(T value, std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex> lock(this->_Mutex);
        if (_ConditionVariableFullQueue.wait_for(lock, timeout, [this] { return this->_Queue.size() < _MaxSize; }))
        {
            this->_Queue.push(std::move(value));
            this->_ConditionVariable.notify_one();
            return true;
        }
        return false;
    }

    virtual std::optional<T> Dequeue(std::chrono::milliseconds timeout) override
    {
        std::unique_lock<std::mutex> lock(this->_Mutex);
        if (this->_ConditionVariable.wait_for(lock, timeout, [this] { return !this->_Queue.empty(); }))
        {
            T value = std::move(this->_Queue.front());
            this->_Queue.pop();
            _ConditionVariableFullQueue.notify_one();
            return value;
        }
        else
        {
            return std::nullopt;
        }
    }

    virtual bool TryDequeue(T &value) override
    {
        std::lock_guard<std::mutex> lock(this->_Mutex);
        if (this->_Queue.empty())
        {
            return false;
        }
        value = std::move(this->_Queue.front());
        this->_Queue.pop();
        _ConditionVariableFullQueue.notify_one();
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
    std::condition_variable _ConditionVariableFullQueue;
    size_t _MaxSize;
};

#endif
