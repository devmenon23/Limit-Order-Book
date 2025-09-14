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

class Engine {
private:
    std::atomic<bool> running;
    std::thread engineThread;

    std::atomic<SeqNum> nextSeq;
    BoundedQueue queue;
    OrderBook book;

    void run();
    void apply(const Command& cmd);
    void matchIfNeeded();

public:
    explicit Engine(size_t queueCapacity = 1 << 16);

    ~Engine();

    void start();

    void stop();

    SeqNum submit(Command cmd);
};

