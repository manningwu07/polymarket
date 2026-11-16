#include "bookside.hpp"
#include "orderPool.hpp"
#include <cstdint>

using namespace std;

BookSide::BookSide(Side side) : side_(side){
    best_price = empty_best();
    levels.resize(kMaxPrice + 1, {.head = null_handle, .tail = null_handle});
}
   
/*
 * Pushes to the back of the linked list
 * Used for submit limits
 * Levels only has specific range, rest should go to heap
 * Range: [0, kMaxPrice], integers
 */
Handle BookSide::push(Price price, Quantity quantity, Sequence sequence){
    size_t i = to_index(price);

    // Insert pools to get handle
    Handle handle = pool.insert(price, quantity, sequence);
    // Insert into pool to get handle
    OrderLevel level = levels[i];
    /*
     * 2 cases:
     * a) H & T == null -> First node for that price
     * b) H & T != null -> @ least 1+ exists
     */
    if (level.head == null_handle) {
        level.head = handle;
    }
    if(level.tail != null_handle) {
        pool.modify_next(level.tail, handle);
        pool.modify_prev(handle, level.tail);
    }
    level.tail = handle;
    
    levels[i] = level;
    best_price = side_ == Side::buy ? 
        max(best_price, price) : min(best_price, price);

    return handle;
}

void BookSide::set_quantity(Handle handle, Quantity quantity) {
    pool.get(handle).quantity = quantity;
}

vector<BookSideOrder> BookSide::orders() {
    vector<BookSideOrder> res;
    for (const OrderLevel& level : levels) {
        for (Handle h = level.head; h != null_handle; h = pool.get(h).next) {
            res.push_back(get(h));
        }
    }
    return res;
}

void BookSide::pop(Price price){
    size_t i = to_index(price);
    OrderLevel level = levels[i];
    if(level.head == null_handle) 
        throw_out_of_range("No orders at this price");

    erase(levels[to_index(price)].head); // Hotpathing makes negliable diff
}

void BookSide::erase(Handle handle){
    OrderNode& node = pool.get(handle);
    size_t i = to_index(node.price);
    /*
     * 4 cases
     * a) prev == null && next == null -> Lone node
     * b) prev == null -> head
     * c) next == null -> tail
     * d) prev && next != null -> middle of the list
     */
    OrderLevel level = levels[i];
    if(node.prev == null_handle && node.next == null_handle){
        level.head = null_handle;
        level.tail = null_handle;
    } else if (node.prev == null_handle) {
        level.head = node.next;
        pool.modify_prev(node.next, null_handle);
    } else if(node.next == null_handle){
        level.tail = node.prev;
        pool.modify_next(node.prev, null_handle);
    } else {
        pool.modify_next(node.prev, node.next);
        pool.modify_prev(node.next, node.prev);
    }
    
    levels[i] = level;
    pool.erase(handle);
    find_best_price(static_cast<Price>(i));
}

Price BookSide::getBestPrice() {
    return best_price;
}

// Helper functions:

Price BookSide::empty_best() const {
    return side_ == Side::buy ? numeric_limits<Price>::min() : numeric_limits<Price>::max();
}

/*
 * Called after a level loses an order
 * best only moves if that level was best and is now empty;
 * 
 * the next best is then strictly worse
 * so scan away from it and stop at the first non-empty level.
 * 
 * Might convert to bitmap for constant speedup via SIMD
 * If levels is too big
 * For now its managable
 */ 
void BookSide::find_best_price(int64_t index){
    auto has_orders = [&](int64_t k) { 
        return levels[static_cast<size_t>(k)].head != null_handle; 
    };
    if (index != best_price || has_orders(index)) return;

    if(side_ == Side::buy){
        for (int64_t k = index - 1; k >= 0; --k) {
            if(has_orders(k)) {
                best_price = k;
                return;
            }
        }
    } else {
        for(int64_t k = index + 1; k <= kMaxPrice; ++k){
            if(has_orders(k)) {
                best_price = k;
                return;
            }
        }
    }
    best_price = empty_best();
}