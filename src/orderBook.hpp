#pragma once

#include "order.hpp"
#include "orderHeap.hpp"
#include <unordered_map>

using namespace std;

struct Entry {
  Handle handle;
  Side side;
};

class OrderBook {
    public:
        Execution submit_limit(const OrderId&, Side, Price, Quantity);
        Execution submit_market(const OrderId&, Side, Quantity);
        Order cancel(const OrderId&);
        Execution modify(const OrderId&, Price, Quantity);

        vector<Order> snapshot(Side side);
        
    private:
        Sequence next_sequence_ = 1;
        unordered_map<OrderId, Entry> logged_ids; // OrderId -> handle
        
        // Buy
        OrderHeap<BidCompare> order_heap_buy;
        Handle next_buy_handle_ = 1;
        // Sell
        OrderHeap<AskCompare> order_heap_sell;
        Handle next_sell_handle_ = 1;
        
        optional<pair<Side, int>> findId(const OrderId& id);
        Execution update(const Order& incoming);
};