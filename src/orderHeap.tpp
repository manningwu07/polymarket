template<typename Compare>
void OrderHeap<Compare>::update(size_t i, const Order& newOrder) {
    heap_[i] = newOrder;

    if (i > 0 && higher_priority_(heap_[i], heap_[(i - 1) / 2])) {
        sift_up(i);
    } else {
        sift_down(i);
    }
}

template<typename Compare>
size_t OrderHeap<Compare>::size(){
    return heap_.size();
}

template<typename Compare>
Order OrderHeap<Compare>::top(){
    return heap_[0];
} // returns top

template<typename Compare>
void OrderHeap<Compare>::pop() {
    size_t last = heap_.size() - 1;
    swap_nodes(0, last);
    positions_.erase(heap_[last].id);
    heap_.pop_back();
    
    if(heap_.size() > 0) sift_down(0); // sift down root
}; // deletes the top

template<typename Compare>
void OrderHeap<Compare>::push(Order order){
    heap_.push_back(order);
    positions_[order.id] = heap_.size() - 1;
    
    sift_up(heap_.size() - 1); // sift up new order
} // push element

template<typename Compare>
int OrderHeap<Compare>::find(OrderId id){
    if (positions_.contains(id)){
        return positions_[id];
    }

    return -1;
} // returns index of id in O(1) or -1

template<typename Compare>
void OrderHeap<Compare>::sift_up(size_t i){
    if (i == 0) return;

    size_t parent = (i - 1) / 2;
    if(higher_priority_(heap_[i], heap_[parent])) {
        swap_nodes(i, parent);
        sift_up(parent);
    }
}

template<typename Compare>
void OrderHeap<Compare>::sift_down(size_t i) {
    size_t l = i * 2 + 1;
    size_t r = i * 2 + 2;
    size_t n = heap_.size();

    if (l >= n) return; // No children

    if(r >= n){ // One child
        if(higher_priority_(heap_[i], heap_[l])) swap_nodes(i, l);
    } else { // 2 children
        size_t swap_child_index = higher_priority_(heap_[l], heap_[r]) ? r : l;
        
        if (higher_priority_(heap_[i], heap_[swap_child_index])) {
            swap_nodes(i, swap_child_index);
            sift_down(swap_child_index);
        }
    }
};

template<typename Compare>
void OrderHeap<Compare>::swap_nodes(size_t a, size_t b) {
    std::swap(heap_[a], heap_[b]);
    
    positions_[heap_[a].id] = a;
    positions_[heap_[b].id] = b;
}