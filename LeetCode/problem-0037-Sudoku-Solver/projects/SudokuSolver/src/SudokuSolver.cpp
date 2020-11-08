
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <vector>

#include "StopWatch.h"

#define MATRIX_USE_BITMAP  0

static const char test_board[81] = {
    '5', '3', '.', '.', '7', '.', '.', '.', '.',
    '6', '.', '.', '1', '9', '5', '.', '.', '.',
    '.', '9', '8', '.', '.', '.', '.', '6', '.',
    '8', '.', '.', '.', '6', '.', '.', '.', '3',
    '4', '.', '.', '8', '.', '3', '.', '.', '1',
    '7', '.', '.', '.', '2', '.', '.', '.', '6',
    '.', '6', '.', '.', '.', '.', '2', '8', '.',
    '.', '.', '.', '4', '1', '9', '.', '.', '5',
    '.', '.', '.', '.', '8', '.', '.', '7', '9'
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
    size_t bitcount_;

public:
    BitMap() : bits_(nullptr), bitcount_(0) {
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

    size_t bitcount() const { return bitcount_; }

    void clear() {
        if (bits_) {
            ::memset(bits_, 0, kBytes * sizeof(size_t));
        }
        bitcount_ = 0;
    }

    bool get(size_t pos) const {
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
        bitcount_++;
        size_t index = pos / kAlignmentBits;
        size_t shift = pos % kAlignmentBits;
        bits_[index] |= (1ULL << shift);
    }

    void reset(size_t pos) {
        assert(pos < Bits);
        bitcount_--;
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
    size_t bitcount_;

public:
    BitSet() : bytes_(nullptr), bitcount_(0) {
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

    size_t bitcount() const { return bitcount_; }

    void clear() {
        if (bytes_) {
            ::memset(bytes_, 0, kBytes * sizeof(uint8_t));
        }
        bitcount_ = 0;
    }

    bool get(size_t pos) const {
        assert(pos < Bits);
        return (bytes_[pos] != 0);
    }

    uint8_t value(size_t pos) const {
        assert(pos < Bits);
        return bytes_[pos];
    }

    void set(size_t pos) {
        assert(pos < Bits);
        bitcount_++;
        bytes_[pos] = 1;
    }

    void reset(size_t pos) {
        assert(pos < Bits);
        bitcount_--;
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
#if MATRIX_USE_BITMAP
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
        for (size_t row = 0; row < Rows; row++) {
            data_[row].clear();
        }
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
    static const size_t Places = 9;

    static const size_t BoardSize = Rows * Cols * Places;

    typedef BitMartix<BoardSize, Rows * Cols * 4> matrix_type;

private:
    matrix_type matrix;
    size_t rows[BoardSize + 1];
    size_t cols[BoardSize + 1];
    size_t places[BoardSize + 1];

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
                size_t m = 0, n = Places - 1;
                if (board[row][col] != '.') {
                    m = n = board[row][col] - '1';
                }
                for (size_t place = m; place <= n; place++) {
#if MATRIX_USE_BITMAP
                    matrix[index].set(81 * 0 + row * 9 + col);
                    matrix[index].set(81 * 1 + row * 9 + place);
                    matrix[index].set(81 * 2 + col * 9 + place);
                    matrix[index].set(81 * 3 + (row / 3 * 3 + col / 3) * 9 + place);
#else
                    matrix[index][81 * 0 + row * 9 + col] = 1;
                    matrix[index][81 * 1 + row * 9 + place] = 1;
                    matrix[index][81 * 2 + col * 9 + place] = 1;
                    matrix[index][81 * 3 + (row / 3 * 3 + col / 3) * 9 + place] = 1;
#endif
                    rows[index + 1] = row;
                    cols[index + 1] = col;
                    places[index + 1] = place;
                    index++;
                }
            }
        }
    }

public:
    void output_answer(std::vector<std::vector<char>> & board,
                       const DancingLinks * dancingLinks);

    static void display_board(const std::vector<std::vector<char>> & board, bool is_input = false) {
        if (is_input) {
            printf("\n");
            printf("The input is:\n");
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
        Unknwon,
        SearchNext,
        Retry,
        BackTracking,
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
                if (matrix[row].get(col)) {
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
        int state = StackState::Unknwon;
        std::vector<StackInfo> stack;
        StackInfo stack_info;
        int depth = 0;
        int i, col;

        do {
            if (this->is_empty()) {
                return true;
            }
        
            if (state != StackState::BackTracking && state != StackState::Retry) {
                col = get_min_column();
                this->erase(col);
            }

            if (state != StackState::BackTracking) {
                if (state != StackState::Retry) {
                    state = StackState::Unknwon;
                    i = link_list[col].down;
                }
                while (i != col) {
                    answer.push_back(link_list[i].row);
                    stack_info.set(col, i);
                    stack.push_back(stack_info);
                    for (int j = link_list[i].next; j != i; j = link_list[j].next) {
                        this->erase(link_list[j].col);
                    }

                    depth++;
                    state = StackState::SearchNext;
                    break;
                }
                if (state == StackState::SearchNext) {
                    continue;
                }
            }
            else {
                depth--;
                stack_info = stack.back();
                i = stack_info.i;
                col = stack_info.col;

                for (int j = link_list[i].prev; j != i; j = link_list[j].prev) {
                    this->recover(link_list[j].col);
                }
                stack.pop_back();
                answer.pop_back();

                i = link_list[i].down;

                state = StackState::Retry;
                continue;
            }

            this->recover(col);
            state = StackState::BackTracking;
        } while (1);

        return false;
    }
};

void SudokuSolver::output_answer(std::vector<std::vector<char>> & board,
                                 const DancingLinks * dancingLinks) {
    for (auto idx : dancingLinks->get_answer()) {
        board[rows[idx]][cols[idx]] = (char)places[idx] + '1';
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
        //dancingLinks.solve_non_recursive();
        dancingLinks.solve();

        sw.stop();

        solver.output_answer(board, &dancingLinks);
        SudokuSolver::display_board(board);
        printf("Elapsed time: %0.3f ms\n\n", sw.getElapsedMillisec());
    }
};

} // namespace v1
} // namespace Problem_0037
} // namespace LeetCode

int main(int argn, char * argv[])
{
    using namespace LeetCode::Problem_0037;

    // Test case
    std::vector<std::vector<char>> board;
    for (int row = 0; row < v1::SudokuSolver::Rows; row++) {
        std::vector<char> line;
        for (int col = 0; col < v1::SudokuSolver::Cols; col++) {
            line.push_back(test_board[row * 9 + col]);
        }
        board.push_back(line);
    }

    v1::Solution solution;
    solution.solveSudoku(board);

#if !defined(NDEBUG) && defined(_MSC_VER)
    ::system("pause");
#endif

    return 0;
}
