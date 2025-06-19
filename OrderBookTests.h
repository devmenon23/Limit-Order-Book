#include "OrderBook.h"
#include "MarkovParetoOrderGenerator.h"
#include <iostream>
#include <chrono>

/**
 * @brief Tests filling orders completely in the order book
 */
void completeFillTest();
/**
 * @brief Tests filling orders partially in the order book
 */
void partialFillTest();
/**
 * @brief Tests matching orders according to time priority in the order book
 */
void timePriorityMatchingTest();
/**
 * @brief Tests canceling orders in the order book
 */
void cancelTest();

/**
 * @brief Benchmarks the efficiency of simulating 5,000,000 orders in the order book
 */
void benchmarkFiveMillionOrders();
