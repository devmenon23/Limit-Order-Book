#include "OrderBookTests.h"

void addAndMatchTest() {
    OrderBook book;

    OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 10);
    OrderPointer order2 = std::make_shared<Order>(2, Side::SELL, 9900, 5);

    book.addOrder(order1);
    book.addOrder(order2);

    assert(order1->getRemainingQuantity() == 5 && "order1 should have 5 remaining");
    assert(order2->getRemainingQuantity() == 0 && "order2 should have 0 remaining");

    std::cout << "addAndMatchTest() passed!\n";
};

void timePriorityMatchingTest() {
    OrderBook book;

    OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 5);
    OrderPointer order2 = std::make_shared<Order>(2, Side::BUY, 10000, 10);
    OrderPointer order3 = std::make_shared<Order>(3, Side::SELL, 10000, 12);

    book.addOrder(order1);
    book.addOrder(order2);
    book.addOrder(order3);

    assert(order1->getRemainingQuantity() == 0 && "order1 should have 0 remaining");
    assert(order2->getRemainingQuantity() == 3 && "order2 should have 3 remaining");
    assert(order3->getRemainingQuantity() == 0 && "order3 should have 0 remaining");

    std::cout << "timePriorityMatchingTest() passed!\n";
};

void cancelTest() {
    OrderBook book;

    OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 10);
    
    book.addOrder(order1);
    book.cancelOrder(1);
    
    assert(order1->getRemainingQuantity() == 0 && "order1 should have 0 remaining");

    std::cout << "cancelTest() passed!\n";
};

void benchmarkFiveMillionOrders(){
    using namespace std::chrono;
    OrderBook book;

    TransitionMatrix matrix = {
        {0.80, 0.10, 0.10}, // Neutral
        {0.10, 0.85, 0.05}, // Buy Pressure
        {0.10, 0.05, 0.85} // Sell Pressure
    };
    std::random_device rd;
    std::mt19937 rng(rd());

    OrderGenerator generator(matrix, rng);

    auto start = high_resolution_clock::now();

    for (int i = 0; i < 5000000; i++) {
        generator.nextState();
        Side orderSide = generator.pickOrderSide();
        std::uint32_t orderPrice = generator.generateOrderPrice(10000, orderSide, 1.0, 2.5); // reference price is $100.00
        std::uint32_t orderSize = generator.generateOrderSize(10.0, 1.7);

        OrderPointer order = std::make_shared<Order>(i, orderSide, orderPrice, orderSize);
        book.addOrder(order);
    }

    auto end = high_resolution_clock::now();
    double elapsed = duration<double>(end - start).count();

    std::cout << "Processed 5,000,000 orders in " << elapsed << " seconds.\n";
    std::cout << "Throughput: " << (5000000.0 / elapsed) << " orders/sec\n";
};

int main() {
    addAndMatchTest();
    timePriorityMatchingTest();
    cancelTest();

    std::cout << "All tests passed!\n";

    benchmarkFiveMillionOrders();

    return 0;
};
