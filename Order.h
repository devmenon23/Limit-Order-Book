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
    FILLED,
    CANCELED
};

/**
 * @brief Represents an order in the market
 */
class Order {
private:
    std::uint64_t idNumber;
    Side side;
    std::uint32_t price;
    std::uint32_t initialQuantity;
    std::uint32_t remainingQuantity;
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
    Order(std::uint64_t id, Side side, std::uint32_t price, std::uint32_t qty);

    /**
     * @brief Retrieves the id number of the order
     * 
     * @return The order's id
     */
    std::uint64_t getIDNumber() const;
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
    std::uint32_t getPrice() const;
    /**
     * @brief Retrieves the initial quantity of the order
     * 
     * @return The order's initial quantity
     */
    std::uint32_t getInitialQuantity() const;
    /**
     * @brief Retrieves the remaining quantity of the order
     * 
     * @return The order's remaining quantity
     */
    std::uint32_t getRemainingQuantity() const;
    /**
     * @brief Retrieves the filled quantity of the order
     * 
     * @return The order's filled quantity
     */
    std::uint32_t getFilledQuantity() const;
    /**
     * @brief Retrieves the status of the order
     * 
     * @return The status of the order
     */
    Status getStatus() const;

    /**
     * @brief Fills a specified number of shares in the order
     * 
     * @param qty the number of shares to fill in the order
     */
    void fill(std::uint32_t qty);
    /**
     * @brief Cancels the order by making the remaining quantity 0
     */
    void cancelOrder();
};

using OrderPointer = std::shared_ptr<Order>;