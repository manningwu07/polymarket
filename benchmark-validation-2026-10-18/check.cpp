#include "orderBook.hpp"
#include "engine.hpp"
#include "trace.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

template<class Compare>
void invariant(OrderHeap<Compare>& heap) {
    if (heap.positions_.size() != heap.size()) {
        throw std::runtime_error("heap/index cardinality mismatch");
    }
    Compare compare;
    for (std::size_t i = 0; i < heap.size(); ++i) {
        const std::unordered_map<OrderId, std::size_t>::const_iterator found =
            heap.positions_.find(heap[i].id);
        if (found == heap.positions_.end() || found->second != i ||
            heap.find(heap[i].id) != static_cast<int>(i)) {
            throw std::runtime_error("heap/index inverse mismatch");
        }
        if (i > 0 && compare(heap[i], heap[(i - 1) / 2])) {
            throw std::runtime_error("heap priority violation at index " + std::to_string(i));
        }
    }
}

template<class Compare>
void compare_side(OrderHeap<Compare>& heap, const std::vector<polymarkets::Order>& expected,
                  const std::unordered_map<std::string, OrderId>& ids) {
    if (heap.size() != expected.size()) {
        throw std::runtime_error("resting order count differs");
    }
    for (const polymarkets::Order& order : expected) {
        const int index = heap.find(ids.at(order.order_id));
        if (index < 0) {
            throw std::runtime_error("missing resting ID " + order.order_id);
        }
        const Order& actual = heap[static_cast<std::size_t>(index)];
        if (actual.price != order.price.value() || actual.quantity != order.quantity) {
            throw std::runtime_error("resting price/quantity differs for " + order.order_id);
        }
        if (actual.sequence != order.sequence) {
            throw std::runtime_error("sequence differs for " + order.order_id +
                ": expected=" + std::to_string(order.sequence) +
                " actual=" + std::to_string(actual.sequence));
        }
    }
}

void replay(const std::string& path) {
    const polymarkets::Trace trace = polymarkets::read_trace(path);
    std::cout << "trace=" << path << " events=" << trace.operations.size()
              << " sha256=" << trace.sha256 << '\n';
    std::unordered_map<std::string, OrderId> ids;
    for (const polymarkets::Operation& operation : trace.operations) {
        if (!ids.contains(operation.order_id)) {
            ids.emplace(operation.order_id, static_cast<OrderId>(ids.size() + 1));
        }
    }
    OrderBook custom;
    polymarkets::OrderBook reference;
    for (std::size_t i = 0; i < trace.operations.size(); ++i) {
        const polymarkets::Operation& operation = trace.operations[i];
        const OrderId id = ids.at(operation.order_id);
        try {
            switch (operation.kind) {
                case polymarkets::Kind::submit_limit: {
                    const polymarkets::Side side = operation.side.value();
                    custom.submit_limit(id, side == polymarkets::Side::buy ? Buy : Sell,
                                        operation.price.value(), operation.quantity.value());
                    reference.submit_limit(operation.order_id, side,
                                           operation.price.value(), operation.quantity.value());
                    break;
                }
                case polymarkets::Kind::submit_market: {
                    const polymarkets::Side side = operation.side.value();
                    custom.submit_market(id, side == polymarkets::Side::buy ? Buy : Sell,
                                         operation.quantity.value());
                    reference.submit_market(operation.order_id, side, operation.quantity.value());
                    break;
                }
                case polymarkets::Kind::cancel:
                    reference.cancel(operation.order_id);
                    if (!custom.cancel(id)) {
                        throw std::runtime_error("custom cancel rejected active ID");
                    }
                    break;
                case polymarkets::Kind::modify:
                    reference.modify(operation.order_id, operation.price.value(), operation.quantity.value());
                    if (!custom.modify(id, operation.price.value(), operation.quantity.value())) {
                        throw std::runtime_error("custom modify rejected active ID");
                    }
                    break;
            }
            invariant(custom.order_heap_buy);
            invariant(custom.order_heap_sell);
            compare_side(custom.order_heap_buy, reference.snapshot(polymarkets::Side::buy), ids);
            compare_side(custom.order_heap_sell, reference.snapshot(polymarkets::Side::sell), ids);
        } catch (const std::exception& error) {
            throw std::runtime_error("event_index=" + std::to_string(i) + " op=" +
                polymarkets::kind_name(operation.kind) + " id=" + operation.order_id +
                " " + error.what());
        }
    }
}

