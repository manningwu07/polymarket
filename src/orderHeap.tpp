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
    erase(0);
}; // deletes the top

template<typename Compare>
void OrderHeap<Compare>::push(Order order){
    heap_.push_back(order);
    positions_[order.id] = heap_.size() - 1;
    
    sift_up(heap_.size() - 1); // sift up new order
} // push element

template<typename Compare>
int OrderHeap<Compare>::find(const OrderId& id){
    if (positions_.contains(id)){
        return positions_[id];
    }

    return -1;
} // returns index of id in O(1) or -1

template<typename Compare>
Order OrderHeap<Compare>::get(size_t i){
    if(i >= heap_.size()) throw out_of_range("Invalid heap index");
    return heap_[i];
} // returns order at index i

template<typename Compare>
void OrderHeap<Compare>::erase(size_t i) {
    if (i >= heap_.size())
        throw out_of_range("Invalid heap index");

    size_t last = heap_.size() - 1;

    // Swap nodes
    if (i != last){
        std::swap(heap_[i], heap_[last]);
        positions_[heap_[i].id] = i;
    } // Dont need to swap positions of last cuz getting popped anyway

    // Erase nodes
    positions_.erase(heap_[last].id);
    heap_.pop_back();

    if (i >= heap_.size()) return;

    if (i > 0 && higher_priority_(heap_[i], heap_[(i - 1) / 2])) {
        sift_up(i);
    } else {
        sift_down(i);
    }
} // erases at id

// Private functions
template<typename Compare>
void OrderHeap<Compare>::sift_up(size_t i){
    bool changed = false;
    while(true){
        if (i == 0) break;
        
        size_t parent = (i - 1) / 2;
        if(!higher_priority_(heap_[i], heap_[parent])) break;
        
        std::swap(heap_[i], heap_[parent]);
        positions_[heap_[i].id] = i;
        i = parent;
        changed = true;
    }

    if(changed) positions_[heap_[i].id] = i;
}

template<typename Compare>
void OrderHeap<Compare>::sift_down(size_t i) {
    size_t n = heap_.size();

    bool changed = false;
    while(true) {
        size_t l = i * 2 + 1;
        size_t r = i * 2 + 2;
    
        if (l >= n) break; // No children
    
        if(r >= n){ // One child
            if(!higher_priority_(heap_[l], heap_[i])) break;
            
            std::swap(heap_[i], heap_[l]);
            positions_[heap_[i].id] = i;
            i = l;
        } else { // 2 children
            size_t child_idx = higher_priority_(heap_[l], heap_[r]) ? l : r;
            if (!higher_priority_(heap_[child_idx], heap_[i])) break;
            
            std::swap(heap_[i], heap_[child_idx]);
            positions_[heap_[i].id] = i;
            i = child_idx;
        }

        changed = true;
    }

    if(changed) positions_[heap_[i].id] = i;
};