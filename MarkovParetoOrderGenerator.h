#pragma once
#include <iostream>
#include <random>
#include <vector>
#include "Order.h"

enum MarketState{
    NEUTRAL,
    BUY_PRESSURE,
    SELL_PRESSURE
};

using TransitionMatrix = std::vector<std::vector<double>>;

class OrderGenerator {
private:
    TransitionMatrix transitionMatrix;
    std::mt19937 rng;
    MarketState state;

    MarketState sampleNextState();
    double samplePareto(double xmin, double alpha);
public:
    OrderGenerator(TransitionMatrix transitionMatrix, std::mt19937 rng);
    MarketState nextState();
    Side pickOrderSide();
    std::uint32_t generateOrderPrice(std::uint32_t referencePrice, Side side, double xmin = 1.0, double alpha = 2.7);
    std::uint32_t generateOrderSize(double xmin = 10.0, double alpha = 1.7);
};

