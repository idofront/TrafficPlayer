#ifndef BOUNDED_THREAD_SAFE_QUEUE_HPP
#define BOUNDED_THREAD_SAFE_QUEUE_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

template <typename T> class BoundedThreadSafeQueue
{
  public:
    explicit BoundedThreadSafeQueue(size_t max_size) : max_size(max_size)
    {
    }

    void enqueue(T value)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv_full.wait(lock, [this] { return q.size() < max_size; });
        q.push(std::move(value));
        cv.notify_one();
    }

    bool try_enqueue(T value, std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv_full.wait_for(lock, timeout, [this] { return q.size() < max_size; }))
        {
            q.push(std::move(value));
            cv.notify_one();
            return true;
        }
        return false;
    }

    std::optional<T> dequeue(std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv.wait_for(lock, timeout, [this] { return !q.empty(); }))
        {
            T value = std::move(q.front());
            q.pop();
            cv_full.notify_one();
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
        cv_full.notify_one();
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
    std::condition_variable cv_full;
    size_t max_size;
};

#endif
