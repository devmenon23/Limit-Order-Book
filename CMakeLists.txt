cmake_minimum_required(VERSION 3.31)
project(Order_Book)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(Order_Book
        MarkovParetoOrderGenerator.cpp
        MarkovParetoOrderGenerator.h
        Order.cpp
        Order.h
        OrderBook.cpp
        OrderBook.h
        OrderBookTests.cpp
        OrderBookTests.h)
