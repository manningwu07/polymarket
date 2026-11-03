#pragma once

#include "../order.hpp"
#include <cstdint>

using namespace std;

const Handle null_handle = UINT32_MAX;

struct OrderNode {
    Price price; // 8
    Quantity quantity; // 8
    Sequence sequence; // 8
    Handle next = null_handle; // 4
    Handle prev = null_handle; // 4
};


class OrderPool{
    public:
        OrderPool();
        OrderNode& get(Handle handle);
        void erase(Handle handle); // No checks. h must be live; caller must unlink first
        Handle insert(Price price, Quantity quantity, Sequence sequence);
        
        void modify_next(Handle index, Handle handle);
        void modify_prev(Handle index, Handle handle);
        
    private:
        vector<OrderNode> order_pool; // handle -> OrderNode
        vector<Handle> dead_handles;
};
    