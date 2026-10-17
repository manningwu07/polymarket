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
void update() {
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
        order_queue_buy.push(order);
    } else if (side == Side::Sell) {
        order_queue_sell.push(order);
    }

    update();
}

/*
 * Create a Buy/Sell at this quantity
 * right now at whatever prices are available
 */
void OrderBook::submit_market(OrderId id, Side side, Quantity quantity){
    // Get price -> match, then update?
    // Or we can j directly modify the sell top?
    update();
}

/*
 * 
 */
bool OrderBook::cancel(OrderId id){
    canceled.insert(id);
    update();
}

/*
 * 
 */
bool OrderBook::modify(OrderId, Price newPrice, Quantity newQuantity){
    // Locate it in priority queue
    
    update();
}