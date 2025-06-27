cmake_minimum_required(VERSION 3.31)
project(Limit_Order_Book)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")


include_directories(.)


add_executable(Limit_Order_Book
        "Order Generator/MarkovParetoOrderGenerator.cpp"
        "Order Book/Order.cpp"
        "Order Book/OrderBook.cpp"
        "Testing/OrderBookTests.cpp"
)
