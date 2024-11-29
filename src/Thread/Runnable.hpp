#ifndef THREAD__RUNNABLE_HPP
#define THREAD__RUNNABLE_HPP

#include <chrono>
#include <memory>

namespace Thread
{
class Runnable
{
  public:
    virtual void Run() final
    {
        PreTask();
        while (IsContinue())
        {
            Task();
        }
        PostTask();
    }
    virtual void TryTerminate() final
    {
        // Change flag to terminate
        _IsRequestedToTerminate = true;

        // Interrupt the sleep if the thread is sleeping
    }
    virtual void Join()
    {
        // Wait for the thread to finish
    }

  protected:
    virtual void Sleep(std::chrono::milliseconds duration)
    {
        // Sleep for the given duration, but should be interrupted if requested to terminate
    }
    virtual void PreTask()
    {
        // Do nothing by default
    }
    virtual void Task()
    {
        // TODO: Change to pure virtual function
    }
    virtual void PostTask()
    {
        // Do nothing by default
    }

  private:
    bool _IsRequestedToTerminate;

    bool IsContinue()
    {
        return !ShouldBeTerminated();
    }
    bool ShouldBeTerminated()
    {
        return _IsRequestedToTerminate;
    }
};

using RunnableSharedPtr = std::shared_ptr<Runnable>;
} // namespace Thread

#endif
