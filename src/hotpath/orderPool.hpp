#pragma once

#include "../order.hpp"
#include <cstdint>
#include <stdexcept>

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
        // Hotpaths:
        OrderNode& get(Handle handle){
            return order_pool.at(handle);
        }
        void erase(Handle handle){
            dead_handles.push_back(handle);
        } // No checks. h must be live; caller must unlink first

        // Coldpath small functions
        void modify_next(Handle index, Handle handle){
            order_pool[index].next = handle;
        }
        void modify_prev(Handle index, Handle handle){
            order_pool[index].prev = handle;
        }

        Handle insert(Price price, Quantity quantity, Sequence sequence);
    private:
        vector<OrderNode> order_pool; // handle -> OrderNode
        vector<Handle> dead_handles;
};
    