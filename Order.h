#pragma once
#include <iostream>
#include <stdexcept>
#include <sstream>

enum class Side { 
    BUY, 
    SELL 
};

enum Status {
    PENDING,
    FILLED,
    CANCELED
};

class Order {
private:
    std::uint64_t idNumber;
    Side side;
    std::uint32_t price;
    std::uint32_t initialQuantity;
    std::uint32_t remainingQuantity;
    Status status;
public:
    Order(std::uint64_t id, Side side, std::uint32_t price, std::uint32_t qty);

    // Getter methods
    std::uint64_t getIDNumber() const;
    Side getSide() const;
    std::uint32_t getPrice() const;
    std::uint32_t getInitialQuantity() const;
    std::uint32_t getRemainingQuantity() const;
    std::uint32_t getFilledQuantity() const;

    void fill(std::uint32_t qty);
    void cancelOrder();
};

using OrderPointer = std::shared_ptr<Order>;