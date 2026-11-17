#pragma once

#include "order.hpp"
#include "fallback/orderHeap.hpp"
#include "hotpath/bookside.hpp"
#include "hotpath/orderHandle.hpp"
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_flat_set.hpp>

struct Entry {
  Handle handle;
  Side side;
  bool in_heap; // price outside BookSide's window; handle belongs to the heap
};

// One side of the book: prices BookSide covers rest in levels, the rest in the heap.
template <class Compare>
struct HalfBook {
    explicit HalfBook(Side side) : levels(side) {}

    BookSide levels;
    std::vector<OrderId> level_ids; // BookSide handle -> id

    OrderHeap<Compare> heap;
    OrderHandle heap_handles;
    std::vector<OrderId> heap_ids; // heap handle -> id
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

        HalfBook<BidCompare> bids{Side::buy};
        HalfBook<AskCompare> asks{Side::sell};
        
        // Unlinks a resting order from its side and returns it
        BookSideOrder remove(const Entry& entry);
        template <class Compare>
        void rest(HalfBook<Compare>& half, const OrderId& id, const Order_Internal& order);
        Execution update(const OrderId& id, Order_Internal incoming);
};
