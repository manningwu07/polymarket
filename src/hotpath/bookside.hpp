#pragma once

#include "../order.hpp"
#include "orderPool.hpp"

using namespace std;

// if head/tail = -1 -> null 
struct OrderLevel{
    Handle head; // 4
    Handle tail; // 4
};

struct BookSideOrder {
    Price price;
    Quantity quantity;
    Sequence sequence;
    Handle handle;
};

// using Bitmap = uint32_t;

class BookSide {
    public:
        /*
         * Insertion
         * Deletion
         * Modification of a specific node
         * Linking and unlinking of the head and tail
         */
        static constexpr Price kMaxPrice = 2000; // levels cover prices [0, kMaxPrice]
        static bool covers(Price price) { return price >= 0 && price <= kMaxPrice; }

        BookSide(Side side);

        // Inlinable Hotpath
        BookSideOrder get(Handle handle){
            const OrderNode& node = pool.get(handle);
            return {
                .price = node.price,
                .quantity = node.quantity,
                .sequence = node.sequence,
                .handle = handle
            };
        } // handle must be live

        BookSideOrder top(Price price){
            Handle head = levels[to_index(price)].head;
            if(head == null_handle)
                throw_out_of_range("No orders at this price");
            return get(head);
        }

        // Modifiers
        Handle push(Price price, Quantity quantity, Sequence sequence); // Insert in the back
        
        // Erase + push: the order loses queue position and gets a new handle. 
        // Throws before touching the book if the new price is out of range.
        // Returns the new handle.
        void pop(Price price); // Knocks out first in
        void erase(Handle handle); // Arbitrary delete

        void set_quantity(Handle handle, Quantity quantity); // Partial fill; keeps queue position

        // Getters
        Price getBestPrice();
        bool empty() const { return best_price == empty_best(); }
        vector<BookSideOrder> orders(); // Every resting order, unordered across levels

    private:
        OrderPool pool;
        vector<OrderLevel> levels; // price -> LL Head + Tail
        Price best_price;
        const Side side_; 

        // Best price of an empty side: loses to any real price
        Price empty_best() const;
        void find_best_price(int64_t i);

        /*
         * Converts price -> index
         */
        size_t to_index(Price price) const {
            if (!covers(price)) throw_out_of_range("Price not covered by levels");
            return static_cast<size_t>(price);
        }
        
        [[noreturn]] [[gnu::noinline]] static void throw_out_of_range(const char* str) {
            throw out_of_range(str);
        }
};