
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <bitset>

#include "StopWatch.h"
#include "CPUWarmUp.h"

#define MATRIX_USE_STD_BITSET   1
#define MATRIX_USE_BITMAP       0

#define DIFFICULTY_LEVEL        2

static const char test_board[3][81] = {
    // Normal
    '5', '3', '.',  '.', '7', '.',  '.', '.', '.',
    '6', '.', '.',  '1', '9', '5',  '.', '.', '.',
    '.', '9', '8',  '.', '.', '.',  '.', '6', '.',

    '8', '.', '.',  '.', '6', '.',  '.', '.', '3',
    '4', '.', '.',  '8', '.', '3',  '.', '.', '1',
    '7', '.', '.',  '.', '2', '.',  '.', '.', '6',

    '.', '6', '.',  '.', '.', '.',  '2', '8', '.',
    '.', '.', '.',  '4', '1', '9',  '.', '.', '5',
    '.', '.', '.',  '.', '8', '.',  '.', '7', '9',

    //
    // Advance
    // https://www.sudoku-cn.com/
    //
    '4', '.', '2',  '.', '.', '.',  '9', '.', '.',
    '.', '.', '.',  '.', '6', '1',  '.', '.', '.',
    '.', '1', '9',  '.', '.', '.',  '.', '.', '.',

    '7', '.', '5',  '.', '.', '.',  '6', '.', '.',
    '2', '.', '4',  '7', '.', '.',  '.', '.', '5',
    '.', '.', '.',  '.', '9', '.',  '7', '.', '.',

    '.', '8', '.',  '2', '.', '9',  '.', '1', '.',
    '.', '.', '7',  '.', '.', '4',  '.', '.', '.',
    '.', '.', '.',  '.', '.', '.',  '.', '5', '2',

    //
    // Hard
    // http://www.cn.sudokupuzzle.org/play.php
    //
    '5', '.', '.',  '.', '.', '.',  '.', '.', '.',
    '.', '1', '.',  '.', '.', '.',  '3', '2', '.',
    '.', '.', '.',  '8', '4', '.',  '.', '.', '.',

    '.', '.', '.',  '.', '.', '.',  '.', '.', '.',
    '.', '.', '.',  '.', '.', '3',  '1', '.', '.',
    '6', '.', '8',  '5', '.', '.',  '.', '.', '.',

    '.', '.', '7',  '.', '.', '.',  '.', '6', '8',
    '.', '3', '4',  '.', '.', '1',  '.', '.', '.',
    '.', '.', '.',  '.', '.', '.',  '.', '.', '.'
};

namespace LeetCode {
namespace Problem_0037 {
namespace v1 {

template <size_t Bits>
class BitMap {
public:
    static const size_t kAlignmentBits = sizeof(size_t) * 8;
    static const size_t kBytes = ((Bits + kAlignmentBits - 1) / kAlignmentBits) * kAlignmentBits;

private:
    size_t * bits_;

public:
    BitMap() : bits_(nullptr) {
        bits_ = new size_t[kBytes];
    }
    ~BitMap() {
        if (bits_ != nullptr) {
            delete[] bits_;
            bits_ = nullptr;
        }
    }

    size_t bits() const  { return Bits; }
    size_t bytes() const { return kBytes; }
    size_t size() const  { return Bits; }
    void * data() const  { return (void *)bits_; }

    void clear() {
        if (bits_) {
            ::memset(bits_, 0, kBytes * sizeof(size_t));
        }
    }

    bool test(size_t pos) const {
        assert(pos < Bits);
        size_t index = pos / kAlignmentBits;
        size_t shift = pos % kAlignmentBits;
        return ((bits_[index] & (1ULL << shift)) != 0);
    }

    size_t value(size_t pos) const {
        assert(pos < Bits);
        size_t index = pos / kAlignmentBits;
        size_t shift = pos % kAlignmentBits;
        return (bits_[index] & (1ULL << shift));
    }

    void set(size_t pos) {
        assert(pos < Bits);
        size_t index = pos / kAlignmentBits;
        size_t shift = pos % kAlignmentBits;
        bits_[index] |= (1ULL << shift);
    }

