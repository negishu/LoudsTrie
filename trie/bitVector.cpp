#include "bitVector.h"

static const uint08_t popCountArray[] =
{
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
};
inline const uint32_t popCount(uint32_t r)
{
    return popCountArray[(r >> 24) & 0xFF] + popCountArray[(r >> 16) & 0xFF] + popCountArray[(r >> 8) & 0xFF] + popCountArray[(r)& 0xFF];
}
inline const uint32_t BitCount1(uint32_t x)
{
    return popCount(x);
}
inline const uint32_t BitCount0(uint32_t x)
{
    return 32 - popCount(x);
}

BitVector::BitVector() : size(0)
{
}
const uint32_t BitVector::getSize() const
{
    return (bits.size() + blocks.size()) * sizeof(uint32_t);
}
void BitVector::clear()
{
    size = 0;
    bits.clear();
    blocks.clear();
}
void BitVector::save(ostream &os)
{
    os.write((const char*)&size, sizeof(uint32_t));
    os.write((const char*)bits.data(), bits.size() * sizeof(uint32_t));
    os.write((const char*)blocks.data(), blocks.size() * sizeof(uint32_t));
}
void BitVector::load(istream &is)
{
    is.read((char*)&size, sizeof(uint32_t));
    uint32_t block_size = (uint32_t)ceil((double)size / WORD_SIZE);
    bits.resize(block_size);
    blocks.resize(block_size + 1);
    is.read((char*)bits.data(), block_size * sizeof(uint32_t));
    is.read((char*)blocks.data(), (block_size + 1) * sizeof(uint32_t));
}
void BitVector::push_back(bool bit)
{
    if (bits.size() * WORD_SIZE == size) {
        bits.push_back(0);
    }
    bits.back() |= bit << size % WORD_SIZE;
    size++;
}
const uint32_t BitVector::get(uint32_t position)
{
    uint32_t index = position / WORD_SIZE;
    uint32_t remainder = position % WORD_SIZE;
    return (bits[index] >> remainder) & 1;
}
void BitVector::build()
{
    blocks.resize(bits.size());
    uint32_t count = 0;
    for (uint32_t i = 0; i < size; ++i) {
        if (i % WORD_SIZE == 0) {
            blocks[i / WORD_SIZE] = count;
        }
        count += get(i);
    }
    blocks.push_back(count);
}
const uint32_t BitVector::rank1(uint32_t position)
{
    uint32_t division = position / WORD_SIZE;
    uint32_t absolute = blocks[division];
    uint32_t remainder = position % WORD_SIZE;

    if (remainder == 0) {
        return absolute;
    }

    uint32_t shifted = bits[division] << (WORD_SIZE - remainder);
    uint32_t relative = popCount(shifted);

    return absolute + relative;
}
const uint32_t BitVector::rank0(uint32_t position)
{
    return position - rank1(position);
}
const uint32_t BitVector::select1(uint32_t count)
{
    if (count == 0) {
        return 0;
    }
    uint32_t index = lower_bound(blocks.begin(), blocks.end(), count) - blocks.begin() - 1;
    uint32_t relative = 0;
    for (uint32_t i = 0; i < WORD_SIZE; ++i) {
        relative += (bits[index] >> i) & 1;
        if (count == blocks[index] + relative) {
            return index * WORD_SIZE + i + 1;
        }
    }
    assert(0);
}
const uint32_t BitVector::select0(uint32_t count)
{
    if (count == 0) {
        return 0;
    }
    uint32_t left = 0;
    uint32_t right = blocks.size();
    while (left < right) {
        uint32_t middle = (left + right) / 2;
        if (middle * WORD_SIZE - blocks[middle] < count) {
            left = middle + 1;
        }
        else {
            right = middle;
        }
    }
    uint32_t index = left - 1;
    uint32_t relative = 0;
    for (uint32_t i = 0; i < WORD_SIZE; ++i) {
        relative += !((bits[index] >> i) & 1);
        if (count == index * WORD_SIZE - blocks[index] + relative) {
            return index * WORD_SIZE + i + 1;
        }
    }
    assert(0);
}
bool BitVector::operator!=(const BitVector &bitVector) const
{
    return !(*this == bitVector);
}
bool BitVector::operator==(const BitVector &bitVector) const
{
    if (size != bitVector.size) {
        return false;
    }
    if (bits.size() != bitVector.bits.size()) {
        return false;
    }
    if (blocks.size() != bitVector.blocks.size()) {
        return false;
    }
    for (size_t i = 0; i < bits.size(); ++i) {
        if (bits.at(i) != bitVector.bits.at(i)) {
            return false;
        }
    }
    for (size_t i = 0; i < blocks.size(); ++i) {
        if (blocks.at(i) != bitVector.blocks.at(i)) {
            return false;
        }
    }
    return true;
}
