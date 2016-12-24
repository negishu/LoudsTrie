#ifndef BITVECTOR_HPP
#define BITVECTOR_HPP

typedef __int8            int08_t;
typedef unsigned __int8  uint08_t;
typedef __int32           int32_t;
typedef unsigned __int32 uint32_t;

#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <cmath>

using namespace std;

class BitVector
{
private:
    static const uint32_t WORD_SIZE = 32;
    uint32_t size;
    vector<uint32_t> bits;
    vector<uint32_t> blocks;
public:
    BitVector();
    const uint32_t getSize() const;
    void clear();
    void save(ostream &os);
    void load(istream &is);
    void push_back(bool bit);
    const uint32_t get(uint32_t position);
    void build();
    const uint32_t rank1(uint32_t position);
    const uint32_t rank0(uint32_t position);
    const uint32_t select1(uint32_t count);
    const uint32_t select0(uint32_t count);
    bool operator!=(const BitVector &bitVector) const;
    bool operator==(const BitVector &bitVector) const;
};
#endif