    void reset(size_t pos) {
        assert(pos < Bits);
        size_t index = pos / kAlignmentBits;
        size_t shift = pos % kAlignmentBits;
        bits_[index] &= ~(1ULL << shift);
    }
};

template <size_t Bits>
class BitSet {
public:
    static const size_t kAlignmentBytes = sizeof(size_t);
    static const size_t kBytes = ((Bits + kAlignmentBytes - 1) / kAlignmentBytes) * kAlignmentBytes;

private:
    uint8_t * bytes_;

public:
    BitSet() : bytes_(nullptr) {
        bytes_ = new uint8_t[kBytes];
    }
    ~BitSet() {
        if (bytes_ != nullptr) {
            delete[] bytes_;
            bytes_ = nullptr;
        }
    }

    size_t bits() const  { return Bits; }
    size_t bytes() const { return kBytes; }
    size_t size() const  { return Bits; }
    void * data() const  { return (void *)bytes_; }

    void clear() {
        if (bytes_) {
            ::memset(bytes_, 0, kBytes * sizeof(uint8_t));
        }
    }

    bool test(size_t pos) const {
        assert(pos < Bits);
        return (bytes_[pos] != 0);
    }

    uint8_t value(size_t pos) const {
        assert(pos < Bits);
        return bytes_[pos];
    }

    void set(size_t pos) {
        assert(pos < Bits);
        bytes_[pos] = 1;
    }

    void reset(size_t pos) {
        assert(pos < Bits);
        bytes_[pos] = 0;
    }

    uint8_t & operator [] (size_t pos) {
        assert(pos < Bits);
        return bytes_[pos];
    }

    uint8_t operator [] (size_t pos) const {
        assert(pos < Bits);
        return bytes_[pos];
    }
};

template <size_t Rows, size_t Cols>
class BitMartix {
private:
#if MATRIX_USE_STD_BITSET
    typedef std::bitset<Cols>    bitmap_type;
#elif MATRIX_USE_BITMAP
    typedef BitMap<Cols>    bitmap_type;
#else
    typedef BitSet<Cols>    bitmap_type;
#endif
    bitmap_type data_[Rows];
    size_t rows_;

public:
    BitMartix() : rows_(Rows) {}
    ~BitMartix() {}

    size_t rows() const { return rows_; }
    size_t cols() const { return Cols; }

    size_t size() const { return Rows; }
    size_t total_size() const { return (Rows * Cols); }

    void setRows(size_t rows) {
        rows_ = rows;
    }

    void clear() {
#if (MATRIX_USE_STD_BITSET == 0)
        for (size_t row = 0; row < Rows; row++) {
            data_[row].clear();
        }
#endif
    }

    bool test(size_t row, size_t col) {
        assert(pos < Rows);
        return data_[pos].test(col);
    }

    size_t value(size_t row, size_t col) {
        assert(pos < Rows);
        return data_[pos].value(col);
    }

    bitmap_type & operator [] (size_t pos) {
        assert(pos < Rows);
        return data_[pos];
    }

    const bitmap_type & operator [] (size_t pos) const {
        assert(pos < Rows);
        return data_[pos];
    }
};

class DancingLinks;

class SudokuSolver {
public:
    static const size_t Rows = 9;
    static const size_t Cols = 9;
    static const size_t Numbers = 9;

    static const size_t BoardSize = Rows * Cols * Numbers;

    typedef BitMartix<BoardSize, Rows * Cols * 4> matrix_type;

private:
    matrix_type matrix;
    size_t rows[BoardSize + 1];
    size_t cols[BoardSize + 1];
    size_t numbers[BoardSize + 1];

public:
    SudokuSolver(const std::vector<std::vector<char>> & board) {
        this->init(board);
    }
    ~SudokuSolver() {}

    matrix_type & getDlkMartix() {
        return matrix;
    }

