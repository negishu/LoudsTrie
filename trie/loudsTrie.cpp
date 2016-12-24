#include <queue>
#include <fstream>
#include <cstring>
#include <iostream>
#include <string>
#include <iterator>

int spliter(std::string& instring, const int c, std::vector<std::string> *output)
{
    std::back_insert_iterator<std::vector<std::string>> it(*output);
    const char *p = instring.data();
    const char *end = instring.size() + p;
    while (p != end) {
        if (*p == c) {
            ++p;
        }
        else {
            const char *start = p;
            while (++p != end && *p != c);
            it++ = std::string(start, p - start);
        }
    }
    return output->size();
}

#include "loudsTrie.h"
const uint32_t LoudsTrie::size() const
{
    return labels.size() + louds.getSize() + terminal.getSize();
}
uint32_t LoudsTrie::linearSearch(uint32_t index, char c)
{
    uint32_t id = louds.rank1(index + 1);
    index = louds.select0(id);    // goto first child
    uint32_t label_index = louds.rank1(index + 1) - 1;

    while (louds.get(index) != 0) {
        char label = labels[label_index];
        if (label == c) {
            return index;
        }
        index = index + 1;    // goto next sibling
        label_index = label_index + 1;
    }
    return NOTFOUND;
}
uint32_t LoudsTrie::lookup(string query)
{
    uint32_t index = 0;
    for (auto c : query) {
        index = linearSearch(index, c);
        if (index == NOTFOUND) {
            return 0;
        }
    }
    uint32_t id = louds.rank1(index + 1);
    if (terminal.get(id - 1)) {
        return terminal.rank1(id);
    }
    return 0;
}
void LoudsTrie::traverse(uint32_t index, string str)
{
    uint32_t id = louds.rank1(index + 1);
    char label = labels.at(id - 2);
    if (terminal.get(id - 1) != 0) {
        cout << str + label << endl;
    }
    uint32_t firstChild = louds.select0(id);
    if (firstChild < louds.getSize() && louds.get(firstChild) != 0) {
        traverse(firstChild, str + label);
    }
    if (index + 1 < louds.getSize() && louds.get(index + 1) != 0) {
        traverse(index + 1, str);
    }
}
inline bool Compare(const std::string& s1, const std::string& s2, const int n)
{
    bool bSame = true;
    int nShortLen = std::min(s1.length(), s2.length());
    const char* ps1 = s1.c_str();
    const char* ps2 = s2.c_str();
    int i = 0;
    while (i < n) {
        if (i == nShortLen) {
            bSame = false;
            break;
        }
        if (*ps1 != *ps2) {
            bSame = false;
            break;
        }
        i++; ps1++; ps2++;
    }
    return bSame;
}
void LoudsTrie::build(std::vector<std::string>& words)
{
    // super root
    louds.push_back(1);
    louds.push_back(0);
    labels.push_back('*');
    terminal.push_back(0);
    for (size_t depth = 0; !words.empty(); ++depth)
    {
        for (size_t i = 0; i < words.size(); ++i)
        {
            const std::string &word = words[i];
            if (word.length() > depth && (i == 0 || Compare(word, words[i - 1], depth + 1) == false)) {
                char cur = (words[i].c_str()[depth]);
                louds.push_back(1);
                labels.push_back(cur);
                if (words[i].length() == depth + 1) {
                    terminal.push_back(1);
                }
                else {
                    terminal.push_back(0);
                }
            }
            if (i == words.size() - 1 || Compare(word, words[i + 1], depth) == false) {
                louds.push_back(0);
            }
        }
        words.erase(remove_if(words.begin(), words.end(), EntryLengthLessThan(depth + 1)), words.end());
    }
    louds.build();
    terminal.build();
}
void LoudsTrie::read(const string &filename, const unsigned int nLineCnt)
{
    ifstream ifs(filename);
    unsigned int nCnt = 0;
    vector<std::string> words;
    string line;
    while (getline(ifs, line)) {
        std::vector<std::string> segs;
        spliter(line, '\t', &segs);
        words.push_back(segs[0]);
        nCnt++;
        if (nCnt == nLineCnt) break;
    }
    std::sort(words.begin(), words.end());
    words.erase(unique(words.begin(), words.end()), words.end());
    build(words);
}
void LoudsTrie::load(const string &filename)
{
    labels.clear();
    louds.clear();
    terminal.clear();
    ifstream ifs(filename, ios::binary);
    louds.load(ifs);
    terminal.load(ifs);
    labels.resize(louds.getSize() - 1);
    ifs.read((char*)labels.data(), louds.getSize() - 1);
    ifs.close();
}
void LoudsTrie::save(const string &filename)
{
    ofstream ofs(filename, ios::binary);
    louds.save(ofs);
    terminal.save(ofs);
    ofs.write(labels.data(), labels.size());
    ofs.close();
}
bool LoudsTrie::operator==(const LoudsTrie &trie) const
{
    if (labels.size() != trie.labels.size()) {
        return false;
    }
    for (size_t i = 0; i < labels.size(); ++i) {
        if (labels.at(i) != trie.labels.at(i)) {
            return false;
        }
    }
    if (louds != trie.louds) {
        return false;
    }
    if (terminal != trie.terminal) {
        return false;
    }
    return true;
}
