#include <Producer.hpp>

Producer::Producer(std::shared_ptr<TrafficPlayer::ThreadSafeQueue<TrafficRecord>> queue) : queue(queue)
{
}
