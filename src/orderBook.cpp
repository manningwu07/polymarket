// #include <__format/range_default_formatter.h>
#include <algorithm>
// #include <csetjmp>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include "orderBook.hpp"
#include "order.hpp"
#include "orderHeap.hpp"

using namespace std;

// Constructor
OrderBook::OrderBook() {
    logged_ids.reserve(1 << 20); // 1.05M---ive given up and im j benchmark maxing now LMAO
}

/*
 * Updates the data structures when new "tick" happens
 * First checks if anything changed
 * Then executes a simulation tick
 */

Execution OrderBook::update(const Order& incoming) {
    Execution ex{
        .order = incoming,
        .fills = {}
    };

    // Match incoming against the opposite heap
    auto match = [&](auto& heap) {
        while (ex.order.quantity > 0 && heap.size() > 0) {
            const Order& maker = heap[0];

            // Check whether prices cross (market orders always cross)
            // Don't need to check if price == -1 bc first if blocks that
            if (ex.order.ordertype == OrderType::limit) {
                if (ex.order.side == Side::buy && 
                    ex.order.price < maker.price) {
                    break;
                }

                if (ex.order.side == Side::sell &&
                    ex.order.price > maker.price) {
                    break;
                }
            }

            // Execute trade
            Quantity traded = std::min(
                ex.order.quantity,
                maker.quantity
            );

            // Record this trade
            ex.fills.push_back(Fill{
                ex.order.side == Side::buy ? ex.order.id : maker.id,
                ex.order.side == Side::sell ? ex.order.id : maker.id,
                maker.price,
                traded
            });

            // Reduce ex.order quantity
            ex.order.quantity -= traded;

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
    if (ex.order.side == Side::buy) {
        match(order_heap_sell);
    } else {
        match(order_heap_buy);
    }

    // Rest unmatched LIMIT quantity
    if (ex.order.ordertype == OrderType::limit &&
        ex.order.quantity > 0) {

        if (ex.order.side == Side::buy) {
            order_heap_buy.push(ex.order);
        } else {
            order_heap_sell.push(ex.order);
        }
    }
    return ex;
}


optional<pair<Side, int>> OrderBook::findId(const OrderId& id) {
    // Get the hashmap
    auto it = logged_ids.find(id);
    if(it == logged_ids.end()) return std::nullopt;
    
    if (it->second.side == Side::buy){
        return pair{it->second.side, order_heap_buy.find(it->second.handle)};
    } else {
        return pair{it->second.side, order_heap_sell.find(it->second.handle)};
    }
}

/*
 * Create a Buy/sell at this quantity, but only at this price or better.
 */
Execution OrderBook::submit_limit(const OrderId& id, Side side, 
    Price price, Quantity quantity) {
    if (side != Side::buy && side != Side::sell) {
        throw std::invalid_argument("Invalid side");
    }
    
    if (id.empty() || price <= 0 || quantity <= 0) {
        throw std::invalid_argument("Invalid limit order");
    }

    // Insert logged_ids, update handles
    Handle handle = (side == Side::buy ? next_buy_handle_ : next_sell_handle_);
    auto[_, inserted] = logged_ids.try_emplace(id, Entry{handle, side});
    if(!inserted) throw std::invalid_argument("Invalid limit order");
    side == Side::buy ? next_buy_handle_++ : next_sell_handle_++;
    
    Order order = {
        .id=id,
        .price=price,
        .quantity=quantity,
        .sequence=next_sequence_++,
        .handle = handle,
        .ordertype=limit,
        .side=side,
    };

    
    return update(order);
}

/*
 * Create a Buy/Sell at this quantity
 * right now at whatever prices are available
 */
Execution OrderBook::submit_market(const OrderId& id, Side side, Quantity quantity){
    // Validation
    if (side != Side::buy && side != Side::sell) {
        throw std::invalid_argument("Invalid side");
    }
    
    if (id.empty() || quantity <= 0) {
        throw std::invalid_argument("Invalid market order");
    }
    
    // Insert logged_ids, update handles
    Handle handle = (side == Side::buy ? next_buy_handle_ : next_sell_handle_);
    auto[_, inserted] = logged_ids.try_emplace(id, Entry{handle, side});
    if(!inserted) throw std::invalid_argument("Invalid market order");
    side == Side::buy ? next_buy_handle_++ : next_sell_handle_++;

    Order order = {
        .id = id,
        .price = kNoPrice,
        .quantity = quantity,
        .sequence = next_sequence_++, 
        .handle = handle,
        .ordertype = market,
        .side = side,
        
    };
    
    return update(order);
}

/*
 * 
 */
Order OrderBook::cancel(const OrderId& id) {
    auto location = findId(id);
    if (!location || location->second == -1) throw out_of_range("Order ID not found");
    auto [side, idx] = *location;
    
    Order order;
    if (side == Side::buy) {
        order = order_heap_buy.get(idx);
        order_heap_buy.erase(idx);
    } else {
        order = order_heap_sell.get(idx);
        order_heap_sell.erase(idx);
    }

    return order;
}

/*
 * Modifies order
 */

Execution OrderBook::modify(const OrderId& id, Price newPrice, Quantity newQuantity) {
    // Validation
    if (newPrice <= 0 || newQuantity <= 0)
        throw std::invalid_argument("Invalid price or quantity");

    auto location = findId(id); // Side, index

    if (!location || location->second == -1)
        throw std::out_of_range("Order ID not found");

    auto [side, idx] = *location;
    // 2 paths, whether modification triggers trade or not
    bool crosses = false;

    Handle handle;
    if (side == Side::buy) {
        if (order_heap_sell.size() > 0) {
            crosses = order_heap_sell[0].price != -1 && newPrice >= order_heap_sell[0].price;
        }
        handle = order_heap_buy[idx].handle;
    } else {
        if (order_heap_buy.size() > 0) {
            crosses = order_heap_buy[0].price != -1 && newPrice <= order_heap_buy[0].price;
        }
        handle = order_heap_sell[idx].handle;
    }

    Order order = {
        .id = id,
        .price = newPrice,
        .quantity = newQuantity,
        .sequence = next_sequence_++,
        .handle = handle,
        .ordertype = limit,
        .side = side,
    };
    
    // If they don't j modify in place and sift accordingly
    if (!crosses) {
        if(order.side == Side::buy){
            order_heap_buy.update(idx, order);
        } else {
            order_heap_sell.update(idx, order);
        }
        return Execution{.order=order, .fills={}};
    } 
    
    // Else, erase and re-insert
    if (order.side == Side::buy) {
        order_heap_buy.erase(idx);
    } else {
        order_heap_sell.erase(idx);
    }

    return update(order);
}


vector<Order> OrderBook::snapshot(Side side){
    // Validation
    if (side != Side::buy && side != Side::sell) {
        throw std::invalid_argument("Invalid side");
    }
    
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