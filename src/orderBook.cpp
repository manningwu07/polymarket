#include <bits/stdc++.h>
#include <unordered_set>
#include "orderBook.hpp"
#include "order.hpp"

using namespace std;

/*
 * Updates the data structures when new "tick" happens
 * First checks if anything changed
 * Then executes a simulation tick
 */
void OrderBook::update() {
    /*
     * 
     * while both sides nonempty
         look at best bid
         look at best ask
     
         if bid.price < ask.price
             stop
     
         otherwise
             trade min quantities
             remove whichever fills
     */
    return;
}

/*
 * Create a Buy/sell at this quantity, but only at this price or better.
 */
void OrderBook::submit_limit(OrderId id, Side side, 
    Price price, Quantity quantity) {
    Order order = {
        .id=id,
        .side=side,
        .price=price,
        .quantity=quantity,

        .sequence=next_sequence_, 
    };

    // O(log n)
    if (side == Side::Buy) {
        order_heap_buy.push(order);
    } else {
        order_heap_sell.push(order);
    }

    update();
}

template<typename Compare>
void OrderBook::submit_market_helper(OrderId id, OrderHeap<Compare> heap, Quantity quantity) {
    while(heap.size() > 0) {
        Order order = heap.top();
        if (order.quantity > quantity) {
            modify(order.id, order.price, order.quantity - quantity);
            break;
        } else {
            quantity -= order.quantity;
            heap.pop();
        }
    }
}

/*
 * Create a Buy/Sell at this quantity
 * right now at whatever prices are available
 */
void OrderBook::submit_market(OrderId id, Side side, Quantity quantity){
    // Get price -> match, then update?
    // Or we can j directly modify the opposite side top?
    // Or j use match

    if(side == Side::Buy) {
        submit_market_helper(id, order_heap_sell, quantity);
    } else {
        submit_market_helper(id, order_heap_buy, quantity);
    }

    update();
}

/*
 * 
 */
bool OrderBook::cancel(OrderId id){
    if (order_heap_buy.find(id) == -1 && 
        order_heap_sell.find(id) == -1) return false;
    canceled.insert(id);
    update();

    return true;
}

/*
 * Modifies order
 */
bool OrderBook::modify(OrderId, Price newPrice, Quantity newQuantity){
    // Locate it in priority queue, then update quantities + sequence
    
    update();

    return true;
}