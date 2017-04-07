#ifndef veb_h
#define veb_h
#define ull unsigned long long

ull const NO = -1;

template<unsigned int S>
class AbstractVEBTree {
public:
    virtual void add(ull x) = 0;
    virtual void remove(ull x) = 0;
    virtual ull next(ull x) const = 0;
    virtual ull prev(ull x) const = 0;
    virtual ull getMin() const = 0;
    virtual ull getMax() const = 0;
};

template<unsigned int S>
class VEBTree;


#endif /* veb_h */
