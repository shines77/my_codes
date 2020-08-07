
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <memory.h>

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>

#include "stop_watch.h"

using namespace std;

//
// 212. Word Search II
//
// https://leetcode-cn.com/problems/word-search-ii/
//

struct Direction {
    int x, y;
};

static const Direction direction[4] = {
    { -1,  0 }, // Left
    {  1,  0 }, // Right
    {  0,  1 }, // Down
    {  0, -1 }  // Up
};

struct VistedMap {
    char * map_;
    int col_, row_;
    
    VistedMap() : map_(nullptr), col_(0), row_(0) {}
    ~VistedMap() {
        destroy();
    }
    
    int col() const { return col_; }
    int row() const { return row_; }
    int size() const { return (col_ * row_); }
    
    void create(int col, int row) {
        destroy();

        map_ = new char[col * row];
        ::memset(map_, 0, col * row * sizeof(char));
        
        col_ = col;
        row_ = row;
    }
    
    void destroy() {
        if (map_) {
            delete[] map_;
            map_ = nullptr;
        }
    }
    
    bool test(int pos) {
        if (pos < size())
            return (map_[pos] != 0);
        else
            return false;
    }
    
    bool test(int col, int row) {
        int pos = row * col_ + col;
        return test(pos);
    }
    
    void set(int pos) {
        if (pos < size())
            map_[pos] = 1;
    }
    
    void set(int col, int row) {
        int pos = row * col_ + col;
        set(pos);
    }
    
    void clear(int pos) {
        if (pos < size())
            map_[pos] = 0;
    }
    
    void clear(int col, int row) {
        int pos = row * col_ + col;
        clear(pos);
    }
};

struct CharBoard {
    char * map_;
    int col_, row_;
    
    CharBoard() : map_(nullptr), col_(0), row_(0) {}
    ~CharBoard() {
        destroy();
    }
    
    int col() const { return col_; }
    int row() const { return row_; }
    int size() const { return (col_ * row_); }
    
    void create(int col, int row) {
        destroy();
        
        map_ = new char[col * row];
        ::memset(map_, 0, col * row * sizeof(char));
        
        col_ = col;
        row_ = row;
    }
    
    void create(vector<vector<char>> & board) {
        int row = (int)board.size();
        int col = (int)board[0].size();
        
        destroy();
        
        map_ = new char[col * row];
        
        int index = 0;
        for (int y = 0; y < row; y++) {
            for (int x = 0; x < col; x++) {
                map_[index++] = board[y][x];
            }
        }
        
        col_ = col;
        row_ = row;
    }
    
    void destroy() {
        if (map_) {
            delete[] map_;
            map_ = nullptr;
        }
    }
    
    char get(int pos) {
        if (pos < size())
            return map_[pos];
        else
            return 0;
    }
    
    char get(int col, int row) {
        int pos = row * col_ + col;
        return get(pos);
    }
    
    void set(int pos, char c) {
        if (pos < size())
            map_[pos] = c;
    }
    
    void set(int col, int row, char c) {
        int pos = row * col_ + col;
        set(pos, c);
    }
};

struct TrieTree {
    int        isLeaf;
    TrieTree * next[26];

    TrieTree() : isLeaf(0) {
        ::memset((void *)this->next, 0, sizeof(next));
    }

    ~TrieTree() {
        ///*
        for (int i = 0; i < 26; i++) {
            TrieTree * node = this->next[i];
            if (node != nullptr) {
                delete node;
                this->next[i] = nullptr;
            }
        }
        //*/
    }

    bool isExists(unsigned char ch) {
        return (this->next[ch - 'a'] != nullptr);
    }

    void buildTree(const string & word) {
        TrieTree * cur = (TrieTree *)this;
        TrieTree * next;
        for (unsigned char ch : word) {
            next = cur->getOrCreateNextNode(ch);
            cur = next;
        }
        cur->isLeaf = true;
    }

    void buildTree(const vector<string> & words) {
        for (const string & word : words) {
            this->buildTree(word);
        }
    }

    TrieTree * getNextNode(unsigned char ch) const {
        return this->next[ch - 'a'];
    }

    TrieTree * getNextNode(char ch) const {
        unsigned char uc = static_cast<unsigned char>(ch);
        return this->getNextNode(uc);
    }

    TrieTree * createNextNode(unsigned char ch) {
        TrieTree * node = new TrieTree();
        this->next[ch - 'a'] = node;
        return node;
    }

    TrieTree * createNextNode(char ch) {
        unsigned char uc = static_cast<unsigned char>(ch);
        return this->createNextNode(uc);
    }

    TrieTree * getOrCreateNextNode(unsigned char ch) {
        TrieTree * next = getNextNode(ch);
        if (next == nullptr) {
            next = createNextNode(ch);
        }
        return next;
    }

    TrieTree * getOrCreateNextNode(char ch) {
        unsigned char uc = static_cast<unsigned char>(ch);
        return this->getOrCreateNextNode(uc);
    }

    void removeNextNode(unsigned char ch) {
        TrieTree * next = getNextNode(ch);
        delete[] next;
        this->next[ch - 'a'] = nullptr;
    }

    void removeNextNode(char ch) {
        unsigned char uc = static_cast<unsigned char>(ch);
        return this->removeNextNode(uc);
    }

