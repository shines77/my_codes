
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <memory.h>

#include <string>
#include <vector>
#include <map>
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

struct StateMap {
    char * map_;
    int col_, row_;
    
    StateMap() : map_(nullptr), col_(0), row_(0) {}
    ~StateMap() {
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

bool string_cmp_desc(const string & s1, const string & s2) {
    return (s1.size() > s2.size());
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
    int size;
    int maxWordLen;
    vector<string> resultList;
    vector<string> wordsList;
    map<string, bool> wordsMap;

    vector<string> wordSearchII(vector<vector<char>> & board, vector<string> & words) {
        row = (int)board.size(); 
        col = (int)board[0].size();
        
        size = col * row;
        
        CharBoard charBoard;
        charBoard.create(board);
        
        StateMap stateMap;
        stateMap.create(col, row);
        
        maxWordLen = 0;
        for (auto it = words.cbegin(); it != words.cend(); ++it) {
            const string & word = *it;
            wordsMap.emplace(word, false);
            if (word.size() > maxWordLen) {
                maxWordLen = (int)word.size();
            }
        }

        wordsList = words;
        sort(wordsList.begin(), wordsList.end(), string_cmp_desc);
        
        int count = startSearch(charBoard, stateMap);
        return resultList;
    }

    bool isMatchDictionary(const string & target, bool & isEqual) {
        bool matched = false;
        for (size_t i = 0; i < wordsList.size(); i++) {
            const string & word = wordsList[i];
            if (target.size() <= word.size()) {
                bool contained = true;
                for (size_t n = 0; n < target.size(); n++) {
                    if (target[n] != word[n]) {
                        contained = false;
                        break;
                    }
                }
                if (contained) {
                    matched = true;
                    if (target.size() == word.size()) {
                        isEqual = true;
                        return true;
                    }
                }
            }
            else break;
        }
        return matched;
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
    
    int searchWords(int step, int maxStep, string & target, int x, int y,
                    CharBoard & board, StateMap & state) {
        int results = 0;
        if (step >= maxStep) {
            return 0;
        }
        
        int newX, newY;
        string saveTarget = target;
        for (int dir = 0; dir < 4; dir++) {
            newX = x + direction[dir].x;
            if (newX < 0 || newX >= col)
                continue;
            newY = y + direction[dir].y;
            if (newY < 0 || newY >= row)
                continue;
            
            if (state.test(newX, newY))
                continue;
            
            // Do character link
            target += board.get(newX, newY);

            bool isEqual = false;
            if (!isMatchDictionary(target, isEqual)) {
                target = saveTarget;
                continue;
            }

            // Get a answer
            if (isEqual) {
                if (recordAnswer(target)) {
                    results++;
                }
            }

            state.set(newX, newY);
            
            int success = searchWords(step + 1, maxStep, target, newX, newY, board, state);
            if (success > 0) {
                results += success;
            }
            
            // Undo character link
            target = saveTarget;
            state.clear(newX, newY);
        }
        return results;
    }
    
    int startSearch(CharBoard & board, StateMap & state) {
        int count = 0;
        string target, saveTarget;
        saveTarget = target;
        for (int y = 0; y < row; y++) {
            for (int x = 0; x < col; x++) {
                // Do character link
                target += board.get(x, y);

                bool isEqual = false;
                if (!isMatchDictionary(target, isEqual)) {
                    target = saveTarget;
                    continue;
                }

                // Get a answer
                if (isEqual) {
                    if (recordAnswer(target)) {
                        count++;
                    }
                }

                state.set(x, y);

                int results = searchWords(1, maxWordLen, target, x, y, board, state);

                // Undo character link
                target = saveTarget;
                state.clear(x, y);

                if (results > 0) {
                    count += results;
                }
            }
        }
        return count;
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
