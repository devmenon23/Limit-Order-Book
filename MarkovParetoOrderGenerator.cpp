#include "MarkovParetoOrderGenerator.h"

OrderGenerator::OrderGenerator(TransitionMatrix transitionMatrix, std::mt19937 rng):
    transitionMatrix(transitionMatrix),
    rng(rng),
    state(MarketState::NEUTRAL)
{}

MarketState OrderGenerator::sampleNextState() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double randVal = dist(rng);

    double cumulative = 0.0;
    for (size_t i = 0; i < transitionMatrix[state].size(); i++) {
        cumulative += transitionMatrix[state][i];
        if (cumulative > randVal){
            return static_cast<MarketState>(i);
        }
    }
    return state;
};

MarketState OrderGenerator::nextState() {
    state = sampleNextState();
    return state;
};

Side OrderGenerator::pickOrderSide() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double randVal = dist(rng);

    switch (state) {
        case BUY_PRESSURE:
            return (randVal < 0.9) ? Side::BUY : Side::SELL;
        case SELL_PRESSURE:
            return (randVal < 0.1) ? Side::BUY : Side::SELL;
        case NEUTRAL:
            return (randVal < 0.5) ? Side::BUY : Side::SELL;
    }
};

double OrderGenerator::samplePareto(double xmin, double alpha) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double u = dist(rng);
    if (u == 0.0) u = std::numeric_limits<double>::min(); // avoid division by zero
    double x = xmin / std::pow(u, 1.0 / alpha); // use pareto inverse transform sampling to 
    return x;
};

std::uint32_t OrderGenerator::generateOrderPrice(std::uint32_t referencePrice, Side side, double xmin, double alpha) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double randVal = dist(rng);
    if (randVal < 0.2) return referencePrice; // generate market orders 20% of the time

    double offset = samplePareto(xmin, alpha);
    int ticks = static_cast<int>(std::round(offset));
    int sign = (side == Side::BUY) ? -1 : 1; // subtract if buying and add if selling to reference price 

    int newPrice = referencePrice + sign * ticks;
    if (newPrice < 1) newPrice = 1; // don't let price go to 0 or negative
    return static_cast<std::uint32_t>(newPrice);
};

std::uint32_t OrderGenerator::generateOrderSize(double xmin, double alpha) {
    return static_cast<std::uint32_t>(std::round(samplePareto(xmin, alpha)));
};


