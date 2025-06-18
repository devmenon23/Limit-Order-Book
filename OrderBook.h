#pragma once
#include "Order.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>

struct PriceLevel {
    std::uint32_t price;
    std::size_t startIndex = 0;
    std::vector<OrderPointer>orders; // FIFO by time priotiy at a certain price level but implemented as vector because ability to iterate is required
};

class OrderBook {
private:
    std::map<std::uint32_t, PriceLevel, std::greater<std::uint32_t>> bids; // sorted descending
    std::map<std::uint32_t, PriceLevel, std::less<std::uint32_t>> asks; // sorted ascending
    std::unordered_map<std::uint64_t, OrderPointer> orders; // to get orders by id

    OrderPointer findHighestBid();
    OrderPointer findLowestAsk();
    void matchOrders();
public:
    void addOrder(OrderPointer order);
    void cancelOrder(std::uint64_t idNumber);
};


