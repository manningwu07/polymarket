#pragma once

#include "../order.hpp"

/*
 * Manages handle data states + logic
 */
class OrderHandle {
    public:
        OrderHandle() { free_handles.reserve(64); } // measured max 35

        Handle assign_handle() {
            if (free_handles.empty()) return next_handle_++;
            Handle handle = free_handles.back();
            free_handles.pop_back();
            return handle;
        }

        void remove_handle(Handle handle) { free_handles.push_back(handle); }

    private:
        vector<Handle> free_handles;
        Handle next_handle_ = 0;
};
