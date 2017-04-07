#include <map>
#include <iostream>
#include <cassert>
#include <functional>
#include "veb.h"

template<unsigned int S>
class VEBTree: AbstractVEBTree<S> {
public:
    VEBTree() {
        aux = nullptr;
        min_value = max_value = NO;
    }
    
    ~VEBTree() {
        if (aux != nullptr) {
            for (ull i = aux -> getMin(); i <= aux -> getMax(); i = next(i)) {
                childrens[get_high_half(i)] -> ~VEBTree<HALF>();
            }
            aux -> ~VEBTree<HALF>();
        }
    }
    
    void add(ull x) {
        assert(x < (1ull << S));
        
        if (min_value && max_value == NO) {
            min_value = max_value = x;
            return;
        }
        if (min_value == max_value) {
            if (min_value >= x) {
                min_value = x;
            } else {
                max_value = x;
            }
            return;
        }
        
        if (S == 1) return;
        
        //adding deeper
        if (min_value > x) {
            std::swap(x, min_value);
        }
        if (max_value < x) {
            std::swap(x, max_value);
        }
        
        ull const high = get_high_half(x), low = get_low_half(x);
        if (childrens.find(high) == childrens.end()) {
            childrens[high] = new VEBTree<HALF>();
        }
        childrens[high] -> add(low);
        if (aux == nullptr) {
            aux = new VEBTree<HALF>();
        }
        aux -> add(high);
        
    }
    
    void remove(ull x) {
        if (min_value == max_value) {
            if (x == min_value) {
                if (aux != nullptr) {
                    aux -> remove(get_high_half(x));
                    if (aux -> is_empty()) {
                        aux -> ~VEBTree<HALF>();
                        aux = nullptr;
                    }
                }
                min_value = max_value = NO;
            }
            return;
        }
        ull high, low;
        if (x == min_value) {
            min_value = next(x);
            high = get_high_half(min_value);
            low = get_low_half(min_value);
            if (childrens.find(high) != childrens.end()) {
                childrens[high] -> remove(low);
            }
            //TODO: ?
            return;
        }
        if (x == max_value) {
            max_value = prev(x);
            high = get_high_half(max_value);
            low = get_low_half(max_value);
            if (childrens.find(high) != childrens.end()) {
                childrens[high] -> remove(low);
            }
            //TODO: ?
            return;
        }
        
        high = get_high_half(x), low = get_low_half(x);
        if (childrens.find(high) != childrens.end()) {
            childrens[high] -> remove(low);
        }
        
    }
    
    ull next(ull x) const {
        return abstract_next(x, [] (ull a, ull b) {return a < b;});
    }
    
    ull prev(ull x) const {
        return abstract_next(x, [] (ull a, ull b) {return a > b;});
    }
    
    ull getMin() const {
        return min_value;
    }
    
    ull getMax() const {
        return max_value;
    }
    
    bool is_empty() {
        return min_value == NO;
    }
    
    typedef bool func_t(ull, ull);
    ull abstract_next(ull x, func_t comparator) const {
        ull new_min = (comparator(min_value, max_value)) ? min_value : max_value;
        ull new_max = (new_min == min_value) ? max_value : min_value;
        
        if (x == new_max) {
            return NO;
        }
        if (comparator(x, new_min)) {
            return new_min;
        }
        
        ull const high = get_high_half(x), low = get_low_half(x);
        ull subres = NO;
        if (childrens.find(high) != childrens.end()) {
            subres = childrens.at(high) -> abstract_next(low, comparator);
        }
        if (subres != NO) {
            return concat(high, subres);
        }
        if (aux != nullptr) {
            subres = aux -> abstract_next(high, comparator);
            if (subres != NO) {
                return concat(subres, childrens.at(subres) -> getMin());
            }
        }
        
        return new_max;
    }
    
private:
    
    static const unsigned int HALF = S >> 1;
    VEBTree<HALF> * aux;
    std::map<ull, VEBTree<HALF> *> childrens;
    ull min_value, max_value;
    
    ull get_high_half(ull x) const {
        return x >> HALF;
    }
    
    ull get_low_half(ull x) const {
        return x & ((1ull << HALF) - 1);
    }
    
    ull concat(ull high, ull low) const {
        return (high << HALF) | low;
    }
    
};

int main() {
    VEBTree<20> tree;
    tree.add(5);
    tree.add(11);
    tree.add(10);
    std::cout << tree.next(5) << " " << tree.prev(5) << std::endl;
    tree.remove(10);
    tree.add(7);
    tree.add(3);
    std::cout << tree.next(5) << " " << tree.prev(5) << std::endl;
}
