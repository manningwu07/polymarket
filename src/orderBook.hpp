#pragma once

#include <unordered_set>
#include "order.hpp"
#include "orderHeap.hpp"

using namespace std;

class OrderBook {
    public:
        Execution submit_limit(const OrderId&, Side, Price, Quantity);
        Execution submit_market(const OrderId&, Side, Quantity);
        Order cancel(const OrderId&);
        Execution modify(const OrderId&, Price, Quantity);

        vector<Order> snapshot(Side side);
        
    private:
        Sequence next_sequence_ = 1;
        unordered_set<OrderId> logged_ids;
        // Buy
        OrderHeap<BidCompare> order_heap_buy;
        // Sell
        OrderHeap<AskCompare> order_heap_sell;
        
        optional<pair<Side, size_t>> findId(const OrderId& id);
        Execution update(const Order& incoming);
};