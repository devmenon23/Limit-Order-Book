cmake_minimum_required(VERSION 3.31)
project(Limit_Order_Book)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")


include_directories(.)

find_package(OpenMP REQUIRED)

add_executable(Limit_Order_Book
        "Order_Generator/MarkovParetoOrderGenerator.cpp"
        "Order_Book/Order.cpp"
        "Order_Book/OrderBook.cpp"
        "Testing/OrderBookTests.cpp"
        "Engine/EngineCommand.h"
        "Engine/BoundedQueue.h"
        "Engine/BoundedQueue.cpp"
        "Engine/Engine.h"
        "Engine/Engine.cpp"
)
