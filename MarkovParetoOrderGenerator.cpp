#include "MarkovParetoOrderGenerator.h"

OrderGenerator::OrderGenerator(const TransitionMatrix& transitionMatrix, std::mt19937& rng):
    transitionMatrix(transitionMatrix),
    rng(rng),
    state(MarketState::NEUTRAL)
{}

MarketState OrderGenerator::sampleNextState() const {
    std::uniform_real_distribution dist(0.0, 1.0);
    const double randVal = dist(rng);

    double cumulative = 0.0;
    for (size_t i = 0; i < transitionMatrix[static_cast<int>(state)].size(); i++) {
        cumulative += transitionMatrix[static_cast<int>(state)][i];
        if (cumulative > randVal) {
            return static_cast<MarketState>(i);
        }
    }
    return state;
}

MarketState OrderGenerator::nextState() {
    state = sampleNextState();
    return state;
}

Side OrderGenerator::pickOrderSide() const {
    std::uniform_real_distribution dist(0.0, 1.0);
    const double randVal = dist(rng);

    switch (state) {
        case MarketState::BUY_PRESSURE:
            return (randVal < 0.9) ? Side::BUY : Side::SELL;
        case MarketState::SELL_PRESSURE:
            return (randVal < 0.1) ? Side::BUY : Side::SELL;
        case MarketState::NEUTRAL:
            return (randVal < 0.5) ? Side::BUY : Side::SELL;
    }

    return Side::BUY; // Will never reach; just to guarantee a return statement
};

double OrderGenerator::samplePareto(const double xMin, const double alpha) const {
    std::uniform_real_distribution dist(0.0, 1.0);
    double u = dist(rng);
    if (u == 0.0) u = std::numeric_limits<double>::min(); // avoid division by zero
    double x = xMin / std::pow(u, 1.0 / alpha); // use pareto inverse transform sampling to
    return x;
}

std::uint32_t OrderGenerator::generateOrderPrice(const std::uint32_t referencePrice, const Side side, const double xMin, const double alpha) const {
    std::uniform_real_distribution dist(0.0, 1.0);
    const double randVal = dist(rng);
    if (randVal < 0.2) return referencePrice; // generate market orders 20% of the time

    const double offset = samplePareto(xMin, alpha);
    const int ticks = static_cast<int>(std::round(offset));
    const int sign = (side == Side::BUY) ? -1 : 1; // subtract if buying and add if selling to reference price

    int newPrice = static_cast<int>(referencePrice) + sign * ticks;
    if (newPrice < 1) newPrice = 1; // don't let price go to 0 or negative
    return static_cast<std::uint32_t>(newPrice);
}

std::uint32_t OrderGenerator::generateOrderSize(const double xMin, const double alpha) const {
    return static_cast<std::uint32_t>(std::round(samplePareto(xMin, alpha)));
}


