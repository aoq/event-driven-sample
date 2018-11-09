#include "event_processor.h"


namespace event
{

EventProcessor::EventProcessor() :
    started_(false),
    exitLoop_(false)
{
}

EventProcessor::~EventProcessor()
{
    stop();
}

void EventProcessor::start()
{
    std::lock_guard<std::mutex> lock(processorMutex_);
    if (started_) return;

    processorThread_ = std::thread([this] { loop(); });
}

void EventProcessor::stop()
{
    std::lock_guard<std::mutex> lock(processorMutex_);
    if (!started_) return;

    exitLoop_ = true;
    queueCondition_.notify_one();

    if (processorThread_.joinable())
    {
        processorThread_.join();
    }
}

bool EventProcessor::isStarted()
{
    std::lock_guard<std::mutex> lock(processorMutex_);
    return started_;
}

void EventProcessor::queue(const std::shared_ptr<Event>& e)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    queue_.push(e);
    queueCondition_.notify_one();
}

void EventProcessor::loop()
{
    started_ = true;
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            while (queue_.empty())
            {
                if (exitLoop_)
                {
                    break;
                }
                // Wait for a new event.
                queueCondition_.wait(lock);
            }

            if (exitLoop_)
            {
                break;
            }

            if (queue_.empty())
            {
                continue;
            }

            auto& e = queue_.front();
            e->run();
            queue_.pop();
        }
    }
    started_ = false;
}

}