    const matrix_type & getDlkMartix() const {
        return matrix;
    }

private:
    void init(const std::vector<std::vector<char>> & board) {
        size_t empties = 0;
        for (size_t row = 0; row < Rows; row++) {
            for (size_t col = 0; col < Cols; col++) {
                if (board[row][col] == '.') {
                    empties++;
                }
            }
        }

        matrix.setRows(empties * 8 + 9 * 9);

        size_t index = 0;
        matrix.clear();
        for (size_t row = 0; row < Rows; row++) {
            for (size_t col = 0; col < Cols; col++) {
                size_t m = 0, n = Numbers - 1;
                if (board[row][col] != '.') {
                    m = n = board[row][col] - '1';
                }
                for (size_t number = m; number <= n; number++) {
#if MATRIX_USE_BITMAP
                    matrix[index].set(81 * 0 + row * 9 + col);
                    matrix[index].set(81 * 1 + row * 9 + number);
                    matrix[index].set(81 * 2 + col * 9 + number);
                    matrix[index].set(81 * 3 + (row / 3 * 3 + col / 3) * 9 + number);
#else
                    matrix[index][81 * 0 + row * 9 + col] = 1;
                    matrix[index][81 * 1 + row * 9 + number] = 1;
                    matrix[index][81 * 2 + col * 9 + number] = 1;
                    matrix[index][81 * 3 + (row / 3 * 3 + col / 3) * 9 + number] = 1;
#endif
                    rows[index + 1] = row;
                    cols[index + 1] = col;
                    numbers[index + 1] = number;
                    index++;
                }
            }
        }
    }

public:
    void output_answer(std::vector<std::vector<char>> & board,
                       const DancingLinks * dancingLinks);

    static void display_board(const std::vector<std::vector<char>> & board, bool is_input = false) {
        int filled = 0;
        for (size_t row = 0; row < Rows; row++) {
            for (size_t col = 0; col < Cols; col++) {
                if (board[row][col] != '.')
                    filled++;
            }
        }

        if (is_input) {
            //printf("\n");
            printf("The input is: (filled = %d)\n", filled);
        }
        else {
            printf("The answer is:\n");
        }
        printf("\n");
        for (size_t row = 0; row < Rows; row++) {
            printf("  ");
            for (size_t col = 0; col < Cols; col++) {
                char num = board[row][col];
                if (num == '.')
                    num = '0';
                if (col < Cols - 1)
                    printf("%c, ", num);
                else
                    printf("%c", num);
            }
            printf("\n");
        }
        printf("\n");
    }
};

class DancingLinks {
private:
    struct Node {
        int prev, next;
        int up, down;
        int row, col;
    };
    
    std::vector<Node>   link_list;
    std::vector<int>    col_size;
    int                 free_idx;
    std::vector<int>    answer;
    std::vector<std::vector<int>>
                        answers;

    struct StackInfo {
        int col;
        int i;

        StackInfo() : col(0), i(0) {}
        StackInfo(int col, int i) : col(col), i(i) {}

        void set(int col, int i) {
            this->col = col;
            this->i = i;
        }
    };

    enum StackState {
        SearchNext,
        BackTracking,
        BackTrackingRetry,
        Last
    };

public:
    DancingLinks(typename SudokuSolver::matrix_type & matrix, size_t nodes)
        : link_list(nodes), col_size(matrix.cols() + 1), free_idx((int)matrix.cols() + 1) {
        this->init(matrix);
    }

    ~DancingLinks() {}

    bool is_empty() const { return (link_list[0].next == 0); }

    const std::vector<int> & get_answer() const { return this->answer; }

private:
    void init(const typename SudokuSolver::matrix_type & matrix) {
        int cols = (int)matrix.cols();
        for (int col = 0; col <= cols; col++) {
            link_list[col].prev = col - 1;
            link_list[col].next = col + 1;
            link_list[col].up = col;
            link_list[col].down = col;
        }
        link_list[0].prev = cols;
        link_list[cols].next = 0;

        init_from_martix(matrix);
    }

    void init_from_martix(const typename SudokuSolver::matrix_type & matrix) {
        int rows = (int)matrix.rows();
        int cols = (int)matrix.cols();
        for (int row = rows - 1; row >= 0; row--) {
            int head = free_idx, tail = free_idx;
            for (int col = 0; col < cols; col++) {
                if (matrix[row].test(col)) {
                    tail = this->insert(head, tail, row + 1, col + 1);
                }
            }
        }
    }

public:
    int insert(int head, int tail, int row, int col) {
        link_list[free_idx].prev = tail;
        link_list[free_idx].next = head;
        link_list[free_idx].up = col;
        link_list[free_idx].down = link_list[col].down;
        link_list[free_idx].row = row;
        link_list[free_idx].col = col;

#if 0
        link_list[link_list[free_idx].prev].next = free_idx;
        link_list[link_list[free_idx].next].prev = free_idx;
        link_list[link_list[free_idx].up].down = free_idx;
        link_list[link_list[free_idx].down].up = free_idx;
#else
        link_list[tail].next = free_idx;
        link_list[head].prev = free_idx;
        link_list[col].down = free_idx;
        link_list[link_list[free_idx].down].up = free_idx;
#endif
        col_size[col]++;
        tail = free_idx++;
        return tail;
    }

