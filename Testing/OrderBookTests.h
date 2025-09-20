#pragma once
#include "Order_Book/OrderBook.h"
#include "Order_Generator/MarkovParetoOrderGenerator.h"
#include "Engine/Engine.h"
#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>

static void waitUntil(Engine& eng, SeqNum target);

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
 * @brief Benchmarks the efficiency of simulating 5,000,000 orders in the order book
 */
void benchmarkFiveMillionOrders();
/**
 * @brief Benchmarks the efficiency of simulating 5,000,000 operations in the order book
 */
void benchmarkFiveMillionOperations();
