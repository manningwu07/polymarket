#include <bits/stdc++.h>
#include "order.hpp"

using namespace std;

struct BidCompare {
    bool operator()(const Order& a, const Order& b) {
        if (a.price != b.price) {
            return a.price > b.price;
        }

        return a.sequence < b.sequence;
    }  
};

struct AskCompare {
    bool operator()(const Order& a, const Order& b) {
        if (a.price != b.price) {
            return a.price < b.price;
        }

        return a.sequence < b.sequence;
    }
};

template<typename Compare>
class OrderHeap {
    public:
        Order top(); // returns top
        void pop(); // deletes root
        void insert(Order order); // insert
        int find(OrderId id); // returns index of id
        
    private:
        vector<Order> heap_;
        unordered_map<OrderId, size_t> positions_;
        Compare comp_;

        void sift_up(size_t i);
        void sift_down(size_t i);
        void swap_nodes(size_t a, size_t b);
        
};