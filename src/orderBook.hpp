#include <bits/stdc++.h>
#include <unordered_set>
#include "order.hpp"
#include "orderHeap.hpp"

using namespace std;

class OrderBook {
    public:
        void submit_limit(OrderId id, Side side, 
            Price price, Quantity quantity);

        void submit_market(OrderId id, Side side, Quantity quantity);

        bool cancel(OrderId id);

        bool modify(OrderId, Price newPrice, Quantity newQuantity);
    private:
        Sequence next_sequence_ = 0;
        unordered_set<OrderId> canceled;
        // Buy
        OrderHeap<Order> order_queue_buy;
        // Sell
        OrderHeap<Order> order_queue_sell;

        void update();
};