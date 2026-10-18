#include <bits/stdc++.h>
#include <csetjmp>
#include <optional>
#include <unordered_set>
#include "orderBook.hpp"
#include "order.hpp"
#include "orderHeap.hpp"

using namespace std;

/*
 * Updates the data structures when new "tick" happens
 * First checks if anything changed
 * Then executes a simulation tick
 */
void OrderBook::update() {
    while(order_heap_buy.size() != 0 || order_heap_sell.size() != 0){
        Order best_bid = order_heap_buy[0];
        Order best_ask = order_heap_sell[0];

        if (best_bid.price < best_ask.price) break;

        if(best_bid.quantity > best_ask.quantity) {
            order_heap_buy.update_quantity(0, best_bid.quantity - best_ask.quantity);
            order_heap_sell.pop();
        } else if(best_bid.quantity < best_ask.quantity) {
            order_heap_sell.update_quantity(0, best_ask.quantity - best_bid.quantity);
            order_heap_buy.pop();
        } else { // equal, pop both
            order_heap_buy.pop();
            order_heap_sell.pop();
        }
    }
    
    return;
}

optional<pair<Side, int>> OrderBook::findId(OrderId id) {
    int buy_i = order_heap_buy.find(id);
    if (buy_i != -1)
        return pair{Side::Buy, buy_i};

    int sell_i = order_heap_sell.find(id);
    if (sell_i != -1)
        return pair{Side::Sell, sell_i};

    return std::nullopt;
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

        .sequence=next_sequence_++,
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
void OrderBook::submit_market_helper(OrderId id, OrderHeap<Compare>& heap, Quantity quantity) {
    while(heap.size() > 0) {
        Quantity heap_quant = heap[0].quantity;
        if (heap_quant > quantity) {
            heap.update_quantity(0, heap_quant - quantity);
            break;
        } else {
            quantity -= heap_quant;
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
    if (!findId(id)) return false;
    canceled.insert(id);
    update();

    return true;
}

template<typename Compare>
void OrderBook::modify_helper(OrderHeap<Compare> heap, int i, OrderId id, Price newPrice, Quantity newQuantity){
    Order order = heap[i];
    bool reSequence = newPrice > order.price || newQuantity > order.quantity;

    Order newOrder = {
        .id = id,
        .price = newPrice,
        .quantity = newQuantity,
        .sequence = reSequence ? next_sequence_++ : order.sequence
    };

    heap.update(i, newOrder);
}

/*
 * Modifies order
 */
bool OrderBook::modify(OrderId id, Price newPrice, Quantity newQuantity){
    // Locate it in priority queue, then update quantities + sequence
    auto side = findId(id);
    if(!side) return false;

    if (side->first == Side::Buy){
        modify_helper(order_heap_buy, side->second, id, newPrice, newQuantity);
    } else {
        modify_helper(order_heap_sell, side->second, id, newPrice, newQuantity);
    }
    
    update();

    return true;
}