    void erase(int col) {
        link_list[link_list[col].prev].next = link_list[col].next;
        link_list[link_list[col].next].prev = link_list[col].prev;

        for (int i = link_list[col].down; i != col; i = link_list[i].down) {
            for (int j = link_list[i].next; j != i; j = link_list[j].next) {
                link_list[link_list[j].up].down = link_list[j].down;
                link_list[link_list[j].down].up = link_list[j].up;
                col_size[link_list[j].col]--;
            }
        }
    }

    void recover(int col) {
        for (int i = link_list[col].up; i != col; i = link_list[i].up) {
            for (int j = link_list[i].prev; j != i; j = link_list[j].prev) {
                link_list[link_list[j].down].up = j;
                link_list[link_list[j].up].down = j;
                col_size[link_list[j].col]++;
            }
        }

        link_list[link_list[col].next].prev = col;
        link_list[link_list[col].prev].next = col;
    }

    int get_min_column() const {
        int min_col = link_list[0].next;
        for (int i = link_list[0].next; i != 0 ; i = link_list[i].next) {
            if (col_size[i] < col_size[min_col])
                min_col = i;
        }
        return min_col;
    }

    bool solve() {
        if (this->is_empty()) {
            return true;
        }
        
        int col = get_min_column();
        this->erase(col);
        for (int i = link_list[col].down; i != col; i = link_list[i].down) {
            answer.push_back(link_list[i].row);
            for (int j = link_list[i].next; j != i; j = link_list[j].next) {
                this->erase(link_list[j].col);
            }

            if (solve()) {
                return true;
            }

            for (int j = link_list[i].prev; j != i; j = link_list[j].prev) {
                this->recover(link_list[j].col);
            }
            answer.pop_back();
        }
        this->recover(col);
        return false;
    }

    bool solve_non_recursive() {
        int state = StackState::SearchNext;
        std::vector<StackInfo> stack;
        StackInfo stack_info;
        int i, col;

        do {       
            if (state == StackState::SearchNext) {
Search_Next:
                if (this->is_empty()) {
                    return true;
                }

                col = get_min_column();
                this->erase(col);

                i = link_list[col].down;

                while (i != col) {
                    stack_info.set(col, i);
                    stack.push_back(stack_info);
                    answer.push_back(link_list[i].row);

                    for (int j = link_list[i].next; j != i; j = link_list[j].next) {
                        this->erase(link_list[j].col);
                    }

                    // SearchNext
                    goto Search_Next;
                }
            }
            else if (state == StackState::BackTrackingRetry) {
BackTracking_Retry:
                while (i != col) {
                    stack_info.set(col, i);
                    stack.push_back(stack_info);
                    answer.push_back(link_list[i].row);
                    for (int j = link_list[i].next; j != i; j = link_list[j].next) {
                        this->erase(link_list[j].col);
                    }

                    state = StackState::SearchNext;
                    goto Search_Next;
                }
            }
            else {
                // StackState::BackTracking
BackTracking_Entry:
                answer.pop_back();
                stack_info = stack.back();
                stack.pop_back();
                i = stack_info.i;
                col = stack_info.col;

                for (int j = link_list[i].prev; j != i; j = link_list[j].prev) {
                    this->recover(link_list[j].col);
                }

                i = link_list[i].down;

                state = StackState::BackTrackingRetry;
                goto BackTracking_Retry;
            }

            this->recover(col);

            if (stack.size() != 0) {
                state = StackState::BackTracking;
                goto BackTracking_Entry;
            }
            else break;
        } while (1);

        return false;
    }
};

void SudokuSolver::output_answer(std::vector<std::vector<char>> & board,
                                 const DancingLinks * dancingLinks) {
    for (auto idx : dancingLinks->get_answer()) {
        board[rows[idx]][cols[idx]] = (char)numbers[idx] + '1';
    }
}

class Solution {
public:
    void solveSudoku(std::vector<std::vector<char>> & board) {
        SudokuSolver::display_board(board, true);

        jtest::StopWatch sw;
        sw.start();

        SudokuSolver solver(board);

        DancingLinks dancingLinks(solver.getDlkMartix(), SudokuSolver::BoardSize * 4 + 1);
        dancingLinks.solve_non_recursive();
        //dancingLinks.solve();

        sw.stop();

        solver.output_answer(board, &dancingLinks);
        SudokuSolver::display_board(board);
        printf("Elapsed time: %0.3f ms\n\n", sw.getElapsedMillisec());
    }
};

} // namespace v1
} // namespace Problem_0037
} // namespace LeetCode

