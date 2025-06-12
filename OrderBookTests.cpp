#include "OrderBook.h"

void addAndMatchTest(){
    OrderBook book;

    OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 10);
    OrderPointer order2 = std::make_shared<Order>(2, Side::SELL, 9900, 5);

    // Add and match orders
    book.addOrder(order1);
    book.addOrder(order2);
    book.matchOrders();

    // Check remaining quantities
    assert(order1->getRemainingQuantity() == 5 && "order1 should have 5 remaining");
    assert(order2->getRemainingQuantity() == 0 && "order2 should have 0 remaining");

    std::cout << "addAndMatchTest() passed!\n";
}

void timePriorityMatchingTest(){
    OrderBook book;

    OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 5);
    OrderPointer order2 = std::make_shared<Order>(2, Side::BUY, 10000, 10);
    OrderPointer order3 = std::make_shared<Order>(3, Side::SELL, 10000, 12);

    // Add and match orders
    book.addOrder(order1);
    book.addOrder(order2);
    book.addOrder(order3);
    book.matchOrders();

    // Check remaining quantities
    assert(order1->getRemainingQuantity() == 0 && "order1 should have 0 remaining");
    assert(order2->getRemainingQuantity() == 3 && "order2 should have 3 remaining");
    assert(order3->getRemainingQuantity() == 0 && "order3 should have 0 remaining");

    std::cout << "timePriorityMatchingTest() passed!\n";
}

void cancelTest() {
    OrderBook book;

    OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 10);
    
    // Add and cancel order
    book.addOrder(order1);
    book.cancelOrder(1);
    
    // Check remaining quantities
    assert(order1->getRemainingQuantity() == 0 && "order1 should have 0 remaining");

    std::cout << "cancelTest() passed!\n";
}

int main() {
    addAndMatchTest();
    timePriorityMatchingTest();
    cancelTest();

    std::cout << "All tests passed!\n";
    return 0;
}
