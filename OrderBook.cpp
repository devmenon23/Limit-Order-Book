#include "OrderBook.h"

OrderPointer OrderBook::findHighestBid() {
    OrderPointer highestBid;

    // Find highest bid that is not filled or canceled (has reamaining quantity 0)
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
};

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
};

void OrderBook::addOrder(OrderPointer order) {
    if (order->getSide() == Side::BUY) {
        // Check if there is a list of the appropriate price level otherwise add a list
        if (bids.count(order->getPrice()) == 0) {
            PriceLevel priceLevel;
            priceLevel.price = order->getPrice();
            bids.insert(std::make_pair(order->getPrice(), priceLevel));
        }
        bids[order->getPrice()].orders.push_back(order);
    }

    if (order->getSide() == Side::SELL) {
        // Check if there is a list of the appropriate price level otherwise add a list
        if (asks.count(order->getPrice()) == 0) {
            PriceLevel priceLevel;
            priceLevel.price = order->getPrice();
            asks.insert(std::make_pair(order->getPrice(), priceLevel));
        }
        asks[order->getPrice()].orders.push_back(order);
    }

    orders[order->getIDNumber()] = order;
};

void OrderBook::cancelOrder(std::uint64_t idNumber) {
    Side side = orders[idNumber]->getSide();
    std::uint32_t priceLevel = orders[idNumber]->getPrice();

    if (side == Side::BUY) {
        // Cancel order through idNumber
        for (OrderPointer order : bids[priceLevel].orders) {
            if (order->getIDNumber() == idNumber) {
                order->cancelOrder();
                break;
            }
        }
    }

    if (side == Side::SELL) {
        // Cancel order through idNumber
        for (OrderPointer order : asks[priceLevel].orders) {
            if (order->getIDNumber() == idNumber) {
                order->cancelOrder();
            }
        }
    }
};

void OrderBook::matchOrders() {
    OrderPointer highestBid = findHighestBid();
    OrderPointer lowestAsk = findLowestAsk();

    // Fill orders based on remaining quantity differences
    while (highestBid && lowestAsk && highestBid->getPrice() >= lowestAsk->getPrice()) {
        std::uint64_t qty = std::min(highestBid->getRemainingQuantity(), lowestAsk->getRemainingQuantity());
        highestBid->fill(qty);
        lowestAsk->fill(qty);

        // Redefine highest bid and lowest ask depending on remaining quantities
        if (highestBid->getRemainingQuantity() == 0) {
            highestBid = findHighestBid();
        }
        if (lowestAsk->getRemainingQuantity() == 0) {
            lowestAsk = findLowestAsk();
        }
    }
};