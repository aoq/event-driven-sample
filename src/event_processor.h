#ifndef EVENT_PROCESSOR_H_
#define EVENT_PROCESSOR_H_

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace event
{

class Event
{
public:
    Event(std::function<void(const void* e)> runner) :
        runner_(runner)
    {
    }

    virtual ~Event()
    {
    }

    void run()
    {
        if (runner_) runner_(this);
    }

private:
    std::function<void(const void* e)> runner_;
};



class ValueEvent : public Event
{
public:
    ValueEvent(int value, std::function<void(const void* e)> runner) :
        value_(value),
        Event(runner)
    {
    }

    virtual ~ValueEvent()
    {
    }

    int getValue() const
    {
        return value_;
    }

private:
    int value_;
};


class EventProcessor
{
public:
    EventProcessor();
    virtual ~EventProcessor();

    /**
     * @brief Starts processing events.
     */
    void start();
    /**
     * @brief Stops processing events.
     */
    void stop();
    /**
     * @brief Determines whether the event processing is started or not.
     */
    bool isStarted();
    /**
     * @brief Adds an event to the event queue.
     */
    void queue(const std::shared_ptr<Event>& e);

private:
    // The worker thread to process events.
    std::thread processorThread_;
    // The lock object for thread operations.
    std::mutex processorMutex_;
    // The flag indicating whether the event processing loop is started or not.
    std::atomic_bool started_;
    // The flag indicating whether the loop should exit or not.
    std::atomic_bool exitLoop_;

    // The event queue.
    std::queue<std::shared_ptr<Event>> queue_;
    // The condition for the event queue.
    std::condition_variable queueCondition_;
    // The loc object for the event queue.
    std::mutex queueMutex_;

    /**
     * @brief Processes queued events in a loop.
     */
    void loop();
};

}
#endif // EVENT_PROCESSOR_H_