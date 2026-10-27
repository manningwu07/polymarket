#pragma once

#include "order.hpp"
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
  Order get(size_t i);

  // Heap APIs
  Order top();            // returns top
  void pop();             // deletes root
  void push(const Order& order); // insert
  int find(Handle handle);   // returns index of id, pass in hashmap val 
  size_t size();          // size of heap

private:
  vector<Order> heap_;
  vector<int> positions_; // id -> heap idx
  // unordered_map<OrderId, size_t> positions_; 
  Compare higher_priority_;
  
/*
 *  is_new allows sift_up to forcefully add into positions_ in the end
 *  used for push so it doesnt need to redundently add into positions_
 */
  void sift_up(size_t i); 
  void sift_down(size_t i);
};

#include "orderHeap.tpp"
