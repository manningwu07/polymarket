#pragma once

#include "order.hpp"
#include "orderHeap.hpp"
#include <boost/unordered/unordered_flat_map.hpp>

using namespace std;

struct Entry {
  Handle handle;
  Side side;
};

class OrderBook {
    public:
        OrderBook();
        Execution submit_limit(const OrderId&, Side, Price, Quantity);
        Execution submit_market(const OrderId&, Side, Quantity);
        Order cancel(const OrderId&);
        Execution modify(const OrderId&, Price, Quantity);

        vector<Order> snapshot(Side side);
        
    private:
        Sequence next_sequence_ = 1;
        boost::unordered_flat_map<OrderId, Entry> logged_ids; // OrderId -> handle
        
        // Buy
        OrderHeap<BidCompare> order_heap_buy;
        Handle next_buy_handle_ = 1;
        vector<OrderId> buy_ids; // handle -> id
        // Sell
        OrderHeap<AskCompare> order_heap_sell;
        Handle next_sell_handle_ = 1;
        vector<OrderId> sell_ids; // handle -> id
        
        optional<pair<Side, int>> findId(const OrderId& id);
        Execution update(const OrderId& id, Order_Internal incoming);
};