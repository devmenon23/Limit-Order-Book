#include "OrderBookTests.h"

void completeFillTest() {
    OrderBook book;

    const OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 5);
    const OrderPointer order2 = std::make_shared<Order>(2, Side::SELL, 9900, 5);

    book.addOrder(order1);
    book.addOrder(order2);
    book.matchOrders();

    assert(!book.contains(order1) && "order1 should be removed from the order book");
    assert(!book.contains(order2) && "order2 should be removed from the order book");

    std::cout << "completeFillTest() passed!\n";
}

void partialFillTest() {
    OrderBook book;

    const OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 10);
    const OrderPointer order2 = std::make_shared<Order>(2, Side::SELL, 9900, 5);

    book.addOrder(order1);
    book.addOrder(order2);
    book.matchOrders();

    assert(book.contains(order1) && "order1 should be in the order book");
    assert(order1->getRemainingQuantity() == 5 && "order1 should have 5 remaining");
    assert(!book.contains(order2) && "order2 should be removed from the order book");

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
    book.matchOrders();

    assert(!book.contains(highestPriorityBuyOrder) && "highestPriorityBuyOrder should be removed from the order book");
    assert(!book.contains(sellOrder) && "sellOrder should be removed from the order book");

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
    book.matchOrders();

    assert(!book.contains(order1) && "order1 should be removed from the order book");
    assert(book.contains(order2) && "order2 should be in the order book");
    assert(order2->getRemainingQuantity() == 3 && "order2 should have 3 remaining");
    assert(!book.contains(order3) && "order3 should be removed from the order book");

    std::cout << "timePriorityMatchingTest() passed!\n";
}

void modifyValidOrderTest() {
    OrderBook book;

    const OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 5);
    const OrderPointer order2 = std::make_shared<Order>(2, Side::BUY, 5000, 5);
    const OrderPointer order3 = std::make_shared<Order>(3, Side::SELL, 5000, 5);

    book.addOrder(order1);
    book.addOrder(order2);
    const OrderPointer modifiedOrder1 = book.modifyOrder(1, 5000, 10);
    book.addOrder(order3);
    book.matchOrders();

    assert(modifiedOrder1->getPrice() == 5000 && "order1 should have a price of 5000");
    assert(modifiedOrder1->getRemainingQuantity() == 10 && "order1 should have 10 remaining");
    assert(!book.contains(order2) && "order2 should be removed from the order book");

    std::cout << "modifyValidOrderTest() passed!\n";
}

void cancelValidOrderTest() {
    OrderBook book;

    const OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 10);
    
    book.addOrder(order1);
    book.cancelOrder(1);
    book.matchOrders();
    
    assert(!book.contains(order1) && "order1 should be removed from the order book");

    std::cout << "cancelValidOrderTest() passed!\n";
}

void cancelFilledOrderTest() {
    OrderBook book;

    const OrderPointer order1 = std::make_shared<Order>(1, Side::BUY, 10000, 10);
    const OrderPointer order2 = std::make_shared<Order>(2, Side::SELL, 10000, 10);

    book.addOrder(order1);
    book.addOrder(order2);
    book.matchOrders();

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
    constexpr int NUM_ORDERS = 5000000;

    const auto start = high_resolution_clock::now();

    for (int i = 0; i < NUM_ORDERS; i++) {
        generator.nextState();
        Side orderSide = generator.pickOrderSide();
        std::uint32_t orderPrice = generator.generateOrderPrice(10000, orderSide, 1.0, 2.5); // reference price is $100.00
        std::uint32_t orderSize = generator.generateOrderSize(10.0, 1.7);

        OrderPointer order = std::make_shared<Order>(i, orderSide, orderPrice, orderSize);
        book.addOrder(order);
        book.matchOrders();
    }

    const auto end = high_resolution_clock::now();
    const double elapsed = duration<double>(end - start).count();

    std::cout << "5 Million Orders Benchmark (Add Only):\n";
    std::cout << "Processed " << NUM_ORDERS << " orders in " << elapsed << " seconds.\n";
    std::cout << "Throughput: " << (NUM_ORDERS / elapsed) << " orders/sec\n\n";
}

