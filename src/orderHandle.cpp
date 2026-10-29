#include "orderHandle.hpp"
#include "order.hpp"

OrderHandle::OrderHandle() {
    free_handles.reserve(1<<8); 
}

Handle OrderHandle::assign_handle() {
    Handle handle;
    if (!free_handles.empty()){
        handle = free_handles.back();
        free_handles.pop_back();
    } else {
        handle = next_handle_++;
    }

    return handle;
}

void OrderHandle::remove_handle(Handle handle) {
    free_handles.push_back(handle);
}