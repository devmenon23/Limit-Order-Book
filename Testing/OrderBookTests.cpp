#include "OrderBookTests.h"

static void waitUntil(Engine& eng, SeqNum target) {
    while (eng.lastProcessed() < target) {
        std::this_thread::yield();
    }
}

void completeFillTest() {
    Engine eng;
    eng.start();

    Command c1 = {CommandType::ADD};
    c1.idNumber = 1; c1.side = Side::BUY; c1.price = 10000; c1.qty = 5;
    eng.submit(c1);

    Command c2 = {CommandType::ADD};
    c2.idNumber = 2; c2.side = Side::SELL; c2.price = 9900; c2.qty = 5;
    const SeqNum lastSeqNum = eng.submit(c2);

    waitUntil(eng, lastSeqNum);
    eng.stop();

    const OrderBook& book = eng.getBook();

    assert(!book.contains(1) && "order1 should be removed from the order book");
    assert(!book.contains(2) && "order2 should be removed from the order book");

    std::cout << "completeFillTest() passed!\n";
}

void partialFillTest() {
    Engine eng;
    eng.start();

    Command c1 = {CommandType::ADD};
    c1.idNumber = 1; c1.side = Side::BUY; c1.price = 10000; c1.qty = 10;
    eng.submit(c1);

    Command c2 = {CommandType::ADD};
    c2.idNumber = 2; c2.side = Side::SELL; c2.price = 9900; c2.qty = 5;
    const SeqNum lastSeqNum = eng.submit(c2);

    waitUntil(eng, lastSeqNum);
    eng.stop();

    const OrderBook& book = eng.getBook();
    OrderPointer order1 = book.getOrderByID(1);

    assert(book.contains(1) && "order1 should be in the order book");
    assert(order1->getRemainingQuantity() == 5 && "order1 should have 5 remaining");
    assert(!book.contains(2) && "order2 should be removed from the order book");

    std::cout << "partialFillTest() passed!\n";
}

void multiplePriceLevelTest() {
    Engine eng;
    eng.start();

    for (int i = 0; i < 100; i++) {
        Command c = {CommandType::ADD};
        c.idNumber = i; c.side = Side::BUY; c.price = std::floor(i/10); c.qty = 10; // 10 order per price level
        eng.submit(c);
    }

    Command c1 = {CommandType::ADD};
    c1.idNumber = 100; c1.side = Side::SELL; c1.price = 9; c1.qty = 10;
    eng.submit(c1);

    Command c2 = {CommandType::ADD};
    c2.idNumber = 101; c2.side = Side::SELL; c2.price = 9; c2.qty = 5;
    const SeqNum lastSeqNum = eng.submit(c2);

    waitUntil(eng, lastSeqNum);
    eng.stop();

    const OrderBook& book = eng.getBook();
    OrderPointer secondHighestPriorityBuyOrder = book.getOrderByID(91);

    assert(!book.contains(90) && "highestPriorityBuyOrder should be removed from the order book");
    assert(secondHighestPriorityBuyOrder->getRemainingQuantity() == 5 && "secondHighestPriorityBuyOrder should have 5 remaining");
    assert(!book.contains(101) && "sellOrder2 should be removed from the order book");

    std::cout << "multiplePriceLevelTest() passed!\n";
}

void timePriorityMatchingTest() {
    Engine eng;
    eng.start();

    Command c1 = {CommandType::ADD};
    c1.idNumber = 1; c1.side = Side::BUY; c1.price = 10000; c1.qty = 5;
    eng.submit(c1);

    Command c2 = {CommandType::ADD};
    c2.idNumber = 2; c2.side = Side::BUY; c2.price = 10000; c2.qty = 10;
    eng.submit(c2);

    Command c3 = {CommandType::ADD};
    c3.idNumber = 3; c3.side = Side::SELL; c3.price = 10000; c3.qty = 12;
    const SeqNum lastSeqNum = eng.submit(c3);

    waitUntil(eng, lastSeqNum);
    eng.stop();

    const OrderBook& book = eng.getBook();
    OrderPointer order2 = book.getOrderByID(2);

    assert(!book.contains(1) && "order1 should be removed from the order book");
    assert(book.contains(2) && "order2 should be in the order book");
    assert(order2->getRemainingQuantity() == 3 && "order2 should have 3 remaining");
    assert(!book.contains(3) && "order3 should be removed from the order book");

    std::cout << "timePriorityMatchingTest() passed!\n";
}

void modifyValidOrderTest() {
    Engine eng;
    eng.start();

    Command c1 = {CommandType::ADD};
    c1.idNumber = 1; c1.side = Side::BUY; c1.price = 10000; c1.qty = 5;
    eng.submit(c1);

    Command c2 = {CommandType::ADD};
    c2.idNumber = 2; c2.side = Side::BUY; c2.price = 5000; c2.qty = 5;
    eng.submit(c2);

    Command c3 = {CommandType::MODIFY};
    c3.idNumber = 1; c3.price = 5000; c3.qty = 10;
    eng.submit(c3);

    Command c4 = {CommandType::ADD};
    c4.idNumber = 3; c4.side = Side::SELL; c4.price = 5000; c4.qty = 5;
    const SeqNum lastSeqNum = eng.submit(c4);

    waitUntil(eng, lastSeqNum);
    eng.stop();

    const OrderBook& book = eng.getBook();
    OrderPointer modifiedOrder1 = book.getOrderByID(1);

    assert(modifiedOrder1->getPrice() == 5000 && "order1 should have a price of 5000");
    assert(modifiedOrder1->getRemainingQuantity() == 10 && "order1 should have 10 remaining");
    assert(!book.contains(2) && "order2 should be removed from the order book");

    std::cout << "modifyValidOrderTest() passed!\n";
}

