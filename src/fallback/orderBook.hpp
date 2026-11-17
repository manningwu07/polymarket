#pragma once

#include "../order.hpp"
#include "orderHeap.hpp"
#include "../hotpath/orderHandle.hpp"
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_flat_set.hpp>
#include <optional>

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

        std::vector<Order> snapshot(Side side);
        
    private:
        Sequence next_sequence_ = 1;
        boost::unordered_flat_map<OrderId, Entry> logged_ids; // live OrderId -> Entry
        boost::unordered_flat_set<OrderId> seen_set;
        
        // Buy
        OrderHeap<BidCompare> order_heap_buy;
        OrderHandle buy_handle;
        std::vector<OrderId> buy_ids; // handle -> id
        
        // Sell
        OrderHeap<AskCompare> order_heap_sell;
        OrderHandle sell_handle;
        std::vector<OrderId> sell_ids; // handle -> id
        
        
        std::optional<Entry> getEntry(const OrderId& id);
        Handle register_order(const OrderId& id, Side side);
        Execution update(const OrderId& id, Order_Internal incoming);
};