#include <iostream>
#include <stdexcept>
#include <sstream>

enum Side {
    BID,
    ASK
};

enum Status {
    PENDING,
    FILLED,
    CANCELED
};

class Order {
private:
    int idNumber;
    Side side;
    double price;
    int initialQuantity;
    int remainingQuantity;
    Status status;

public:
    Order(int idNumber, Side side, double price, int quantity): 
        idNumber(idNumber), side(side), price(price), initialQuantity(quantity), remainingQuantity(quantity), status(Status::PENDING) {}

    // Getter methods
    int GetIDNumber() const { return idNumber; }
    Side GetSide() const { return side; }
    double GetPrice() const { return price; }
    int GetInitialQuantity() const { return initialQuantity; }
    int GetRemainingQuantity() const { return remainingQuantity; }
    int GetFilledQuantity() const { return GetInitialQuantity() - GetRemainingQuantity(); }

    // Fills an order by decreasing remaining quantity
    void Fill(int quantity) {
        if (quantity > GetRemainingQuantity()){
            std::ostringstream err;
            err << "Order (" << idNumber << ") cannot be filled for more than its current remaining quantity";
            throw std::invalid_argument(err.str());
        }

        remainingQuantity -= quantity;
        status = Status::FILLED;
    };

    void CancelOrder(){
        remainingQuantity = 0;
        status = Status::CANCELED;
    }
};
