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

void Order::fill(std::uint32_t quantity) {
    if (quantity > getRemainingQuantity()) {
        std::ostringstream err;
        err << "Order (" << idNumber << ") cannot be filled for more than its current remaining quantity";
        throw std::invalid_argument(err.str());
    }

        remainingQuantity -= quantity;
        status = Status::FILLED;
};

void Order::cancelOrder() {
    if (status == Status::FILLED){
        std::ostringstream err;
        err << "Order (" << idNumber << ") cannot be canceled as it is already filled";
        throw std::logic_error(err.str());
    }
    remainingQuantity = 0;
    status = Status::CANCELED;
};
