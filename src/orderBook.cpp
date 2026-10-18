#include <algorithm>
#include <csetjmp>
#include <cstddef>
#include <optional>
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
    while(order_heap_buy.size() != 0 && order_heap_sell.size() != 0){
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
        return pair{Side::buy, buy_i};

    int sell_i = order_heap_sell.find(id);
    if (sell_i != -1)
        return pair{Side::sell, sell_i};

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
    if (side == Side::buy) {
        order_heap_buy.push(order);
    } else {
        order_heap_sell.push(order);
    }

    update();
}

template<typename Compare>
void OrderBook::submit_market_helper(OrderId id, OrderHeap<Compare>& heap, Quantity quantity) {
    while(heap.size() > 0 && quantity > 0) {
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
    if(side == Side::buy) {
        submit_market_helper(id, order_heap_sell, quantity);
    } else {
        submit_market_helper(id, order_heap_buy, quantity);
    }

    update();
}

/*
 * 
 */
bool OrderBook::cancel(OrderId id) {
    auto location = findId(id);
    if (!location) return false;

    if (location->first == Side::buy) {
        order_heap_buy.erase(location->second);
    } else {
        order_heap_sell.erase(location->second);
    }

    return true;
}

/*
 * Modifies order
 */
bool OrderBook::modify(OrderId id, Price newPrice, Quantity newQuantity){
    // Locate it in priority queue, then update quantities + sequence
    auto side = findId(id);
    if(!side) return false;

    Order newOrder = {
        .id = id,
        .side = side->first,
        .price = newPrice,
        .quantity = newQuantity,
        .sequence = next_sequence_++
    };

    if (side->first == Side::buy){
        order_heap_buy.update(side->second, newOrder);
    } else {
        order_heap_sell.update(side->second, newOrder);
    }
    
    update();

    return true;
}

vector<Order> OrderBook::snapshot(Side side){
    if(side == buy) {
        vector<Order> heap = order_heap_buy.get_heap();
        sort(heap.begin(), heap.end(), BidCompare{});
        return heap;
    }
    // else
    vector<Order> heap = order_heap_sell.get_heap();
    sort(heap.begin(), heap.end(), AskCompare{});
    return heap;
}