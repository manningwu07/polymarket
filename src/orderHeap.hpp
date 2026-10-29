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
  // Position based modifications
  const vector<Order_Internal> &get_heap() const { return heap_; }
  const Order_Internal &operator[](size_t i) const { return heap_[i]; }
  
  void update_top_quantity(Quantity quantity) { heap_[0].quantity = quantity;}
  void update(Handle h, const Order_Internal &newOrder);
  void erase(Handle h);
  Order_Internal get(Handle h);

  // Heap APIs
  Order_Internal top();            // returns top
  void pop();             // deletes root
  void push(const Order_Internal& order); // insert
  size_t size();          // size of heap

private:
  vector<Order_Internal> heap_;
  vector<int> positions_; // handle -> heap idx
  Compare higher_priority_;
  
  void sift_up(size_t i); 
  void sift_down(size_t i);
};

#include "orderHeap.tpp"
