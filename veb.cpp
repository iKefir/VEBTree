#include <map>
#include <iostream>
#include <cassert>
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
            for (ull i = aux -> getMin(); i <= aux -> getMax(); i = aux -> next(i)) {
                delete childrens[i];
                childrens.erase(i);
            }
            delete aux;
            aux = nullptr;
        }
    }
    
    void add(ull x) {
        if (S < 64 && x >= (1ull << S)) return;
        
        if (is_empty()) {
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
        if (x == min_value || x == max_value) {
            return;
        }
        
        //adding deeper
        if (min_value > x) {
            std::swap(x, min_value);
        }
        if (max_value < x) {
            std::swap(x, max_value);
        }
        
        if (S > 1) {
            ull const high = get_high_half(x), low = get_low_half(x);
            if (aux == nullptr) {
                aux = new VEBTree<S - HALF>();
            }
            if (childrens.find(high) == childrens.end()) {
                childrens[high] = new VEBTree<HALF>();
                aux -> add(high);
            }
            childrens[high] -> add(low);
        }
        
    }
    
    void remove(ull x) { 
        if (min_value == x && max_value == x) {
            min_value = max_value = NO;
            return;
        }
        ull high, low;
        if (x == min_value) {
            if (aux == nullptr) {
                min_value = max_value;
                return;
            }
            min_value = next(x);
            x = min_value;
        }
        else if (x == max_value) {
            if (aux == nullptr) {
                max_value = min_value;
                return;
            }
            max_value = prev(x);
            x = max_value;
        }
        high = get_high_half(x);
        low = get_low_half(x);
        if (childrens.find(high) != childrens.end()) {
            childrens[high] -> remove(low);
            if (childrens[high] -> is_empty()) {
                aux -> remove(high);
                childrens.erase(high);
            }
            if (aux -> is_empty()) {
                delete aux;
                aux = nullptr;
            }
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
    
    bool is_empty() const {
        return min_value == NO;
    }
    
    typedef bool func_t(ull, ull);
    ull abstract_next(ull x, func_t comparator) const {
        ull new_min = (comparator(min_value, max_value)) ? min_value : max_value;
        ull new_max = (new_min == min_value) ? max_value : min_value;
        
        if (comparator(new_max, x) ||  new_max == x || is_empty()) {
            return NO;
        }
        if (comparator(x, new_min)) {
            return new_min;
        }
        if (aux == nullptr) {
            return new_max;
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
                return concat(subres, (new_min < new_max ? childrens.at(subres) -> getMin() : childrens.at(subres) -> getMax()));
            }
        }
        
        return new_max;
    }
    
private:
    
    static const unsigned int HALF = S >> 1;
    VEBTree<S - HALF> * aux;
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
    tree.add(27);
    tree.add(86);
    tree.add(4);
    std::cout << tree.prev(25) << std::endl;
    tree.remove(10);
    tree.add(7);
    tree.add(3);
    std::cout << tree.next(5) << " " << tree.prev(5) << std::endl;

}
