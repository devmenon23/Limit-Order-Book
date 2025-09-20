#pragma once
#include "EngineCommand.h"
#include "BoundedQueue.h"
#include "Order_Book/OrderBook.h"
#include <atomic>
#include <thread>

struct TopOfBook {
    Price bestBidPrice = 0;
    Price bestBidQty = 0;
    Price bestAskPrice = 0;
    Price bestAskQty = 0;
};

using ErrorHandler = std::function<void(const Command&, const std::exception&)>;

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

    void run();
    void apply(const Command& cmd);
    void matchIfNeeded();

public:
    explicit Engine(size_t queueCapacity = 1 << 16);

    ~Engine();

    void start();

    void stop();

    SeqNum submit(Command cmd);

    const OrderBook& getBook() const;

    SeqNum lastProcessed() const;

    void set_error_handler(ErrorHandler f);
};

