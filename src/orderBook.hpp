#pragma once

#include <unordered_set>
#include "order.hpp"
#include "orderHeap.hpp"

using namespace std;

class OrderBook {
    public:
        Execution submit_limit(OrderId id, Side side, 
            Price price, Quantity quantity);
        Execution submit_market(OrderId id, Side side, Quantity quantity);
        Order cancel(OrderId id);
        Execution modify(OrderId, Price newPrice, Quantity newQuantity);

        vector<Order> snapshot(Side side);
        
    private:
        Sequence next_sequence_ = 1;
        unordered_set<OrderId> logged_ids;
        // Buy
        OrderHeap<BidCompare> order_heap_buy;
        // Sell
        OrderHeap<AskCompare> order_heap_sell;
        
        optional<pair<Side, size_t>> findId(OrderId id);
        Execution update(Order incoming);
};