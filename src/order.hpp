#pragma once

#include <cstdint>
#include <vector>
#include <optional>

using namespace std;

using OrderId = string;
using Price = int64_t;       // integer ticks, divide by 100 to get actual price
using Quantity = int64_t;
using Sequence = uint64_t;

enum Side: uint8_t {
    buy, 
    sell
};

enum OrderType {
    limit,
    market
};


struct Order {
    OrderId id;
    Side side;
    optional<Price> price; // divide by 100 to get actual price
    Quantity quantity;
    
    OrderType ordertype;
    Sequence sequence;
    // chrono::system_clock created_at; (not optimal but would add in future)
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