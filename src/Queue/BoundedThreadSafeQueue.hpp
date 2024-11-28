#ifndef QUEUE__BOUNDED_THREAD_SAFE_QUEUE_HPP
#define QUEUE__BOUNDED_THREAD_SAFE_QUEUE_HPP

#include <Queue/IThreadSafeQueue.hpp>
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T> class BoundedThreadSafeQueue : public IThreadSafeQueue<T>
{
  public:
    explicit BoundedThreadSafeQueue(size_t max_size) : max_size(max_size)
    {
    }

    virtual void enqueue(T value) override
    {
        std::unique_lock<std::mutex> lock(this->mtx);
        cv_full.wait(lock, [this] { return this->q.size() < max_size; });
        this->q.push(std::move(value));
        this->cv.notify_one();
    }

    virtual bool try_enqueue(T value, std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex> lock(this->mtx);
        if (cv_full.wait_for(lock, timeout, [this] { return this->q.size() < max_size; }))
        {
            this->q.push(std::move(value));
            this->cv.notify_one();
            return true;
        }
        return false;
    }

    virtual std::optional<T> dequeue(std::chrono::milliseconds timeout) override
    {
        std::unique_lock<std::mutex> lock(this->mtx);
        if (this->cv.wait_for(lock, timeout, [this] { return !this->q.empty(); }))
        {
            T value = std::move(this->q.front());
            this->q.pop();
            cv_full.notify_one();
            return value;
        }
        else
        {
            return std::nullopt;
        }
    }

    virtual bool try_dequeue(T &value) override
    {
        std::lock_guard<std::mutex> lock(this->mtx);
        if (this->q.empty())
        {
            return false;
        }
        value = std::move(this->q.front());
        this->q.pop();
        cv_full.notify_one();
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
    std::condition_variable cv_full;
    size_t max_size;
};

#endif
