#ifndef THREAD__WORKER_HPP
#define THREAD__WORKER_HPP

#include <Queue/IThreadSafeQueue.hpp>
#include <Thread/Runnable.hpp>
#include <spdlog/spdlog.h>

namespace Thread
{
/// @brief Worker class
class Worker : public Runnable
{
  public:
    /// @brief Constructor
    Worker(RunnableQueuePtr queue) : _QueuePtr(queue)
    {
    }

    /// @brief Destructor
    virtual ~Worker()
    {
    }

    void Task()
    {
        while (IsContinue())
        {
            auto runnablePtrOpt = _QueuePtr->Dequeue(std::chrono::milliseconds(1000));

            if (!runnablePtrOpt.has_value())
            {
                continue;
            }

            auto runnablePtr = runnablePtrOpt.value();

            if (!runnablePtr)
            {
                spdlog::warn("Runnable is nullptr.");
                continue;
            }

            runnablePtr->Run();
        }
    }

  private:
    RunnableQueuePtr _QueuePtr;
};
using WorkerPtr = std::shared_ptr<Worker>;
} // namespace Thread

#endif