void benchmarkFiveMillionOperations() {
    using namespace std::chrono;
    OrderBook book;

    TransitionMatrix matrix = {
        {0.80, 0.10, 0.10}, // Neutral
        {0.10, 0.85, 0.05}, // Buy Pressure
        {0.10, 0.05, 0.85} // Sell Pressure
    };
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution actionDist(0.0, 1.0);

    OrderGenerator generator(matrix, rng);

    std::vector<std::uint64_t> activeOrderIds;
    constexpr int NUM_OPS = 5000000;
    int adds = 0, cancels = 0, modifies = 0;

    const auto start = high_resolution_clock::now();

    for (int i = 0; i < NUM_OPS; i++) {
        double action = actionDist(rng);
        if (action < 0.6 || activeOrderIds.empty()) {
            // Add order
            generator.nextState();
            Side orderSide = generator.pickOrderSide();
            std::uint32_t orderPrice = generator.generateOrderPrice(10000, orderSide, 1.0, 2.5); // reference price is $100.00
            std::uint32_t orderSize = generator.generateOrderSize(10.0, 1.7);

            OrderPointer order = std::make_shared<Order>(i, orderSide, orderPrice, orderSize);
            book.addOrder(order);
            book.matchOrders();
            activeOrderIds.push_back(i);
            adds++;
        }
        else if (action < 0.8 && !activeOrderIds.empty()) {
            // Cancel random order
            std::uniform_int_distribution<std::size_t> indexDist(0, activeOrderIds.size() - 1);
            const std::size_t index = indexDist(rng);
            const std::uint32_t cancelId = activeOrderIds[index];
            try {
                book.cancelOrder(cancelId);
                cancels++;
            }
            catch (const std::logic_error& e) {}

            // O(1) removal
            activeOrderIds[index] = activeOrderIds.back();
            activeOrderIds.pop_back();
        }
        else if (!activeOrderIds.empty()) {
            // Modify random order
            std::uniform_int_distribution<std::size_t> indexDist(0, activeOrderIds.size() - 1);
            const std::size_t index = indexDist(rng);
            const std::uint32_t modifyId = activeOrderIds[index];
            try {
                const std::uint32_t newPrice = generator.generateOrderPrice(10000, book.getOrderByID(modifyId)->getSide(), 1.0, 2.5);
                const std::uint32_t newQty = generator.generateOrderSize(10.0, 1.7);

                book.modifyOrder(modifyId, newPrice, newQty);
                book.matchOrders();
                modifies++;
            }
            catch (const std::logic_error& e) {}
        }
    }

    const auto end = high_resolution_clock::now();
    const double elapsed = duration<double>(end - start).count();

    std::cout << "5 Million Operations Benchmark (Add/Cancel/Modify):\n";
    std::cout << "Adds: " << adds << ", Cancels: " << cancels << ", Modifies: " << modifies << "\n";
    std::cout << "Processed " << NUM_OPS << " operations in " << elapsed << " seconds.\n";
    std::cout << "Throughput: " << (NUM_OPS / elapsed) << " ops/sec\n\n";
}

int main() {
    std::cout << "UNIT TESTS\n";
    std::cout << "----------------\n";
    completeFillTest();
    partialFillTest();
    multiplePriceLevelTest();
    timePriorityMatchingTest();
    modifyValidOrderTest();
    cancelValidOrderTest();
    cancelFilledOrderTest();
    cancelNonExistentOrderTest();

    std::cout << "All tests passed!\n";
    std::cout << "----------------\n";

    std::cout << "BENCHMARKING\n";
    std::cout << "----------------\n";
    benchmarkFiveMillionOrders();
    benchmarkFiveMillionOperations();

    return 0;
}
