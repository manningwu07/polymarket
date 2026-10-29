#pragma once

#include "order.hpp"

/*
 * Manages handle data states + logic
 */
class OrderHandle {
    public:
        OrderHandle();
        Handle assign_handle();
        void remove_handle(Handle handle);
        
    private:
        vector<Handle> free_handles;
        Handle next_handle_ = 0;
};
