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
            if (ex.order.ordertype == OrderType::limit) {
                if (ex.order.side == Side::buy &&
                    *ex.order.price < *maker.price) {
                    break;
                }

                if (ex.order.side == Side::sell &&
                    *ex.order.price > *maker.price) {
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
                *maker.price,
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


optional<pair<Side, size_t>> OrderBook::findId(const OrderId& id) {
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
Execution OrderBook::submit_limit(const OrderId& id, Side side, 
    Price price, Quantity quantity) {
    if (side != Side::buy && side != Side::sell) {
        throw std::invalid_argument("Invalid side");
    }
    
    if (id.empty() || price <= 0 || quantity <= 0 || logged_ids.contains(id)) {
        throw std::invalid_argument("Invalid limit order");
    }
    
    Order order = {
        .id=id,
        .side=side,
        .price=price,
        .quantity=quantity,
        .ordertype=limit,
        .sequence=next_sequence_++,
    };

    logged_ids.insert(id);
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
    
    if (id.empty() || quantity <= 0 ||logged_ids.contains(id)) {
        throw std::invalid_argument("Invalid market order");
    }

    Order order = {
        .id = id,
        .side = side,
        .quantity = quantity,
        .ordertype = market,

        .sequence = next_sequence_++, 
    };
    logged_ids.insert(id);
    return update(order);
}

/*
 * 
 */
Order OrderBook::cancel(const OrderId& id) {
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

Execution OrderBook::modify(const OrderId& id, Price newPrice, Quantity newQuantity) {
    // Validation
    if (newPrice <= 0 || newQuantity <= 0) {
        throw std::invalid_argument("Invalid price or quantity");
    }

    auto location = findId(id); // Side, index

    if (!location) {
        throw std::out_of_range("Order ID not found");
    }

    // 2 paths, whether modification triggers trade or not
    bool crosses = false;
    
    if (location->first == Side::buy) {
        if (order_heap_sell.size() > 0) {
            crosses = newPrice >= *order_heap_sell[0].price;
        }
    } else {
        if (order_heap_buy.size() > 0) {
            crosses = newPrice <= *order_heap_buy[0].price;
        }
    }

    Order order = {
        .id = id,
        .side = location->first,
        .price = newPrice,
        .quantity = newQuantity,
        .ordertype = limit,
        .sequence = next_sequence_++
    };
    
    // If they don't j modify in place and sift accordingly
    if (!crosses) {
        if(order.side == Side::buy){
            order_heap_buy.update(location->second, order);
        } else {
            order_heap_sell.update(location->second, order);
        }
        return Execution{.order=order, .fills={}};
    } 
    
    // Else, erase and re-insert
    if (order.side == Side::buy) {
        order_heap_buy.erase(location->second);
    } else {
        order_heap_sell.erase(location->second);
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