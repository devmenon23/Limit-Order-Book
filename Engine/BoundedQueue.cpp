#include "BoundedQueue.h"

BoundedQueue::BoundedQueue(size_t capacity):
    capacity(capacity),
    stopped(false)
{}

void BoundedQueue::push(const Command& command) {
    std::unique_lock<std::mutex> lock(m);
    notFull.wait(lock, [&]{ return stopped || q.size() < capacity; });
    if (stopped) return;
    q.push_back(command);
    notEmpty.notify_one();
}

bool BoundedQueue::try_push(const Command& command) {
    std::lock_guard<std::mutex> lock(m);
    if (stopped || q.size() < capacity) return false;
    q.push_back(command);
    notEmpty.notify_one();
    return true;
}

bool BoundedQueue::pop(Command& out) {
    std::unique_lock<std::mutex> lock(m);
    notEmpty.wait(lock, [&]{ return stopped || !q.empty(); });
    if (q.empty()) return false;
    out = std::move(q.front());
    q.pop_front();
    notFull.notify_one();
    return true;
}

void BoundedQueue::stop() {
    std::lock_guard<std::mutex> lock(m);
    stopped = true;
    notFull.notify_all();
    notEmpty.notify_all();
}

