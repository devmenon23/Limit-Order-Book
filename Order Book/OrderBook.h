#pragma once
#include "Order.h"
#include <list>
#include <map>
#include <unordered_map>

#include "OrderBook.h"
#include "OrderBook.h"

/**
 * @brief Represents a price level in the order book which contains all 
 * orders at a specific price according to time priority
 */
struct PriceLevel {
    std::uint16_t price;
    std::list<OrderPointer>orders; // FIFO by time priority at a certain price level but implemented as doubly-linked list because ability to iterate is required
    std::unordered_map<std::uint64_t, std::list<OrderPointer>::iterator> orderIters;
};

using PriceLevelPointer = std::shared_ptr<PriceLevel>;

/**
 * @brief Represents a limit order book for matching buy and sell orders
 */
class OrderBook {
private:
    std::unordered_map<std::uint32_t, PriceLevelPointer> bids;
    std::unordered_map<std::uint32_t, PriceLevelPointer> asks;
    std::unordered_map<std::uint64_t, OrderPointer> orders; // to get orders by id

    PriceLevelPointer highestBidLevel;
    PriceLevelPointer lowestAskLevel;

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
     * @brief Matches buy and sell orders in the order book according to
     * price-time priority
     *
     * @return A shared pointer to the lowest ask in the order book
     */
    void matchOrders();
    /**
     * @brief Retrieves the order with the given id in the order book
     *
     * @param idNumber The id number of the order to be retrieved
     *
     * @return A shared pointer to the modified order
     */
    OrderPointer getOrderByID(std::uint64_t idNumber);
    /**
     * @brief Retrieves the total number of orders in the order book
     *
     * @return A shared pointer to the modified order
     */
    std::size_t getNumberOfOrders() const;
    /**
     * @brief Retrieves if order is in the order book or not
     *
     * @return If the order book contains order
     */
    bool contains(const OrderPointer& order) const;
};


