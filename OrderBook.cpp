#include <map>
#include <vector>
#include <memory>
#include "Order.cpp"

struct PriceLevel {
    double price;
    int startIndex = 0;
    std::vector<std::shared_ptr<Order>> orders; // FIFO by time priotiy at a certain price level
};

class OrderBook {
private:
    std::map<double, PriceLevel, std::greater<>> bids; // sorted descending
    std::map<double, PriceLevel> asks; // sorted ascending
    std::unordered_map<int, std::shared_ptr<Order>> orders; // to get orders by id

public:
    void AddOrder(std::shared_ptr<Order> order) {
        // Check if side is bid
        if (order->GetSide() == Side::BID){
            // Check if there is a list of the appropriate price level otherwise add a list
            if (bids.count(order->GetPrice()) == 0){
                PriceLevel priceLevel;
                priceLevel.price = order->GetPrice();
                bids.insert(std::make_pair(order->GetPrice(), priceLevel));
            }

            // Add order to appropriate price level list
            bids[order->GetPrice()].orders.push_back(order);
        }

        // Check if side is ask
        if (order->GetSide() == Side::ASK){
            // Check if there is a list of the appropriate price level otherwise add a list
            if (asks.count(order->GetPrice()) == 0){
                PriceLevel priceLevel;
                priceLevel.price = order->GetPrice();
                asks.insert(std::make_pair(order->GetPrice(), priceLevel));
            }

            // Add order to appropriate price level list
            asks[order->GetPrice()].orders.push_back(order);
        }

        orders[order->GetIDNumber()] = order;
    }

    void CancelOrder(int idNumber) {
        Side side = orders[idNumber]->GetSide(); // get order side
        double priceLevel = orders[idNumber]->GetPrice(); // get price level of order to be canceled

        // Check if side is bid
        if (side == Side::BID){
            // Iterate through every order in given price level
            for (std::shared_ptr<Order> order : bids[priceLevel].orders){
                // Cancel order if idnumbers match
                if (order->GetIDNumber() == idNumber){
                    order->CancelOrder();
                }
            }
        }

        // Check if side is ask
        if (side == Side::ASK){
            // Iterate through every order in given price level
            for (std::shared_ptr<Order> order : asks[priceLevel].orders){
                // Cancel order if idnumbers match
                if (order->GetIDNumber() == idNumber){
                    order->CancelOrder();
                }
            }
        }
    }

    void FindHighestBid(std::shared_ptr<Order>& highestBid){
        // Find highest bid that is not filled or canceled (has reamaining quantity 0)
        auto it = bids.begin();
        while (it != bids.end()){
            int i = it->second.startIndex; // start at specific index
            // Find next valid bid order
            while (i < it->second.orders.size() && it->second.orders[i]->GetRemainingQuantity() == 0){
                i++;
            }
            // If highestBid found assign it
            if (i < it->second.orders.size()){
                highestBid = it->second.orders[i];
                it->second.startIndex = i;
                return;
            }
            // Remove price level
            it = bids.erase(it);
        }
        highestBid = nullptr; // No valid bid found
    }

    void FindLowestAsk(std::shared_ptr<Order>& lowestAsk){
        // Find lowest ask that is not filled or canceled (has remaining quantity 0)
        auto it = asks.begin();
        while (it != asks.end()){
            int i = it->second.startIndex; // start at specific index
            // Find next valid ask order
            while (i < it->second.orders.size() && it->second.orders[i]->GetRemainingQuantity() == 0){
                i++;
            }
            // If lowestAsk found assign it
            if (i < it->second.orders.size()){
                lowestAsk = it->second.orders[i];
                it->second.startIndex = i;
                return;
            }
            // Remove price level
            it = asks.erase(it);
        }
        lowestAsk = nullptr; // No valid ask found
    }

    void MatchOrders(){
        // Find highest bid and lowest ask
        std::shared_ptr<Order> highestBid;
        std::shared_ptr<Order> lowestAsk;
        FindHighestBid(highestBid);
        FindLowestAsk(lowestAsk);

        // Fill orders based on remaining quantity differences
        while (highestBid && lowestAsk && highestBid->GetPrice() >= lowestAsk->GetPrice()) {
            // Only fill the minimum quantity between the highest bid and lowest ask
            int qty = std::min(highestBid->GetRemainingQuantity(), lowestAsk->GetRemainingQuantity());
            highestBid->Fill(qty);
            lowestAsk->Fill(qty);

            // Redefine highest bid and lowest ask depending on remaining quantities
            if (highestBid->GetRemainingQuantity() == 0){
                FindHighestBid(highestBid);
            }
            if (lowestAsk->GetRemainingQuantity() == 0){
                FindLowestAsk(lowestAsk);
            }
        }
    }
};