void cancelValidOrderTest() {
    Engine eng;
    eng.start();

    Command c1 = {CommandType::ADD};
    c1.idNumber = 1; c1.side = Side::BUY; c1.price = 10000; c1.qty = 10;
    eng.submit(c1);

    Command c2 = {CommandType::CANCEL};
    c2.idNumber = 1;
    const SeqNum lastSeqNum = eng.submit(c2);

    waitUntil(eng, lastSeqNum);
    eng.stop();

    const OrderBook& book = eng.getBook();

    assert(!book.contains(1) && "order1 should be removed from the order book");

    std::cout << "cancelValidOrderTest() passed!\n";
}

void benchmarkFiveMillionOrders() {
    using namespace std::chrono;

    TransitionMatrix matrix = {
        {0.80, 0.10, 0.10}, // Neutral
        {0.10, 0.85, 0.05}, // Buy Pressure
        {0.10, 0.05, 0.85} // Sell Pressure
    };

    std::random_device rd;
    std::mt19937 rng(rd());

    OrderGenerator generator(matrix, rng);
    constexpr int NUM_ORDERS = 5000000;

    Engine eng;
    eng.start();

    const auto start = high_resolution_clock::now();

    SeqNum lastSeqNum = 0;
    for (int i = 0; i < NUM_ORDERS; i++) {
        generator.nextState();
        Side orderSide = generator.pickOrderSide();
        std::uint32_t orderPrice = generator.generateOrderPrice(10000, orderSide, 1.0, 2.5); // reference price is $100.00
        std::uint32_t orderSize = generator.generateOrderSize(10.0, 1.7);

        Command c = {CommandType::ADD};
        c.idNumber = i; c.side = orderSide; c.price = orderPrice; c.qty = orderSize;
        lastSeqNum = eng.submit(c);
    }

    waitUntil(eng, lastSeqNum);
    eng.stop();

    const auto end = high_resolution_clock::now();
    const double elapsed = duration<double>(end - start).count();

    std::cout << "5 Million Orders Benchmark (Add Only):\n";
    std::cout << "Processed " << NUM_ORDERS << " orders in " << elapsed << " seconds.\n";
    std::cout << "Throughput: " << (NUM_ORDERS / elapsed) << " orders/sec\n\n";
}

void benchmarkFiveMillionOperations() {
    using namespace std::chrono;

    TransitionMatrix matrix = {
        {0.80, 0.10, 0.10}, // Neutral
        {0.10, 0.85, 0.05}, // Buy Pressure
        {0.10, 0.05, 0.85} // Sell Pressure
    };
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution actionDist(0.0, 1.0);

    OrderGenerator generator(matrix, rng);

    struct Active { IdNumber idNumber; Side side; };
    std::vector<Active> active;
    constexpr int NUM_OPS = 5000000;
    std::atomic<int> adds = 0, cancels = 0, modifies = 0;

    Engine eng;
    eng.start();

    eng.set_error_handler([&](const Command& c, const std::exception& e){
        (void)e;
        switch (c.type) {
            case CommandType::ADD: adds.fetch_sub(1, std::memory_order_relaxed); break;
            case CommandType::CANCEL: cancels.fetch_sub(1, std::memory_order_relaxed); break;
            case CommandType::MODIFY: modifies.fetch_sub(1, std::memory_order_relaxed); break;
        }
    });

    const auto start = high_resolution_clock::now();

    SeqNum lastSeqNum = 0;
    for (int i = 0; i < NUM_OPS; i++) {
        const double action = actionDist(rng);
        if (action < 0.6 || active.empty()) {
            // Add order
            generator.nextState();
            Side orderSide = generator.pickOrderSide();
            std::uint32_t orderPrice = generator.generateOrderPrice(10000, orderSide, 1.0, 2.5); // reference price is $100.00
            std::uint32_t orderSize = generator.generateOrderSize(10.0, 1.7);

            Command c = {CommandType::ADD};
            c.idNumber = i; c.side = orderSide; c.price = orderPrice; c.qty = orderSize;
            lastSeqNum = eng.submit(c);

            active.push_back({c.idNumber, orderSide});
            adds.fetch_add(1, std::memory_order_relaxed);
        }
        else if (action < 0.8 && !active.empty()) {
            // Cancel random order
            std::uniform_int_distribution<std::size_t> indexDist(0, active.size() - 1);
            const std::size_t index = indexDist(rng);
            const std::uint32_t cancelId = active[index].idNumber;

            Command c = {CommandType::CANCEL};
            c.idNumber = cancelId;
            lastSeqNum = eng.submit(c);

            // O(1) removal
            active[index] = active.back();
            active.pop_back();
            cancels.fetch_add(1, std::memory_order_relaxed);
        }
        else if (!active.empty()) {
            // Modify random order
            std::uniform_int_distribution<std::size_t> indexDist(0, active.size() - 1);
            const std::size_t index = indexDist(rng);
            const auto [modifyId, side] = active[index];

            const std::uint32_t newPrice = generator.generateOrderPrice(10000, side, 1.0, 2.5);
            const std::uint32_t newSize = generator.generateOrderSize(10.0, 1.7);

            Command c = {CommandType::MODIFY};
            c.idNumber = modifyId; c.price = newPrice; c.qty = newSize;
            lastSeqNum = eng.submit(c);

            modifies.fetch_add(1, std::memory_order_relaxed);
        }
    }

    waitUntil(eng, lastSeqNum);
    eng.stop();

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

    std::cout << "All tests passed!\n";
    std::cout << "----------------\n";

    std::cout << "BENCHMARKING\n";
    std::cout << "----------------\n";
    benchmarkFiveMillionOrders();
    benchmarkFiveMillionOperations();

    return 0;
}
