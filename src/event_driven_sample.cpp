#include <conio.h>

#include <iostream>
#include <memory>

#include "event_processor.h"

using namespace event;

char waitForCommandInput()
{
    char inputKey;
    inputKey = _getch();
    return inputKey;
}

int main()
{
    auto eventProcessor = std::make_unique<EventProcessor>();
    eventProcessor->start();

    bool exitLoop = false;
    int value = 0;
    while (!exitLoop)
    {
        auto key = waitForCommandInput();
        switch (key)
        {
        case 'a':
        {
            auto e = std::make_shared<Event>([](const void* e)
            {
                auto a = static_cast<const Event*>(e);
                std::cout << "EVENT" << std::endl;
            });
            eventProcessor->queue(std::move(e));
            break;
        }
        case 'v':
        {
            auto e = std::make_shared<ValueEvent>(value++, [](const void* e)
            {
                auto a = static_cast<const ValueEvent*>(e);
                std::cout << "VALUE EVENT:" << a->getValue() << std::endl;
            });
            eventProcessor->queue(std::move(e));
            break;
        }
        case 'e':
            exitLoop = true;
            break;
        }
    }

    eventProcessor->stop();
    return 0;
}

