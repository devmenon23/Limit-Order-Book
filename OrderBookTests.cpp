#include <iostream>
#include <cassert>
#include "OrderBook.cpp"

void addAndMatchTest(){
    OrderBook book;

    std::shared_ptr<Order> order1 = std::make_shared<Order>(1, Side::BID, 100.0, 10);
    std::shared_ptr<Order> order2 = std::make_shared<Order>(2, Side::ASK, 99.0, 5);

    // Add and match orders
    book.AddOrder(order1);
    book.AddOrder(order2);
    book.MatchOrders();

    // Check remaining quantities
    assert(order1->GetRemainingQuantity() == 5 && "order1 should have 5 remaining");
    assert(order2->GetRemainingQuantity() == 0 && "order2 should have 0 remaining");

    std::cout << "addAndMatchTest() passed!\n";
}

void timePriorityMatchingTest(){
    OrderBook book;

    std::shared_ptr<Order> order1 = std::make_shared<Order>(1, Side::BID, 100.0, 5);
    std::shared_ptr<Order> order2 = std::make_shared<Order>(2, Side::BID, 100.0, 10);
    std::shared_ptr<Order> order3 = std::make_shared<Order>(3, Side::ASK, 100.0, 12);

    // Add and match orders
    book.AddOrder(order1);
    book.AddOrder(order2);
    book.AddOrder(order3);
    book.MatchOrders();

    // Check remaining quantities
    assert(order1->GetRemainingQuantity() == 0 && "order1 should have 0 remaining");
    assert(order2->GetRemainingQuantity() == 3 && "order2 should have 3 remaining");
    assert(order3->GetRemainingQuantity() == 0 && "order3 should have 0 remaining");

    std::cout << "timePriorityMatchingTest() passed!\n";
}

void cancelTest() {
    OrderBook book;

    std::shared_ptr<Order> order1 = std::make_shared<Order>(1, Side::BID, 100.0, 10);
    
    // Add and cancel order
    book.AddOrder(order1);
    book.CancelOrder(1);

    // Check remaining quantities
    assert(order1->GetRemainingQuantity() == 0 && "order1 should have 0 remaining");

    std::cout << "cancelTest() passed!\n";
}

int main() {
    addAndMatchTest();
    timePriorityMatchingTest();
    cancelTest();

    std::cout << "All tests passed!\n";
    return 0;
}
