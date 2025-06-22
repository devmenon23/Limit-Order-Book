#include "OrderBook.h"

OrderPointer OrderBook::findHighestBid() {
    OrderPointer highestBid;

    // Find highest bid that is not filled or canceled (has remaining quantity 0)
    auto it = bids.begin();
    while (it != bids.end()) {
        std::size_t i = it->second.startIndex;
        while (i < it->second.orders.size() && it->second.orders[i]->getRemainingQuantity() == 0) {
            i++;
        }

        if (i < it->second.orders.size()) {
            highestBid = it->second.orders[i];
            it->second.startIndex = i;
            return highestBid;
        }

        it = bids.erase(it); // only reached if price level empty
    }
    highestBid = nullptr; // No valid bid found
    return highestBid;
}

OrderPointer OrderBook::findLowestAsk() {
    OrderPointer lowestAsk;

    // Find lowest ask that is not filled or canceled (has remaining quantity 0)
    auto it = asks.begin();
    while (it != asks.end()) {
        std::size_t i = it->second.startIndex;
        while (i < it->second.orders.size() && it->second.orders[i]->getRemainingQuantity() == 0) {
            i++;
        }

        if (i < it->second.orders.size()) {
            lowestAsk = it->second.orders[i];
            it->second.startIndex = i;
            return lowestAsk;
        }

        it = asks.erase(it); // only reached if price level empty
    }
    lowestAsk = nullptr; // No valid ask found
    return lowestAsk;
}

void OrderBook::matchOrders() {
    OrderPointer highestBid = findHighestBid();
    OrderPointer lowestAsk = findLowestAsk();

    // Fill orders based on remaining quantity differences
    while (highestBid && lowestAsk && highestBid->getPrice() >= lowestAsk->getPrice()) {
        const std::uint64_t qty = std::min(highestBid->getRemainingQuantity(), lowestAsk->getRemainingQuantity());
        highestBid->fill(qty);
        lowestAsk->fill(qty);

        // Redefine highest bid and lowest ask depending on remaining quantities
        if (highestBid->getRemainingQuantity() == 0) {
            fillCount++;
            highestBid = findHighestBid();
        }
        if (lowestAsk->getRemainingQuantity() == 0) {
            fillCount++;
            lowestAsk = findLowestAsk();
        }

        // Delete completely filled orders when 500000 have been completely filled
        if (fillCount > 500000) {
            removeAllSpecificOrders(Status::COMPLETELY_FILLED);
        }
    }
}

void OrderBook::addOrder(const OrderPointer& order) {
    if (order->getSide() == Side::BUY) {
        // Check if there is a list of the appropriate price level otherwise add a list
        if (!bids.contains(order->getPrice())) {
            PriceLevel priceLevel;
            priceLevel.price = order->getPrice();
            bids.insert(std::make_pair(order->getPrice(), priceLevel));
        }
        bids[order->getPrice()].orders.push_back(order);
    }

    if (order->getSide() == Side::SELL) {
        // Check if there is a list of the appropriate price level otherwise add a list
        if (!asks.contains(order->getPrice())) {
            PriceLevel priceLevel;
            priceLevel.price = order->getPrice();
            asks.insert(std::make_pair(order->getPrice(), priceLevel));
        }
        asks[order->getPrice()].orders.push_back(order);
    }

    orders[order->getIDNumber()] = order;
    matchOrders();
}

OrderPointer OrderBook::modifyOrder(std::uint64_t idNumber, std::uint32_t newPrice, std::uint32_t newQty) {
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

void OrderBook::cancelOrder(const std::uint64_t idNumber) {
    if (!orders.contains(idNumber)) {
        throw std::logic_error("Order ('" + std::to_string(idNumber) + "') does not exist");
    }
    const OrderPointer tempOrder = orders[idNumber];
    if (tempOrder->getStatus() == Status::PARTIALLY_FILLED || tempOrder->getStatus() == Status::COMPLETELY_FILLED) {
        throw std::logic_error("Order ('" + std::to_string(idNumber) + "') cannot be canceled as it is already filled");
    }

    const Side side = tempOrder->getSide();
    const std::uint32_t priceLevel = tempOrder->getPrice();

    if (side == Side::BUY) {
        // Cancel order through idNumber
        for (const OrderPointer& order : bids[priceLevel].orders) {
            if (order->getIDNumber() == idNumber) {
                order->cancelOrder();
                cancelCount++;
                break;
            }
        }
    }

    if (side == Side::SELL) {
        // Cancel order through idNumber
        for (const OrderPointer& order : asks[priceLevel].orders) {
            if (order->getIDNumber() == idNumber) {
                order->cancelOrder();
                cancelCount++;
                break;
            }
        }
    }

    // Delete canceled orders when 100000 have been canceled
    if (cancelCount == 100000) {
        removeAllSpecificOrders(Status::CANCELED);
    }
}

void OrderBook::removeAllSpecificOrders(Status status) {
    for (auto& [price, priceLevel] : bids) {
        const size_t startIndex = status == Status::CANCELED ? priceLevel.startIndex : 0;
        auto& priceLevelOrders = priceLevel.orders;
        priceLevelOrders.erase(
            std::remove_if(priceLevelOrders.begin() + static_cast<std::vector<OrderPointer>::difference_type>(startIndex),
            priceLevelOrders.end(),
            [status](const OrderPointer& order) {
                    return order->getRemainingQuantity() == 0 && order->getStatus() == status;
            }),
            priceLevelOrders.end());
    }

    for (auto& [price, priceLevel] : asks) {
        const size_t startIndex = status == Status::CANCELED ? priceLevel.startIndex : 0;
        auto& priceLevelOrders = priceLevel.orders;
        priceLevelOrders.erase(
            std::remove_if(priceLevelOrders.begin() + static_cast<std::vector<OrderPointer>::difference_type>(startIndex),
            priceLevelOrders.end(),
            [status](const OrderPointer& order) {
                    return order->getRemainingQuantity() == 0 && order->getStatus() == status;
            }),
            priceLevelOrders.end());
    }

    std::erase_if(orders,
        [status](const auto& pair) {
            return pair.second->getRemainingQuantity() == 0 && pair.second->getStatus() == status;
        });

    status == Status::CANCELED ? cancelCount = 0 : fillCount = 0;
}

