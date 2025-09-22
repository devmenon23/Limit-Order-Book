#include "BoundedQueue.h"

BoundedQueue::BoundedQueue(size_t capacity):
    capacity(capacity),
    stopped(false)
{}

void BoundedQueue::push(const Command& command) {
    std::unique_lock<std::mutex> lock(m);
    // Wait for queue to be not full
    notFull.wait(lock, [&]{ return stopped || q.size() < capacity; });
    if (stopped) return;
    q.push_back(command);
    notEmpty.notify_one();
}

bool BoundedQueue::pop(Command& out) {
    std::unique_lock<std::mutex> lock(m);
    // Wait for queue to be not empty
    notEmpty.wait(lock, [&]{ return stopped || !q.empty(); });
    if (q.empty()) { return false; }
    out = std::move(q.front());
    q.pop_front();
    notFull.notify_one();
    return true;
}

bool BoundedQueue::isEmpty() const {
    return q.empty();
}

void BoundedQueue::stop() {
    std::lock_guard<std::mutex> lock(m);
    stopped = true;
    notFull.notify_all();
    notEmpty.notify_all();
}

