// #include <__format/range_default_formatter.h>
#include <algorithm>
// #include <csetjmp>
#include <cstddef>
#include <stdexcept>
#include "orderBook.hpp"

using namespace std;

// Constructor
OrderBook::OrderBook() { // ive given up and im j benchmark maxing now LMAO
    // Sizes = measured peaks on mixed-1M: 156,984 live ids, 601,785 submits,
    // 79,042 buy / 77,943 sell handles. All containers still grow past these.
    logged_ids.reserve(157'000);
    seen_set.reserve(602'000);

    bids.level_ids.reserve(80'000);
    asks.level_ids.reserve(80'000);
}

// Handles are dense and reused, so a fresh one is always ids.size()
static void store_id(vector<OrderId>& ids, Handle handle, const OrderId& id) {
    if (handle == ids.size()) ids.push_back(id);
    else ids[handle] = id;
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

    auto match = [&](auto& half, auto better) {
        while (incoming.quantity > 0) {
            const bool has_level = !half.levels.empty();
            const bool has_heap = half.heap.size() > 0;
            if (!has_level && !has_heap) break;

            const bool from_heap = has_heap &&
                (!has_level || better(half.heap[0].price, half.levels.getBestPrice()));
            BookSideOrder maker;
            if (from_heap) {
                const Order_Internal& top = half.heap[0];
                maker = {top.price, top.quantity, top.sequence, top.handle};
            } else {
                maker = half.levels.top(half.levels.getBestPrice());
            }

            // Limit stops once the maker is strictly better than it; market always crosses
            if (incoming.ordertype == OrderType::limit && better(incoming.price, maker.price)) break;
            if (ex.fills.empty()) ex.fills.reserve(4); // One time allocation for the ex fills. Cuts ~40% of allocations based off profile

            // Execute trade
            Quantity traded = std::min(incoming.quantity, maker.quantity);
            const OrderId& maker_id = from_heap ? half.heap_ids[maker.handle] : half.level_ids[maker.handle];

            // Record this trade
            ex.fills.push_back(incoming.side == Side::buy
                ? Fill{id, maker_id, maker.price, traded}
                : Fill{maker_id, id, maker.price, traded});

            // Reduce incoming quantity
            incoming.quantity -= traded;

            // Update the resting maker
            if (traded == maker.quantity) {
                logged_ids.erase(maker_id);
                if (from_heap) {
                    half.heap_handles.remove_handle(maker.handle);
                    half.heap.pop();
                } else {
                    half.levels.pop(maker.price);
                }
            } else if (from_heap) {
                half.heap.update_top_quantity(maker.quantity - traded);
            } else {
                half.levels.set_quantity(maker.handle, maker.quantity - traded);
            }
        }
    };

    // Choose the opposite side for comparison
    if (incoming.side == Side::buy) {
        match(asks, [](Price a, Price b) { return a < b; });
    } else {
        match(bids, [](Price a, Price b) { return a > b; });
    }

    // Rest unmatched LIMIT quantity
    if (incoming.ordertype == OrderType::limit &&
        incoming.quantity > 0) {

        if (incoming.side == Side::buy) {
            rest(bids, id, incoming);
        } else {
            rest(asks, id, incoming);
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
 * Rests a limit order: levels if BookSide covers its price, heap otherwise
 */
template <class Compare>
void OrderBook::rest(HalfBook<Compare>& half, const OrderId& id, const Order_Internal& order) {
    const bool in_heap = !BookSide::covers(order.price);
    Handle handle;
    if (in_heap) {
        handle = half.heap_handles.assign_handle();
        Order_Internal resting = order;
        resting.handle = handle;
        half.heap.push(resting);
        store_id(half.heap_ids, handle, id);
    } else {
        handle = half.levels.push(order.price, order.quantity, order.sequence);
        store_id(half.level_ids, handle, id);
    }
    logged_ids[id] = Entry{handle, order.side, in_heap};
}

BookSideOrder OrderBook::remove(const Entry& entry) {
    auto take = [&](auto& half) -> BookSideOrder {
        if (entry.in_heap) {
            Order_Internal order = half.heap.get(entry.handle);
            half.heap.erase(entry.handle);
            half.heap_handles.remove_handle(entry.handle);
            return {order.price, order.quantity, order.sequence, entry.handle};
        }
        BookSideOrder order = half.levels.get(entry.handle);
        half.levels.erase(entry.handle);
        return order;
    };
    
    return entry.side == Side::buy ? take(bids) : take(asks);
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

    Order_Internal order = {
        .price=price,
        .quantity=quantity,
        .sequence=next_sequence_++,
        .handle = 0, // assigned when it rests
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
    
    Order_Internal order = {
        .price = kNoPrice,
        .quantity = quantity,
        .sequence = next_sequence_++, 
        .handle = 0, // markets never rest
        .ordertype = market,
        .side = side,
    };
    
    return update(id, order);
}

/*
 * 
 */
Order OrderBook::cancel(const OrderId& id) {
    auto it = logged_ids.find(id);
    if(it == logged_ids.end()) throw out_of_range("Order ID not found");

    BookSideOrder order = remove(it->second);
    
    Order newOrder = {
        .id = id,
        .price = order.price,
        .quantity = order.quantity,
        .sequence = order.sequence,
        .ordertype = limit,
        .side = it->second.side,
    };

    logged_ids.erase(it);
    return newOrder;
}

/*
 * Modifies order
 */

Execution OrderBook::modify(const OrderId& id, Price newPrice, Quantity newQuantity) {
    // Validation
    if (newPrice <= 0 || newQuantity <= 0)
        throw std::invalid_argument("Invalid price or quantity");

    auto it = logged_ids.find(id);
    if(it == logged_ids.end()) throw std::out_of_range("Order ID not found");

    // Pull order out, then treat it as a new limit.
    // If doesn't cross, update match loop exits
    remove(it->second);

    Order_Internal order = {
        .price = newPrice,
        .quantity = newQuantity,
        .sequence = next_sequence_++,
        .handle = 0, // assigned when it rests
        .ordertype = limit,
        .side = it->second.side,
    };

    Execution execution = update(id, order);
    if(execution.order.quantity == 0) logged_ids.erase(id);
    
    return execution;
}


vector<Order> OrderBook::snapshot(Side side){
    // Validation
    if (side != Side::buy && side != Side::sell) {
        throw std::invalid_argument("Invalid side");
    }

    auto collect = [&](auto& half, auto compare) {
        vector<Order> res;
        auto add = [&](const OrderId& id, Price price, Quantity quantity, Sequence sequence) {
            res.push_back(Order{
                .id = id,
                .price = price,
                .quantity = quantity,
                .sequence = sequence,
                .ordertype = limit,
                .side = side,
            });
        };
        for (const BookSideOrder& o : half.levels.orders()) {
            add(half.level_ids[o.handle], o.price, o.quantity, o.sequence);
        }
        for (const Order_Internal& e : half.heap.get_heap()) {
            add(half.heap_ids[e.handle], e.price, e.quantity, e.sequence);
        }
        sort(res.begin(), res.end(), compare);
        return res;
    };

    if (side == Side::buy) return collect(bids, BidCompare{});
    return collect(asks, AskCompare{});
}
