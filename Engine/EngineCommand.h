#pragma once
#include "Order_Book/Order.h"

enum class CommandType {
    ADD,
    MODIFY,
    CANCEL
};

using SeqNum = uint64_t;

struct Command {
    CommandType type;
    SeqNum seqNum;

    IdNumber idNumber;
    Side side;
    Price price;
    Quantity qty;
};
