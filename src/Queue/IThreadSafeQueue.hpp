#ifndef QUEUE__ITHREAD_SAFE_QUEUE_HPP
#define QUEUE__ITHREAD_SAFE_QUEUE_HPP

#include <chrono>
#include <optional>

template <typename T> class IThreadSafeQueue
{
  public:
    virtual void enqueue(T value) = 0;
    virtual bool try_enqueue(T value, std::chrono::milliseconds timeout) = 0;
    virtual std::optional<T> dequeue(std::chrono::milliseconds timeout) = 0;
    virtual bool try_dequeue(T &value) = 0;
    virtual bool empty() const = 0;
};

#endif
