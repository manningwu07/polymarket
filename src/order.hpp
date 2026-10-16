#pragma once

#include <chrono>
#include <cstdint>

using namespace std;

using OrderId = uint64_t;
using Price = int64_t;       // integer ticks
using Quantity = int64_t;
using Sequence = uint64_t;

enum Side: uint8_t {
    Buy, 
    Sell
};

struct Order{
    OrderId id;
    Side side;
    Price price; // divide by 100 to get actual price
    Quantity quantity;

    Sequence sequence;
    chrono::system_clock created_at;
};
