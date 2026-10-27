#pragma once

#include "order.hpp"
using namespace std;

struct BidCompare {
  template <class T>
  bool operator()(const T& a, const T& b) const {
    if (a.price != b.price) return a.price > b.price;
    return a.sequence < b.sequence;
  }
};


struct AskCompare {
  template <class T>
  bool operator()(const T& a, const T& b) {
    if (a.price != b.price) return a.price < b.price;
    return a.sequence < b.sequence;
  }
}; 

template <typename Compare> class OrderHeap {
public:
  OrderHeap<Compare>();
  // Direct heap modifications
  const vector<Order_Internal> &get_heap() const { return heap_; }

  void update(size_t i, const Order_Internal &newOrder);

  const Order_Internal &operator[](size_t i) const { return heap_[i]; }

  void update_quantity(size_t i, Quantity quantity) {
    heap_[i].quantity = quantity;
  }

  void erase(size_t i);
  Order_Internal get(size_t i);

  // Heap APIs
  Order_Internal top();            // returns top
  void pop();             // deletes root
  void push(const Order_Internal& order); // insert
  int find(Handle handle);   // returns index of id, pass in hashmap val 
  size_t size();          // size of heap

private:
  vector<Order_Internal> heap_;
  vector<int> positions_; // handle -> heap idx
  Compare higher_priority_;
  
/*
 *  is_new allows sift_up to forcefully add into positions_ in the end
 *  used for push so it doesnt need to redundently add into positions_
 */
  void sift_up(size_t i); 
  void sift_down(size_t i);
};

#include "orderHeap.tpp"
