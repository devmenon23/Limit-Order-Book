#pragma once
#include "Order.h"
#include <vector>
#include <map>
#include <unordered_map>

/**
 * @brief Represents a price level in the order book which contains all 
 * orders at a specific price according to time priority
 */
struct PriceLevel {
    std::uint16_t price;
    std::size_t startIndex = 0;
    std::vector<OrderPointer>orders; // FIFO by time priority at a certain price level but implemented as vector because ability to iterate is required
};

/**
 * @brief Represents a limit order book for matching buy and sell orders
 */
class OrderBook {
private:
    std::map<std::uint32_t, PriceLevel, std::greater<>> bids; // sorted descending
    std::map<std::uint32_t, PriceLevel> asks; // sorted ascending
    std::unordered_map<std::uint64_t, OrderPointer> orders; // to get orders by id

    std::uint32_t cancelCount = 0;
    std::uint32_t fillCount = 0;

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
    void addOrder(const OrderPointer& order);
    /**
     * @brief Modifies an order with the new properties and places it in back of the order book
     *
     * @param idNumber The id number of the order
     * @param newPrice The price of the order
     * @param newQty The quantity of the order
     *
     * @return A shared pointer to the modified order
     */
    OrderPointer modifyOrder(std::uint64_t idNumber, std::uint32_t newPrice, std::uint32_t newQty);
    /**
     * @brief Cancels the order related to the specified id number
     * 
     * @param idNumber The id number of the order to be canceled
     */
    void cancelOrder(std::uint64_t idNumber);
    /**
     * @brief Removes all canceled orders from all 3 data structures
     */
    void removeAllSpecificOrders(Status status);
};