    bool isMatched(const string & text) const  {
        TrieTree * cur = (TrieTree *)this;
        TrieTree * next;
        for (unsigned char ch : text) {
            next = cur->getNextNode(ch);
            if (next == nullptr) {
                return false;
            }
            cur = next;
        }
        return true;
    }

    int getMatchType(const string & text) const {
        TrieTree * cur = (TrieTree *)this;
        TrieTree * next;
        for (unsigned char ch : text) {
            next = cur->getNextNode(ch);
            if (next == nullptr) {
                return -1;
            }
            cur = next;
        }
        return (cur->isLeaf == 0) ? 0 : 1;
    }

    int getMatchType(char ch) const {
        TrieTree * next = this->getNextNode(ch);
        if (next == nullptr) {
            return -1;
        }
        return (next->isLeaf == 0) ? 0 : 1;
    }
};

class Solution {
public:
    Solution() {}
    ~Solution() {}

    /**
     * @param board: A list of lists of character
     * @param words: A list of string
     * @return: A list of string
     */
    int row, col;
    int maxWordLen;
    vector<string> resultList;
    unordered_map<string, bool> wordsMap;
    TrieTree trieTree;

    vector<string> wordSearchII(vector<vector<char>> & board, vector<string> & words) {
        row = (int)board.size();
        col = (int)board[0].size();
        
        //CharBoard charBoard;
        //charBoard.create(board);
        
        VistedMap vistedMap;
        vistedMap.create(col, row);
        
        maxWordLen = 0;
        for (auto it = words.cbegin(); it != words.cend(); ++it) {
            const string & word = *it;
            wordsMap.emplace(word, false);
            if (word.size() > maxWordLen) {
                maxWordLen = (int)word.size();
            }
        }

        trieTree.buildTree(words);
        
        startSearch(board, vistedMap, &trieTree);
        return resultList;
    }

    bool recordAnswer(const string & target) {
        bool success = false;
        auto it = wordsMap.find(target);
        if (it != wordsMap.end()) {
            if (!(it->second)) {
                it->second = true;
                resultList.push_back(target);
                success = true;
            }
        }
        return success;
    }
    
    void searchWords(string & target, int x, int y,
                     vector<vector<char>> & board, VistedMap & visted,
                     TrieTree * curTrie) {       
        int newX, newY;
        for (int dir = 0; dir < 4; dir++) {
            newX = x + direction[dir].x;
            if (newX < 0 || newX >= col)
                continue;
            newY = y + direction[dir].y;
            if (newY < 0 || newY >= row)
                continue;
            
            if (visted.test(newX, newY))
                continue;

            char ch = board[newY][newX];
            TrieTree * nextTrie = curTrie->getNextNode(ch);
            if (nextTrie == nullptr) {
                continue;
            }

            // Do character link
            target.push_back(ch);
            visted.set(newX, newY);

            if (nextTrie->isLeaf != 0) {
                // Get a answer
                if (recordAnswer(target))
                    nextTrie->isLeaf = 0;
            }

            searchWords(target, newX, newY, board, visted, nextTrie);
            
            // Undo character link
            target.pop_back();
            visted.clear(newX, newY);
        }
    }
    
    void startSearch(vector<vector<char>> & board, VistedMap & visted,
                     TrieTree * curTrie) {
        string target;
        for (int y = 0; y < row; y++) {
            for (int x = 0; x < col; x++) {
                // Do character link
                char ch = board[y][x];
                TrieTree * nextTrie = curTrie->getNextNode(ch);
                if (nextTrie == nullptr) {
                    continue;
                }

                target.push_back(ch);
                visted.set(x, y);

                if (nextTrie->isLeaf != 0) {
                    // Get a answer
                    if (recordAnswer(target))
                        nextTrie->isLeaf = 0;
                }

                searchWords(target, x, y, board, visted, nextTrie);

                // Undo character link
                target.pop_back();
                visted.clear(x, y);
            }
        }
    }
};

/***************************

  ["abce","sfcs","adee"]
  ["see","se"]

 ***************************/

void test_solution()
{
    Solution solution;
#if 1
    vector<char> row1 = { 'd', 'o', 'a', 'f' };
    vector<char> row2 = { 'a', 'g', 'a', 'i' };
    vector<char> row3 = { 'd', 'c', 'a', 'n' };
    vector<vector<char>> board;
    board.push_back(row1);
    board.push_back(row2);
    board.push_back(row3);
    vector<string> words = { "dog", "dad", "dgdg", "can", "again" };
#else
    vector<char> row1 = { 'a', 'b', 'c', 'e' };
    vector<char> row2 = { 's', 'f', 'c', 's' };
    vector<char> row3 = { 'a', 'd', 'e', 'e' };
    vector<vector<char>> board;
    board.push_back(row1);
    board.push_back(row2);
    board.push_back(row3);
    vector<string> words = { "see", "se" };
#endif

    jimi::StopWatch sw;
    sw.start();
    vector<string> result = solution.wordSearchII(board, words);
    sw.stop();

    printf("Result = \n\n");

    printf("[ ");
    for (size_t i = 0; i < result.size(); i++) {
        if (i < result.size() - 1)
            printf("\"%s\", ", result[i].c_str());
        else
            printf("\"%s\"", result[i].c_str());
    }
    printf(" ]\n");
    printf("\n");

    printf(" Elapsed time: %8.3f ms\n\n", sw.getElapsedMillisec());
}

//int main(int argn, char * argv[])
//{
//    test_solution();
//    return 0;
//}
