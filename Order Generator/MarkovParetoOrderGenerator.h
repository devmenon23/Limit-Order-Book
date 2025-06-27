#pragma once
#include <iostream>
#include <random>
#include <vector>
#include "Order Book/Order.h"

/**
 * @brief All market states
 */
enum class MarketState{
    NEUTRAL,
    BUY_PRESSURE,
    SELL_PRESSURE
};

using TransitionMatrix = std::vector<std::vector<double>>;

/**
 * @brief Generates synthetic orders using a Markov chain for market state and Pareto distributions for price/size
 */
class OrderGenerator {
private:
    const TransitionMatrix& transitionMatrix;
    std::mt19937& rng;
    MarketState state;

    /**
     * @brief Samples the next market state depending on the current state and the transition matrix
     * @return The market state after the transition
     */
    MarketState sampleNextState() const;
    /**
     * @brief Samples a random value from the Pareto distribution with specified properties
     * 
     * @param xMin The minimum x for the Pareto distribution
     * @param alpha The alpha for the Pareto distribution
     * 
     * @return The value sampled from the Pareto distribution
     */
    double samplePareto(double xMin, double alpha) const;
public:
    /**
     * @brief Constructs an order generator with the given transition matrix and random seed
     * 
     * @param transitionMatrix The Markov transition matrix
     * @param rng The random number generator
     */
    OrderGenerator(const TransitionMatrix& transitionMatrix, std::mt19937& rng);
    /**
     * @brief Advances to the next market state according to the Markov model
     * 
     * @return The new market state.
     */
    MarketState nextState();
    /**
     * @brief Picks the side for the next order based on the current market state
     * 
     * @return The side of the order
     */
    Side pickOrderSide() const;
    /**
     * @brief Generates a price for the next order using a Pareto distribution with the given properties
     * 
     * @param referencePrice The reference price to base the new order price on 
     * @param side The side of the order
     * @param xMin The minimum price offset
     * @param alpha The Pareto shape parameter
     * 
     * @return The generated order price
     */
    std::uint32_t generateOrderPrice(std::uint32_t referencePrice, Side side, double xMin = 1.0, double alpha = 2.7) const;
        /**
     * @brief Generates a size for the next order using a Pareto distribution with the given properties
     * 
     * @param xMin The minimum size
     * @param alpha The Pareto shape parameter
     * 
     * @return The generated order size
     */
    std::uint32_t generateOrderSize(double xMin = 10.0, double alpha = 1.7) const;
};

