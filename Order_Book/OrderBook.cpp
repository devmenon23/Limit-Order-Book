#include "OrderBook.h"

void OrderBook::addOrder(const OrderPointer& order) {
    switch (order->getSide()) {
        case Side::BUY: {
            // Check if there is a list of the appropriate price level otherwise add a list
            if (!bids.contains(order->getPrice())) {
                PriceLevelPointer newPriceLevel = std::make_shared<PriceLevel>();
                newPriceLevel->price = order->getPrice();
                bids.insert(std::make_pair(order->getPrice(), newPriceLevel));
            }
            const PriceLevelPointer priceLevel = bids[order->getPrice()];
            const auto it = priceLevel->orders.insert(priceLevel->orders.end(), order);
            priceLevel->orderIters[order->getIDNumber()] = it;
            break;
        }
        case Side::SELL: {
            // Check if there is a list of the appropriate price level otherwise add a list
            if (!asks.contains(order->getPrice())) {
                PriceLevelPointer newPriceLevel = std::make_shared<PriceLevel>();
                newPriceLevel->price = order->getPrice();
                asks.insert(std::make_pair(order->getPrice(), newPriceLevel));
            }
            const PriceLevelPointer priceLevel = asks[order->getPrice()];
            const auto it = priceLevel->orders.insert(priceLevel->orders.end(), order);
            priceLevel->orderIters[order->getIDNumber()] = it;
            break;
        }
    }
    orders[order->getIDNumber()] = order;
}

OrderPointer OrderBook::modifyOrder(IdNumber idNumber, Price newPrice, Quantity newQty) {
    if (!orders.contains(idNumber)) {
        throw std::logic_error("Order('" + std::to_string(idNumber) + "') does not exist");
    }
    const OrderPointer tempOrder = orders[idNumber];
    if (tempOrder->getStatus() != Status::PENDING) {
        throw std::logic_error("Order('" + std::to_string(idNumber) + "') is not pending so it cannot be modified");
    }

    const OrderPointer order = std::make_shared<Order>(tempOrder->getIDNumber(), tempOrder->getSide(), newPrice, newQty);
    cancelOrder(idNumber);
    addOrder(order);
    return order;
}

void OrderBook::cancelOrder(const IdNumber idNumber) {
    if (!orders.contains(idNumber)) {
        throw std::logic_error("Order ('" + std::to_string(idNumber) + "') does not exist or was completely filled");
    }
    const OrderPointer order = orders[idNumber];
    if (order->getStatus() == Status::PARTIALLY_FILLED) {
        throw std::logic_error("Order ('" + std::to_string(idNumber) + "') cannot be canceled as it is already partially filled");
    }

    const Side side = order->getSide();

    if (side == Side::BUY) {
        const PriceLevelPointer priceLevel = bids[order->getPrice()];
        priceLevel->orders.erase(priceLevel->orderIters[idNumber]);
        priceLevel->orderIters.erase(idNumber);
        orders.erase(idNumber);

        if (priceLevel->orders.empty()) {
            bids.erase(priceLevel->price);
        }
    }

    else if (side == Side::SELL) {
        const PriceLevelPointer priceLevel = asks[order->getPrice()];
        priceLevel->orders.erase(priceLevel->orderIters[idNumber]);
        priceLevel->orderIters.erase(idNumber);
        orders.erase(idNumber);

        if (priceLevel->orders.empty()) {
            asks.erase(priceLevel->price);
        }
    }
}

void OrderBook::matchOrders() {
    PriceLevelPointer highestBidLevel;
    PriceLevelPointer lowestAskLevel;

    if (!bids.empty() && !asks.empty()) {
        highestBidLevel = bids.begin()->second;
        lowestAskLevel = asks.begin()->second;
    }

    while (highestBidLevel && lowestAskLevel &&
           !highestBidLevel->orders.empty() && !lowestAskLevel->orders.empty() &&
           highestBidLevel->price >= lowestAskLevel->price) {
        const OrderPointer highestBidOrder = highestBidLevel->orders.front();
        const OrderPointer lowestAskOrder = lowestAskLevel->orders.front();

        // Fill orders based on remaining quantity differences
        const Quantity qty = std::min(highestBidOrder->getRemainingQuantity(), lowestAskOrder->getRemainingQuantity());
        highestBidOrder->fill(qty);
        lowestAskOrder->fill(qty);

        // Remove filled order and update pointers as needed
        if (highestBidOrder->getRemainingQuantity() == 0) {
            IdNumber idNumber = highestBidOrder->getIDNumber();
            highestBidLevel->orders.pop_front();
            highestBidLevel->orderIters.erase(idNumber);
            orders.erase(idNumber);

            if (highestBidLevel->orders.empty()) {
                bids.erase(highestBidLevel->price);
                if (!bids.empty()) {
                    highestBidLevel = bids.begin()->second;
                }
                else {
                    highestBidLevel = nullptr;
                }
            }
        }
        if (lowestAskOrder->getRemainingQuantity() == 0) {
            IdNumber idNumber = lowestAskOrder->getIDNumber();
            lowestAskLevel->orders.pop_front();
            lowestAskLevel->orderIters.erase(idNumber);
            orders.erase(idNumber);

            if (lowestAskLevel->orders.empty()) {
                asks.erase(lowestAskLevel->price);
                if (!asks.empty()) {
                    lowestAskLevel = asks.begin()->second;
                }
                else {
                    lowestAskLevel = nullptr;
                }
            }
        }
    }
}

OrderPointer OrderBook::getOrderByID(const IdNumber idNumber) {
    if (!orders.contains(idNumber)) {
        throw std::logic_error("Order ('" + std::to_string(idNumber) + "') does not exist");
    }

    return orders[idNumber];
}

std::size_t OrderBook::getNumberOfOrders() const {
    return orders.size();
}


bool OrderBook::contains(const OrderPointer& order) const {
    return orders.contains(order->getIDNumber());;
}

