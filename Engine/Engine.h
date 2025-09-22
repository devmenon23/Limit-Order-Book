#pragma once
#include "EngineCommand.h"
#include "BoundedQueue.h"
#include "Order_Book/OrderBook.h"
#include <atomic>
#include <thread>

using ErrorHandler = std::function<void(const Command&, const std::exception&)>;

/**
 * @brief Matching engine that processes commands in a worker thread
 */
class Engine {
private:
    std::atomic<bool> running;
    std::thread engineThread;

    std::atomic<SeqNum> nextSeq;
    std::atomic<SeqNum> lastAppliedSeq;
    BoundedQueue queue;
    OrderBook book;
    static constexpr int BATCH = 30000;
    int pending;

    ErrorHandler onError;

    /**
     * @brief Apply a single command to the order book
     * @param cmd Command to apply
     */
    void apply(const Command& cmd);

    /**
     * @brief Run the matcher if the batch threshold has been reached.
     */
    void matchIfNeeded();


    /**
     * @brief Runs the engine loop that consumes commands from the queue
     */
    void run();

public:
    /**
     * @brief Construct the engine
     * @param queueCapacity Capacity of the internal command queue
     */
    explicit Engine(size_t queueCapacity = 1 << 16);

    /**
     * @brief Destroy the engine, stopping the worker thread if running
     */
    ~Engine();

    /**
     * @brief Start the worker thread
     */
    void start();

    /**
     * @brief Stop the worker thread and flush the queue
     */
    void stop();

    /**
     * @brief Submit a command to the engine
     * @param cmd Command to execute
     * @return Sequence number assigned to the command
     */
    SeqNum submit(Command cmd);

    /**
     * @brief Get a const reference to the internal order book
     * @return Reference to the order book
     */
    const OrderBook& getBook() const;

    /**
     * @brief Get the sequence number of the last processed command
     * @return Last applied sequence number
     */
    SeqNum lastProcessed() const;

    /**
     * @brief Set a custom error handler for failed commands
     * @param f Function taking the command and the thrown exception
     */
    void set_error_handler(ErrorHandler f);
};

