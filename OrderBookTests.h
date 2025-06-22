#pragma once
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
 * @brief Tests if matching occurs at best price in an order book with multiple price levels
 */
void multiplePriceLevelTest();
/**
 * @brief Tests matching orders according to time priority in the order book
 */
void timePriorityMatchingTest();
/**
 * @brief Tests modifying a valid order in the order book
 */
void modifyValidOrderTest();
/**
 * @brief Tests canceling valid orders in the order book
 */
void cancelValidOrderTest();
/**
 * @brief Tests canceling filled orders in the order book
 */
void cancelFilledOrderTest();
/**
 * @brief Tests canceling non-existent orders in the order book
 */
void cancelNonExistentOrderTest();
/**
 * @brief Benchmarks the efficiency of simulating 5,000,000 orders in the order book
 */
void benchmarkFiveMillionOrders();
