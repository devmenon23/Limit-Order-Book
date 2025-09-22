#pragma once
#include "Order_Book/Order.h"

/**
 * @brief All command types
 */
enum class CommandType {
    ADD,
    MODIFY,
    CANCEL
};

using SeqNum = uint64_t;

/**
 * @brief Represents a command to be sent to the engine
 */
struct Command {
    CommandType type;
    SeqNum seqNum;

    IdNumber idNumber;
    Side side;
    Price price;
    Quantity qty;
};
