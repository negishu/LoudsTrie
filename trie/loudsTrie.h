#ifndef LOUDS_HPP
#define LOUDS_HPP

#include <queue>
#include <fstream>
#include <cstring>
#include <iostream>
#include <string>

#include "bitVector.h"
const uint32_t NOTFOUND = -1;

class LoudsTrie
{
public:
    BitVector louds;
    BitVector terminal;
    string labels;
    const uint32_t size() const;
    uint32_t linearSearch(uint32_t index, char c);
    uint32_t lookup(string query);
    void traverse(uint32_t index = 2, string str = "");
    void build(std::vector<std::string>& words);
    void read(const string &filename, const unsigned int nLineCnt = -1);
    void load(const string &filename);
    void save(const string &filename);
    bool operator==(const LoudsTrie &trie) const;
};
class EntryLengthLessThan {
public:
    explicit EntryLengthLessThan(size_t length) : length_(length) {
    }
    bool operator()(const std::string &entry) {
        return entry.length() < length_;
    }
private:
    size_t length_;
};
#endif
