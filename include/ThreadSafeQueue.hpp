#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace TrafficPlayer
{
template <typename T> class ThreadSafeQueue
{
  public:
    void enqueue(T value)
    {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(std::move(value));
        cv.notify_one();
    }

    std::optional<T> dequeue(std::chrono::milliseconds timeout)
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

    bool try_dequeue(T &value)
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

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return q.empty();
    }

  private:
    std::queue<T> q;
    mutable std::mutex mtx;
    std::condition_variable cv;
};
} // namespace TrafficPlayer

#endif
