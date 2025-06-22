#include "OrderBookTests.h"

void completeFillTest() {
    OrderBook book;

    const OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 5);
    const OrderPointer order2 = std::make_shared<Order>(2, Side::SELL, 9900, 5);

    book.addOrder(order1);
    book.addOrder(order2);

    assert(order1->getRemainingQuantity() == 0 && "order1 should have 0 remaining");
    assert(order2->getRemainingQuantity() == 0 && "order2 should have 0 remaining");

    std::cout << "completeFillTest() passed!\n";
}

void partialFillTest() {
    OrderBook book;

    const OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 10);
    const OrderPointer order2 = std::make_shared<Order>(2, Side::SELL, 9900, 5);

    book.addOrder(order1);
    book.addOrder(order2);

    assert(order1->getRemainingQuantity() == 5 && "order1 should have 5 remaining");
    assert(order2->getRemainingQuantity() == 0 && "order2 should have 0 remaining");

    std::cout << "partialFillTest() passed!\n";
}

void multiplePriceLevelTest() {
    OrderBook book;
    OrderPointer highestPriorityBuyOrder;

    for (int i = 0; i < 100; i++) {
        OrderPointer order = std::make_shared<Order>(i, Side::BUY, std::floor(i/10), 10); // 10 order per price level
        book.addOrder(order);
        // Get first order added to highest price level
        if (order->getIDNumber() == 90) {
            highestPriorityBuyOrder = order;
        }
    }

    const OrderPointer sellOrder = std::make_shared<Order>(100, Side::SELL, 9, 10);
    book.addOrder(sellOrder);

    assert(highestPriorityBuyOrder->getRemainingQuantity() == 0 && "highestBuyOrder should have 0 remaining");
    assert(sellOrder->getRemainingQuantity() == 0 && "sellOrder should have 0 remaining");

    std::cout << "multiplePriceLevelTest() passed!\n";
}

void timePriorityMatchingTest() {
    OrderBook book;

    const OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 5);
    const OrderPointer order2 = std::make_shared<Order>(2, Side::BUY, 10000, 10);
    const OrderPointer order3 = std::make_shared<Order>(3, Side::SELL, 10000, 12);

    book.addOrder(order1);
    book.addOrder(order2);
    book.addOrder(order3);

    assert(order1->getRemainingQuantity() == 0 && "order1 should have 0 remaining");
    assert(order2->getRemainingQuantity() == 3 && "order2 should have 3 remaining");
    assert(order3->getRemainingQuantity() == 0 && "order3 should have 0 remaining");

    std::cout << "timePriorityMatchingTest() passed!\n";
}

void modifyValidOrderTest() {
    OrderBook book;

    const OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 5);
    const OrderPointer order2 = std::make_shared<Order>(2, Side::BUY, 5000, 5);
    const OrderPointer order3 = std::make_shared<Order>(3, Side::SELL, 5000, 5);

    book.addOrder(order1);
    book.addOrder(order2);
    OrderPointer modifiedOrder1 = book.modifyOrder(1, 5000, 10);
    book.addOrder(order3);

    assert(modifiedOrder1->getPrice() == 5000 && "order1 should have a price of 5000");
    assert(modifiedOrder1->getRemainingQuantity() == 10 && "order1 should have 10 remaining");
    assert(order2->getRemainingQuantity() == 0 && "order2 should have 0 remaining");

    std::cout << "modifyValidOrderTest() passed!\n";
}

void cancelValidOrderTest() {
    OrderBook book;

    const OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 10);
    
    book.addOrder(order1);
    book.cancelOrder(1);
    
    assert(order1->getRemainingQuantity() == 0 && "order1 should have 0 remaining");

    std::cout << "cancelValidOrderTest() passed!\n";
}

void cancelFilledOrderTest() {
    OrderBook book;

    const OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 10);
    const OrderPointer order2 = std::make_shared<Order>(2, Side::SELL, 10000, 10);

    book.addOrder(order1);
    book.addOrder(order2);
    bool exceptionCaught = false;

    try {
        book.cancelOrder(1);
    } catch (const std::logic_error& e) {
        exceptionCaught = true;
    }

    assert(exceptionCaught && "Expected std::logic_error not caught when trying to cancel a filled order");

    std::cout << "cancelFilledOrderTest() passed!\n";
}

void cancelNonExistentOrderTest() {
    OrderBook book;

    bool exceptionCaught = false;

    try {
        book.cancelOrder(1);
    } catch (const std::logic_error& e) {
        exceptionCaught = true;
    }

    assert(exceptionCaught && "Expected std::logic_error not caught when trying to cancel a non-existent order");

    std::cout << "cancelNonExistentOrderTest() passed!\n";
}

void benchmarkFiveMillionOrders() {
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

    const auto start = high_resolution_clock::now();

    for (int i = 0; i < 5000000; i++) {
        generator.nextState();
        Side orderSide = generator.pickOrderSide();
        std::uint32_t orderPrice = generator.generateOrderPrice(10000, orderSide, 1.0, 2.5); // reference price is $100.00
        std::uint32_t orderSize = generator.generateOrderSize(10.0, 1.7);

        OrderPointer order = std::make_shared<Order>(i, orderSide, orderPrice, orderSize);
        book.addOrder(order);
    }

    const auto end = high_resolution_clock::now();
    const double elapsed = duration<double>(end - start).count();

    std::cout << "Processed 5,000,000 orders in " << elapsed << " seconds.\n";
    std::cout << "Throughput: " << (5000000.0 / elapsed) << " orders/sec\n";
}

int main() {
    completeFillTest();
    partialFillTest();
    multiplePriceLevelTest();
    timePriorityMatchingTest();
    modifyValidOrderTest();
    cancelValidOrderTest();
    cancelFilledOrderTest();
    cancelNonExistentOrderTest();

    std::cout << "All tests passed!\n";

    benchmarkFiveMillionOrders();

    return 0;
}
