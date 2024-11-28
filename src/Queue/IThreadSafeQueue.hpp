#ifndef QUEUE__ITHREAD_SAFE_QUEUE_HPP
#define QUEUE__ITHREAD_SAFE_QUEUE_HPP

#include <chrono>
#include <optional>

template <typename T> class IThreadSafeQueue
{
  public:
    virtual void Enqueue(T value) = 0;
    virtual bool TryEnqueue(T value, std::chrono::milliseconds timeout) = 0;
    virtual std::optional<T> Dequeue(std::chrono::milliseconds timeout) = 0;
    virtual bool TryDequeue(T &value) = 0;
    virtual bool Empty() const = 0;
};

#endif
