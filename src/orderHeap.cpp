#include <bits/stdc++.h>
#include "order.hpp"
#include "orderHeap.hpp"

using namespace std;

template<typename Compare>
Order OrderHeap<Compare>::top(){
    return heap_[0];
} // returns top

template<typename Compare>
void OrderHeap<Compare>::pop() {
    heap_[0] = heap_[heap_.size() - 1];

    sift_down(0); // sift down root
}; // deletes the top

template<typename Compare>
void OrderHeap<Compare>::insert(Order order){
    
} // inserts element

template<typename Compare>
int OrderHeap<Compare>::find(OrderId id){
    
} // returns index of id

template<typename Compare>
void OrderHeap<Compare>::sift_up(size_t i){
    return;
}

template<typename Compare>
void OrderHeap<Compare>::sift_down(size_t i) {
    size_t l = i * 2 + 1;
    size_t r = i * 2 + 2;
    size_t n = heap_.size();

    // No children
    if (l >= n){
        return;
    } 

    if(r >= n){ // One child
        if(comp_(heap_[i], heap_[l])) {
            swap(i, l);
            sift_down(l);
        }
        return;
    } else { // 2 children
        size_t swap_child_index = comp_(heap_[l], heap_[r]) ? r : l;
        
        if (comp_(heap_[i], heap_[swap_child_index])) {
            swap(i, swap_child_index);
            sift_down(swap_child_index);
        }

        return;
    }
};

template<typename Compare>
void OrderHeap<Compare>::swap_nodes(size_t a, size_t b) {
    swap(heap_[a], heap_[b]);
    
    positions_[heap_[a].id] = b;
    positions_[heap_[b].id] = a;
}