#include "Order.h"

Order::Order(std::uint64_t idNumber, Side side, std::uint32_t price, std::uint32_t quantity): 
    idNumber(idNumber), 
    side(side), 
    price(price), 
    initialQuantity(quantity), 
    remainingQuantity(quantity), 
    status(Status::PENDING) 
{}

// Getter methods
std::uint64_t Order::getIDNumber() const { return idNumber; }
Side Order::getSide() const { return side; }
std::uint32_t Order::getPrice() const { return price; }
std::uint32_t Order::getInitialQuantity() const { return initialQuantity; }
std::uint32_t Order::getRemainingQuantity() const { return remainingQuantity; }
std::uint32_t Order::getFilledQuantity() const { return getInitialQuantity() - getRemainingQuantity(); }
Status Order::getStatus() const { return status; }

void Order::fill(const std::uint32_t qty) {
    if (qty > getRemainingQuantity()) {
        throw std::invalid_argument("Order ('" + std::to_string(idNumber) + "') cannot be filled for more than its current remaining quantity");
    }

        remainingQuantity -= qty;
        if (remainingQuantity == 0) {
            status = Status::COMPLETELY_FILLED;
        }
        else if (remainingQuantity < getInitialQuantity()) {
            status = Status::PARTIALLY_FILLED;
        }
}

void Order::cancelOrder() {
    remainingQuantity = 0;
    status = Status::CANCELED;
}
