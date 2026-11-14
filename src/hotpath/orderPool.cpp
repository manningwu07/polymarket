#include "orderPool.hpp"

OrderPool::OrderPool() {
    order_pool.reserve(80'000);
}

Handle OrderPool::insert(Price price, Quantity quantity, Sequence sequence) {
    OrderNode node = {
        .price = price,
        .quantity = quantity,
        .sequence = sequence,
        .next = null_handle,
        .prev = null_handle,
    };

    Handle handle;
    
    if (dead_handles.empty()){
        handle = static_cast<Handle>(order_pool.size());
        order_pool.push_back(node);
    } else {
        handle = dead_handles.back();
        dead_handles.pop_back();
        order_pool[handle] = node;
    }
    
    return handle;
}