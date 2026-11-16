#pragma once

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

using OrderId = string;
using Price = int64_t;       // integer ticks, divide by 100 to get actual price
using Quantity = int64_t;
using Sequence = uint64_t;
using Handle = uint32_t;

const Price kNoPrice = -1;

enum Side: uint8_t {
    buy, 
    sell
};

enum OrderType: uint8_t {
    limit,
    market
};


struct Order {
    OrderId id; // 24
    // divide by 100 to get actual price in $
    // -1 => market price
    Price price = kNoPrice; // 8
    Quantity quantity; // 8
    Sequence sequence; // 8
    // Enums
    OrderType ordertype; // 1
    Side side; // 1
    // chrono::system_clock created_at; (not optimal but would add in future)
};

// Heap
struct Order_Internal {
    // divide by 100 to get actual price in $
    // -1 => market price
    Price price = kNoPrice; // 8
    Quantity quantity; // 8
    Sequence sequence; // 8
    Handle handle; // 4
    // Enums
    OrderType ordertype; // 1
    Side side; // 1
};

struct Fill {
    OrderId buyer_id;
    OrderId seller_id;
    Price price;
    Quantity quantity;
};

struct Execution {
    Order order;
    vector<Fill> fills;
};