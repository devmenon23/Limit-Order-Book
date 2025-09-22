#pragma once
#include "EngineCommand.h"
#include <mutex>
#include <condition_variable>
#include <deque>

/**
 * @brief Represents a thread-safe bounded MPMC queue for engine commands
 */
class BoundedQueue {
private:
    size_t capacity;
    std::deque<Command> q;
    std::mutex m;
    std::condition_variable notFull, notEmpty;
    bool stopped;

public:
    /**
     * @brief Construct a bounded queue
     * @param capacity Maximum capacity of commands allowed in the queue at once
     */
    explicit BoundedQueue(size_t capacity);

    /**
     * @brief Enqueue a command (blocking if full)
     * @param command Command to insert
     */
    void push(const Command& command);

    /**
     * @brief Dequeue the next command (blocking if empty)
     * @param out Receives the dequeued command
     * @return if a command was successfully popped
     */
    bool pop(Command& out);

    /**
     * @brief Evaluates if the queue is empty
     * @return if the queue is empty
     */
    bool isEmpty() const;

    /**
     * @brief Stop the queue and wake all waiting threads
     */
    void stop();
};
