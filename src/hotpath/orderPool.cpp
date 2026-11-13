#include "orderPool.hpp"
#include <stdexcept>

OrderPool::OrderPool() {
    order_pool.reserve(80'000);
}

OrderNode& OrderPool::get(Handle handle) {
    if(handle >= order_pool.size())
        throw out_of_range("Handle out of range");
    return order_pool[handle];
}

void OrderPool::erase(Handle handle){
    dead_handles.push_back(handle);
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

void OrderPool::modify_next(Handle index, Handle handle) {
    order_pool[index].next = handle;
}

void OrderPool::modify_prev(Handle index, Handle handle) {
    order_pool[index].prev = handle;
}