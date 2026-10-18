#pragma once

#include "order.hpp"
#include <unordered_map>

using namespace std;

struct BidCompare {
  bool operator()(const Order &a, const Order &b) {
    if (a.price != b.price) {
      return a.price > b.price;
    }

    return a.sequence < b.sequence;
  }
};

struct AskCompare {
  bool operator()(const Order &a, const Order &b) {
    if (a.price != b.price) {
      return a.price < b.price;
    }

    return a.sequence < b.sequence;
  }
};

template <typename Compare> class OrderHeap {
public:
  // Direct heap modifications
  const vector<Order> &get_heap() const { return heap_; }

  void update(size_t i, const Order &newOrder);

  const Order &operator[](size_t i) const { return heap_[i]; }

  void update_quantity(size_t i, Quantity quantity) {
    heap_[i].quantity = quantity;
  }

  void erase(size_t i);

  // Heap APIs
  Order top();            // returns top
  void pop();             // deletes root
  void push(Order order); // insert
  int find(OrderId id);   // returns index of id
  size_t size();          // size of heap

private:
  vector<Order> heap_;
  unordered_map<OrderId, size_t> positions_;
  Compare higher_priority_;

  void sift_up(size_t i);
  void sift_down(size_t i);
  void swap_nodes(size_t a, size_t b);
};

#include "orderHeap.tpp"
