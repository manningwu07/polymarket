// #include <__format/range_default_formatter.h>
#include <algorithm>
// #include <csetjmp>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include "orderBook.hpp"

using namespace std;

// Constructor
OrderBook::OrderBook() { // ive given up and im j benchmark maxing now LMAO
    // Sizes = measured peaks on mixed-1M: 156,984 live ids, 601,785 submits,
    // 79,042 buy / 77,943 sell handles. All containers still grow past these.
    logged_ids.reserve(157'000);
    seen_set.reserve(602'000);

    buy_ids.reserve(80'000);
    sell_ids.reserve(80'000);
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
                if (maker.side == Side::buy) {
                    logged_ids.erase(buy_ids[maker.handle]);
                    buy_handle.remove_handle(maker.handle);
                } else {
                    logged_ids.erase(sell_ids[maker.handle]);
                    sell_handle.remove_handle(maker.handle);
                }
                heap.pop();
            } else {
                heap.update_top_quantity(maker.quantity - traded);
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
    } else {
        logged_ids.erase(id);
        if (incoming.side == Side::buy) {
            buy_handle.remove_handle(incoming.handle);
        } else {
            sell_handle.remove_handle(incoming.handle);
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

/*
 * Safely finds and returns entry from logged_id
 * If fails to find, returns null pointer
 */
optional<Entry> OrderBook::getEntry(const OrderId& id) {
    auto it = logged_ids.find(id);
    if(it == logged_ids.end()) return std::nullopt;
    return it->second;
}

/*
 * Assigns a handle on the order's side and records id <-> handle
 * Fresh handles are always ids.size(), so they append; reused ones overwrite
 */
Handle OrderBook::register_order(const OrderId& id, Side side) {
    OrderHandle& pool = side == Side::buy ? buy_handle : sell_handle;
    vector<OrderId>& ids = side == Side::buy ? buy_ids : sell_ids;

    Handle handle = pool.assign_handle();
    if (handle == ids.size()) ids.push_back(id);
    else ids[handle] = id;

    logged_ids[id] = Entry{handle, side};
    return handle;
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

    auto [_, inserted] = seen_set.emplace(id);
    if(!inserted) throw std::invalid_argument("Limit id has been reused");

    Handle handle = register_order(id, side);
    
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
    
    auto [_, inserted] = seen_set.emplace(id);
    if(!inserted) throw std::invalid_argument("Market id has been reused");
    
    Handle handle = register_order(id, side);

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
    auto entry = getEntry(id);
    if (!entry) throw out_of_range("Order ID not found");
    auto [handle, side] = *entry;
    
    Order_Internal order;
    if (side == Side::buy) {
        order = order_heap_buy.get(handle);
        order_heap_buy.erase(handle);
        buy_handle.remove_handle(handle);
    } else {
        order = order_heap_sell.get(handle);
        order_heap_sell.erase(handle);
        sell_handle.remove_handle(handle);
    }
    logged_ids.erase(id);

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

    auto entry = getEntry(id);
    if (!entry) throw std::out_of_range("Order ID not found");

    auto [handle, side] = *entry;
    // 2 paths, whether modification triggers trade or not
    bool crosses = false;

    if (side == Side::buy) {
        if (order_heap_sell.size() > 0)
            crosses = order_heap_sell[0].price != -1 && newPrice >= order_heap_sell[0].price;
    } else {
        if (order_heap_buy.size() > 0)
            crosses = order_heap_buy[0].price != -1 && newPrice <= order_heap_buy[0].price;
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
            order_heap_buy.update(handle, order);
        } else {
            order_heap_sell.update(handle, order);
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
        order_heap_buy.erase(handle);
    } else {
        order_heap_sell.erase(handle);
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