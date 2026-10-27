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
    buy_ids.resize(1<<20);
    sell_ids.resize(1<<20);
}

/*
 * Updates the data structures when new "tick" happens
 * First checks if anything changed
 * Then executes a simulation tick
 */

Execution OrderBook::update(const OrderId& id, Order_Internal incoming) {
    Execution ex{
        .order = {},
        .fills = {}
    };

    // Match incoming against the opposite heap
    auto match = [&](auto& heap) {
        while (incoming.quantity > 0 && heap.size() > 0) {
            const Order_Internal& maker = heap[0];

            // Check whether prices cross (market orders always cross)
            // Don't need to check if price == -1 bc first if blocks that
            if (incoming.ordertype == OrderType::limit) {
                if (incoming.side == Side::buy && 
                    incoming.price < maker.price) {
                    break;
                }

                if (incoming.side == Side::sell &&
                    incoming.price > maker.price) {
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
                incoming.side == Side::buy ? 
                    buy_ids[incoming.handle] : buy_ids[maker.handle],
                incoming.side == Side::sell ? 
                    sell_ids[incoming.handle] : sell_ids[maker.handle],
                maker.price,
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

    ex.order = {
        .id = id,
        .price = incoming.price,
        .quantity = incoming.quantity,
        .sequence = incoming.sequence,
        .ordertype = incoming.ordertype,
        .side = incoming.side
    };
    
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
    // inserts handle -> id in proper side and increments respective handle
    side == Side::buy ? buy_ids[next_buy_handle_++] = id : sell_ids[next_sell_handle_++] = id;
    
    
    Order_Internal order = {
        .price=price,
        .quantity=quantity,
        .sequence=next_sequence_++,
        .handle = handle,
        .ordertype=limit,
        .side=side,
    };

    
    return update(id, order);
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
    side == Side::buy ? buy_ids[next_buy_handle_++] = id : sell_ids[next_sell_handle_++] = id;

    Order_Internal order = {
        .price = kNoPrice,
        .quantity = quantity,
        .sequence = next_sequence_++, 
        .handle = handle,
        .ordertype = market,
        .side = side,
    };
    
    return update(id, order);
}

/*
 * 
 */
Order OrderBook::cancel(const OrderId& id) {
    auto location = findId(id);
    if (!location || location->second == -1) throw out_of_range("Order ID not found");
    auto [side, idx] = *location;
    
    Order_Internal order;
    if (side == Side::buy) {
        order = order_heap_buy.get(idx);
        order_heap_buy.erase(idx);
    } else {
        order = order_heap_sell.get(idx);
        order_heap_sell.erase(idx);
    }
    
    Order res = {
        .id = id,
        .price = order.price,
        .quantity = order.quantity, 
        .sequence = order.sequence,
        .ordertype = order.ordertype,
        .side = order.side,
    };
    

    return res;
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

    Order_Internal order = {
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
        return Execution{.order={
            .id = id,
            .price = newPrice,
            .quantity = newQuantity,
            .sequence = order.sequence,
            .ordertype = limit,
            .side = side
        }, .fills={}};
    } 
    
    // Else, erase and re-insert
    if (order.side == Side::buy) {
        order_heap_buy.erase(idx);
    } else {
        order_heap_sell.erase(idx);
    }

    return update(id, order);
}


vector<Order> OrderBook::snapshot(Side side){
    // Validation
    if (side != Side::buy && side != Side::sell) {
        throw std::invalid_argument("Invalid side");
    }

    auto order_conv_heap = [&](auto& heap, auto& ids, auto Compare) {
        vector<Order> res;
        res.reserve(heap.size());
        for (const Order_Internal& e : heap) {
            res.push_back(Order{
                .id = ids[e.handle],
                .price = e.price,
                .quantity = e.quantity,
                .sequence = e.sequence,
                .ordertype = e.ordertype,
                .side = e.side,
            });
        }
        sort(res.begin(), res.end(), Compare);
        return res;
    };

    
    if (side == Side::buy) return order_conv_heap(order_heap_buy.get_heap(), buy_ids, BidCompare{});
    return order_conv_heap(order_heap_sell.get_heap(), sell_ids, AskCompare{});

}