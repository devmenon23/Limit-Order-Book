#pragma once
#include "Order_Book/Order.h"
#include "EngineCommand.h"
#include <mutex>
#include <condition_variable>
#include <deque>

class BoundedQueue {
private:
    size_t capacity;
    std::deque<Command> q;
    std::mutex m;
    std::condition_variable notFull, notEmpty;
    bool stopped;

public:
    explicit BoundedQueue(size_t capacity);

    void push(const Command& command);

    bool try_push(const Command& command);

    bool pop(Command& out);

    void stop();
};