namespace LeetCode {
namespace Problem_0037 {
namespace v2 {

class Solution {
public:
    std::bitset<9> getPossibleStatus(int x, int y)
    {
        return ~(rows[x] | cols[y] | cells[x / 3][y / 3]);
    }

    std::vector<int> getNext(std::vector<std::vector<char>> & board)
    {
        std::vector<int> ret;
        int minCnt = 10;
        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board[i].size(); j++) {
                if (board[i][j] != '.')
                    continue;
                auto cur = getPossibleStatus(i, j);
                if (cur.count() >= minCnt)
                    continue;
                ret = { i, j };
                minCnt = (int)cur.count();
            }
        }
        return ret;
    }

    void fillNum(int x, int y, int n, bool fillFlag)
    {
        rows[x][n] = (fillFlag) ? 1 : 0;
        cols[y][n] = (fillFlag) ? 1 : 0;
        cells[x/3][y/3][n] = (fillFlag) ? 1: 0;
    }
    
    bool dfs(std::vector<std::vector<char>> & board, int cnt)
    {
        if (cnt == 0)
            return true;

        auto next = getNext(board);
        auto bits = getPossibleStatus(next[0], next[1]);
        for (int n = 0; n < bits.size(); n++) {
            if (!bits.test(n))
                continue;
            fillNum(next[0], next[1], n, true);
            board[next[0]][next[1]] = n + '1';
            if (dfs(board, cnt - 1))
                return true;
            board[next[0]][next[1]] = '.';
            fillNum(next[0], next[1], n, false);
        }
        return false;
    }

    void solveSudoku(std::vector<std::vector<char>> & board) 
    {
        v1::SudokuSolver::display_board(board, true);

        jtest::StopWatch sw;
        sw.start();

        rows = std::vector<std::bitset<9>>(9, std::bitset<9>());
        cols = std::vector<std::bitset<9>>(9, std::bitset<9>());
        cells = std::vector<std::vector<std::bitset<9>>>(3, std::vector<std::bitset<9>>(3, std::bitset<9>()));

        int cnt = 0;
        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board[i].size(); j++) {
                cnt += (board[i][j] == '.');
                if (board[i][j] == '.')
                    continue;
                int n = board[i][j] - '1';
                rows[i] |= (1ULL << n);
                cols[j] |= (1ULL << n);
                cells[i / 3][j / 3] |= (1ULL << n);
            }
        }
        dfs(board, cnt);

        sw.stop();

        v1::SudokuSolver::display_board(board);
        printf("Elapsed time: %0.3f ms\n\n", sw.getElapsedMillisec());
    }

private:
    std::vector<std::bitset<9>> rows;
    std::vector<std::bitset<9>> cols;
    std::vector<std::vector<std::bitset<9>>> cells;
};

} // namespace v2
} // namespace Problem_0037
} // namespace LeetCode

int main(int argn, char * argv[])
{
    using namespace LeetCode::Problem_0037;

    jtest::CPU::warmup(1000);

    {
        printf("--------------------------------\n\n");
        printf("Sudiku: Dancing Links\n\n");
        // Test case 2
        std::vector<std::vector<char>> board;
        for (int row = 0; row < v1::SudokuSolver::Rows; row++) {
            std::vector<char> line;
            for (int col = 0; col < v1::SudokuSolver::Cols; col++) {
                line.push_back(test_board[DIFFICULTY_LEVEL][row * 9 + col]);
            }
            board.push_back(line);
        }

        v1::Solution solution1;
        solution1.solveSudoku(board);
    }

    {
        printf("--------------------------------\n\n");
        printf("Sudiku: Depth first search\n\n");

        // Test case 2
        std::vector<std::vector<char>> board;
        for (int row = 0; row < v1::SudokuSolver::Rows; row++) {
            std::vector<char> line;
            for (int col = 0; col < v1::SudokuSolver::Cols; col++) {
                line.push_back(test_board[DIFFICULTY_LEVEL][row * 9 + col]);
            }
            board.push_back(line);
        }

        v2::Solution solution2;
        solution2.solveSudoku(board);
    }

#if !defined(NDEBUG) && defined(_MSC_VER)
    ::system("pause");
#endif

    return 0;
}
