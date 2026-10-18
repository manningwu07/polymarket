#include <__format/range_default_formatter.h>
#include <algorithm>
#include <csetjmp>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include "orderBook.hpp"
#include "order.hpp"
#include "orderHeap.hpp"

using namespace std;

/*
 * Updates the data structures when new "tick" happens
 * First checks if anything changed
 * Then executes a simulation tick
 */

Execution OrderBook::update(Order incoming) {
    Execution ex{
        .order = incoming,
        .fills = {}
    };

    // Match incoming against the opposite heap
    auto match = [&](auto& heap) {
        while (incoming.quantity > 0 && heap.size() > 0) {
            Order maker = heap[0];

            // Check whether prices cross (market orders always cross)
            if (incoming.ordertype == OrderType::limit) {
                if (incoming.side == Side::buy &&
                    *incoming.price < *maker.price) {
                    break;
                }

                if (incoming.side == Side::sell &&
                    *incoming.price > *maker.price) {
                    break;
                }
            }

            // Execute trade
            Quantity traded = std::min(
                incoming.quantity,
                maker.quantity
            );

            // Record this trade
            ex.fills.push_back(Fill{
                maker.id,
                incoming.id,
                *maker.price,
                traded
            });

            // Reduce incoming quantity
            incoming.quantity -= traded;

            // Update the resting maker
            if (traded == maker.quantity) {
                heap.pop();
            } else {
                heap.update_quantity(
                    0, maker.quantity - traded
                );
            }
        }
    };

    // Choose the opposite heap
    if (incoming.side == Side::buy) {
        match(order_heap_sell);
    } else {
        match(order_heap_buy);
    }

    // Rest unmatched LIMIT quantity
    if (incoming.ordertype == OrderType::limit &&
        incoming.quantity > 0) {

        if (incoming.side == Side::buy) {
            order_heap_buy.push(incoming);
        } else {
            order_heap_sell.push(incoming);
        }
    }

    // Return residual order and all fills
    ex.order = incoming;
    return ex;
}


optional<pair<Side, size_t>> OrderBook::findId(OrderId id) {
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
Execution OrderBook::submit_limit(OrderId id, Side side, 
    Price price, Quantity quantity) {
    Order order = {
        .id=id,
        .side=side,
        .price=price,
        .quantity=quantity,
        .ordertype=limit,
        .sequence=next_sequence_++,
    };

    logged_ids.insert(id);
    // O(log n)
    if (side == Side::buy) {
        order_heap_buy.push(order);
    } else {
        order_heap_sell.push(order);
    }

    return update(order);
}

/*
 * Create a Buy/Sell at this quantity
 * right now at whatever prices are available
 */
Execution OrderBook::submit_market(OrderId id, Side side, Quantity quantity){
    auto submit_helper = [&](auto& heap){
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
    };
    
    if(side == Side::buy) {
        submit_helper(order_heap_sell);
    } else {
        submit_helper(order_heap_buy);
    }

    return update({
        .id = id,
        .side = side,
        .quantity = quantity,
        .ordertype = market,

        .sequence = next_sequence_++, 
    });
}

/*
 * 
 */
Order OrderBook::cancel(OrderId id) {
    auto location = findId(id);
    if (!location) throw out_of_range("Order ID not found");

    Order order;
    if (location->first == Side::buy) {
        order = order_heap_buy.get(location->second);
        order_heap_buy.erase(location->second);
    } else {
        order = order_heap_sell.get(location->second);
        order_heap_sell.erase(location->second);
    }

    return order;
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