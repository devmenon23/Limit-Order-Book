#include "Engine.h"

Engine::Engine(size_t queueCapacity):
    queue(queueCapacity),
    running(false),
    nextSeq(1),
    lastAppliedSeq(0),
    pending(0)
{}

Engine::~Engine() { stop(); }

void Engine::apply(const Command& cmd) {
    switch (cmd.type) {
        case CommandType::ADD: {
            const OrderPointer order = std::make_shared<Order>(cmd.idNumber, cmd.side, cmd.price, cmd.qty);
            book.addOrder(order);
            break;
        }
        case CommandType::MODIFY: {
            book.modifyOrder(cmd.idNumber, cmd.price, cmd.qty);
            break;
        }
        case CommandType::CANCEL: {
            book.cancelOrder(cmd.idNumber);
            break;
        }
    }
}

void Engine::matchIfNeeded() {
    book.matchOrders();
}

void Engine::run() {
    while (running.load(std::memory_order_relaxed)) {
        Command cmd;
        if (!queue.pop(cmd)) {
            break;
        }
        try {
            apply(cmd);
            // Match if pending is more than the BATCH threshold or if the queue is empty
            if (++pending >= BATCH || queue.isEmpty()) {
                matchIfNeeded();
                pending = 0;
            }
            lastAppliedSeq.store(cmd.seqNum, std::memory_order_release);
        } catch (const std::exception& e) {
            if (onError) { onError(cmd, e); }
            lastAppliedSeq.store(cmd.seqNum, std::memory_order_release);
        }

    }
}

void Engine::start() {
    bool expected = false;
    if (!running.compare_exchange_strong(expected, true)) { return; }
    engineThread = std::thread([this]{ this->run(); });
}

void Engine::stop() {
    bool expected = true;
    if (!running.compare_exchange_strong(expected, false)) { return; }
    queue.stop();
    if (engineThread.joinable()) { engineThread.join(); }
}

SeqNum Engine::submit(Command cmd) {
    cmd.seqNum = nextSeq.fetch_add(1, std::memory_order_relaxed);
    queue.push(cmd);
    return cmd.seqNum;
}

const OrderBook& Engine::getBook() const { return book; }

SeqNum Engine::lastProcessed() const { return lastAppliedSeq.load(); }

void Engine::set_error_handler(ErrorHandler f) { onError = std::move(f); }

