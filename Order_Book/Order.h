#pragma once
#include <iostream>
/**
 * @brief Both order sides
 */
enum class Side { 
    BUY, 
    SELL 
};

/**
 * @brief All statuses of an order in the book
 */
enum class Status {
    PENDING,
    PARTIALLY_FILLED,
    COMPLETELY_FILLED
};

using IdNumber = std::uint64_t;
using Price = std::uint32_t;
using Quantity = std::uint32_t;

/**
 * @brief Represents an order in the market
 */
class Order {
private:
    IdNumber idNumber;
    Side side;
    Price price;
    Quantity initialQuantity;
    Quantity remainingQuantity;
    Status status;
public:
    /**
     * @brief Constructs a new Order object with given properties
     * 
     * @param id The numbered id of the order
     * @param side The side of the order
     * @param price The price of the order
     * @param qty The quantity of shares of the order
     */
    Order(IdNumber id, Side side, Price price, Quantity qty);

    /**
     * @brief Retrieves the id number of the order
     * 
     * @return The order's id
     */
    IdNumber getIDNumber() const;
    /**
     * @brief Retrieves the side of the order
     * 
     * @return The order's side
     */
    Side getSide() const;
    /**
     * @brief Retrieves the price of the order
     * 
     * @return The order's price
     */
    Price getPrice() const;
    /**
     * @brief Retrieves the initial quantity of the order
     * 
     * @return The order's initial quantity
     */
    Quantity getInitialQuantity() const;
    /**
     * @brief Retrieves the remaining quantity of the order
     * 
     * @return The order's remaining quantity
     */
    Quantity getRemainingQuantity() const;
    /**
     * @brief Retrieves the filled quantity of the order
     * 
     * @return The order's filled quantity
     */
    Quantity getFilledQuantity() const;
    /**
     * @brief Retrieves the status of the order
     * 
     * @return The status of the order
     */
    Status getStatus() const;

    /**
     * @brief Fills a specified number of shares in the order
     * 
     * @param qty The number of shares to fill in the order
     */
    void fill(std::uint32_t qty);
};

using OrderPointer = std::shared_ptr<Order>;