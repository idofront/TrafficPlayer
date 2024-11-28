#ifndef QUEUE__THREAD_SAFE_QUEUE_HPP
#define QUEUE__THREAD_SAFE_QUEUE_HPP

#include <Queue/IThreadSafeQueue.hpp>
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T> class ThreadSafeQueue : public IThreadSafeQueue<T>
{
  public:
    virtual void enqueue(T value) override
    {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(std::move(value));
        cv.notify_one();
    }

    virtual bool try_enqueue(T value, std::chrono::milliseconds timeout) override
    {
        this->enqueue(std::move(value));
        return true;
    }

    virtual std::optional<T> dequeue(std::chrono::milliseconds timeout) override
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv.wait_for(lock, timeout, [this] { return !q.empty(); }))
        {
            T value = std::move(q.front());
            q.pop();
            return value;
        }
        else
        {
            return std::nullopt;
        }
    }

    virtual bool try_dequeue(T &value) override
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (q.empty())
        {
            return false;
        }
        value = std::move(q.front());
        q.pop();
        return true;
    }

    virtual bool empty() const override
    {
        std::lock_guard<std::mutex> lock(mtx);
        return q.empty();
    }

  private:
    std::queue<T> q;
    mutable std::mutex mtx;
    std::condition_variable cv;
};

#endif
