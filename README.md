# Limit-Order-Book
This Limit Order Book (LOB) is a high-performance, configurable implementation in modern C++. It is able to execute over 6,000,000 orders per second and perform over 100,000 general operations per second. The LOB's primary goal is to provide a robust core data structure for electronic trading systems, HFT, and quantitative research with a focus on efficiency, correctness, and maintainability.

Becnhmarking the performance of the Order Book was quite challenging as I had to simulate the market to get realistic results. I simulated a real-world style order flow using a Markov chain/Pareto process that provides automated realistic benchmarking.

## Background
### Matching Engine
A matching engine is the core component of electronic trading platforms—used by exchanges, brokers, and market simulations to facilitate the buying and selling of assets (such as stocks, crypto, or commodities). Its main purpose is to match buyers' and sellers' interests to execute trades in a fair and efficient fashion. The engine maintains a an electronic order book holding buy (“bid”) and sell (“ask”) orders, organized by price and arrival time. Having a robust, high-performance matching engine enables realistic simulation, research, and strategy testing in finance. It’s the backbone for understanding market microstructure and building low-latency trading systems.

## Project Tree
```bash
Limit-Order-Book/
├── Order Book/            * files that make up the order book
│   ├── Order.cpp
│   ├── Order.h
│   ├── OrderBook.cpp
│   └── OrderBook.h
├── Order Generator/       * files to generate order data
│   ├── MarkovParetoOrderGenerator.cpp
│   └── MarkovParetoOrderGenerator.h
├── Testing/               * files with unit tests and benchmarking
│   ├── OrderBookTests.cpp
│   └── OrderBookTests.h
├── CMakeLists.txt
├── cmake-build-debug/
└── cmake-build-release/
├── README.md
```