bool cancel_if_active(OrderBook& book, OrderId id) {
    try {
        return book.cancel(id);
    } catch (const std::out_of_range&) {
        return false;
    }
}

template<class Action>
bool rejects_invalid(Action action) {
    try {
        action();
        return false;
    } catch (const std::invalid_argument&) {
        return true;
    }
}

int minimal_cases() {
    int failures = 0;
    for (const Side side : {Buy, Sell}) {
        OrderBook book;
        for (OrderId id = 1; id <= 4; ++id) {
            const Price price = side == Sell ? 99 + static_cast<Price>(id)
                                            : 104 - static_cast<Price>(id);
            book.submit_limit(id, side, price, 1);
        }
        book.submit_market(5, side == Sell ? Buy : Sell, 2);
        const bool second_still_active = cancel_if_active(book, 2);
        const bool third_still_active = cancel_if_active(book, 3);
        std::cout << "public_market_priority side=" << (side == Sell ? "sell" : "buy")
                  << " expected_cancel_2=false actual=" << std::boolalpha << second_still_active
                  << " expected_cancel_3=true actual=" << third_still_active << '\n';
        failures += second_still_active || !third_still_active;
    }
    {
        OrderHeap<AskCompare> heap;
        for (OrderId id = 1; id <= 4; ++id) {
            heap.push(Order{id, Sell, 99 + static_cast<Price>(id), 1, id});
        }
        heap.pop();
        std::cout << "heap_pop expected_top=101 actual=" << heap.top().price
                  << " parent=" << heap[0].price << " left=" << heap[1].price
                  << " right=" << heap[2].price << '\n';
        failures += heap.top().price != 101;
        bool index_ok = heap.positions_.size() == heap.size();
        for (std::size_t i = 0; i < heap.size(); ++i) {
            index_ok = index_ok && heap.find(heap[i].id) == static_cast<int>(i);
        }
        std::cout << "heap_index_after_pop=" << index_ok << '\n';
        failures += !index_ok;
    }
    {
        OrderBook book;
        book.submit_limit(1, Sell, 100, 1);
        book.submit_market(2, Buy, 1);
        book.submit_limit(3, Buy, 90, 1);
        const Sequence actual = book.order_heap_buy[0].sequence;
        std::cout << "market_consumes_sequence expected=3 actual=" << actual << '\n';
        failures += actual != 3;
    }
    {
        OrderBook book;
        book.submit_limit(1, Buy, 90, 1);
        const bool rejected = rejects_invalid([&book]() { book.submit_limit(1, Buy, 89, 1); });
        const std::size_t size = book.order_heap_buy.size();
        const std::size_t index_size = book.order_heap_buy.positions_.size();
        std::cout << "duplicate_id rejected=" << rejected << " actual_heap_size=" << size
                  << " index_size=" << index_size << '\n';
        failures += !rejected || size != 1 || index_size != 1 ||
            book.order_heap_buy[0].price != 90 || book.next_sequence_ != 2;
    }
    {
        OrderBook book;
        book.submit_limit(1, Buy, 90, 1);
        book.cancel(1);
        const bool rejected = rejects_invalid([&book]() { book.submit_limit(1, Buy, 90, 1); });
        std::cout << "retired_id rejected=" << rejected << '\n';
        failures += !rejected || book.order_heap_buy.size() != 0 || book.next_sequence_ != 2;
    }
    {
        OrderBook book;
        const bool rejected = rejects_invalid([&book]() { book.submit_limit(1, Buy, 90, 0); });
        std::cout << "zero_quantity rejected=" << rejected << '\n';
        failures += !rejected || book.order_heap_buy.size() != 0 || book.next_sequence_ != 1;
    }
    std::cout << "failing_cases=" << failures << '\n';
    return failures == 0 ? 0 : 1;
}

int main(int argc, char** argv) {
    try {
        if (argc == 1) {
            return minimal_cases();
        }
        if (argc != 2) {
            throw std::invalid_argument("usage: check [trace.jsonl]");
        }
        replay(argv[1]);
        std::cout << "state_and_invariants=pass fills=unavailable\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL " << error.what() << '\n';
        return 1;
    }
}
