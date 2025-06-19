#pragma once
#include "Order.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>

/**
 * @brief Represents a price level in the order book which contains all 
 * orders at a specific price according to time priority
 */
struct PriceLevel {
    std::uint32_t price;
    std::size_t startIndex = 0;
    std::vector<OrderPointer>orders; // FIFO by time priotiy at a certain price level but implemented as vector because ability to iterate is required
};

/**
 * @brief Represents a limit order book for matching buy and sell orders
 */
class OrderBook {
private:
    std::map<std::uint32_t, PriceLevel, std::greater<std::uint32_t>> bids; // sorted descending
    std::map<std::uint32_t, PriceLevel, std::less<std::uint32_t>> asks; // sorted ascending
    std::unordered_map<std::uint64_t, OrderPointer> orders; // to get orders by id

    /**
     * @brief Finds the highest bid in the order book
     * 
     * @return A shared pointer to the highest bid in the order book
     */
    OrderPointer findHighestBid();
    /**
     * @brief Finds the lowest ask in the order book
     * 
     * @return A shared pointer to the lowest ask in the order book
     */
    OrderPointer findLowestAsk();
    /**
     * @brief Matches buy and sell orders in the order book according to 
     * price-time priority
     * 
     * @return A shared pointer to the lowest ask in the order book
     */
    void matchOrders();
public:
    /**
     * @brief Adds an order to the order book and attempts to match it
     * 
     * @param order The shared pointer to the order to be added
     */
    void addOrder(OrderPointer order);
    /**
     * @brief Cancels the order related to the specified id number
     * 
     * @param idNumber The id number of the order to be canceled
     */
    void cancelOrder(std::uint64_t idNumber);
};


