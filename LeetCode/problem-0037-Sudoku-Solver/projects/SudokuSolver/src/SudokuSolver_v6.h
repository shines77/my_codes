
#ifndef LEETCODE_SUDOKU_SOLVER_V6_H
#define LEETCODE_SUDOKU_SOLVER_V6_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <list>
#include <bitset>

#include "SudokuSolver.h"
#include "StopWatch.h"

#define V6_SEARCH_ALL_STAGE     0

#ifdef NDEBUG
#define V6_USE_MOVE_PATH        0
#else
#define V6_USE_MOVE_PATH        1
#endif

namespace LeetCode {
namespace Problem_37 {
namespace v6 {

static const bool kAllowFindInvalidIndex = false;

template <typename T, size_t Capacity>
class SmallFixedDualList {
public:
    typedef T                               value_type;
    typedef SmallFixedDualList<T, Capacity> this_type;

    struct Node {
        int        prev;
        int        next;
        value_type value;

        Node() = default;
        Node(int prev, int next) : prev(prev), next(next) {
        }
        Node(const Node & src) = default;
        ~Node() = default;
    };

    typedef Node node_type;

    static const size_t kCapacity = Capacity + 1;

private:
    size_t           size_;
    size_t           capacity_;
    node_type        data_[kCapacity];

    void init() {
        this->data_[0].prev = 1;
        this->data_[0].next = 1;
    }

public:
    SmallFixedDualList() : size_(1), capacity_(2) {
        this->init();
    }
    ~SmallFixedDualList() {}

    int begin() const { return this->data_[0].next; }
    int end() const   { return 0; }

    int next(int index) const {
        return this->data_[index].next;
    }

    size_t size() const { return this->size_; }
    size_t capacity() const { return this->capacity_; }
    size_t max_capacity() const { return this_type::kCapacity; }

    void init_all() {
        for (int i = 0; i < this_type::kCapacity; i++) {
            this->data_[i].prev = i - 1;
            this->data_[i].next = i + 1;
        }
        this->data_[0] = this_type::kCapacity - 1;
        this->data_[this_type::kCapacity - 1].next = 0;
    }

    void finalize() {
        this->data_[0].prev = (int)(this->size_ - 1);
        this->data_[this->size_ - 1].next = 0;
        this->capacity_ = this->size_ + 1;
    }

    int find(const value_type & key) {
        int index = this->data_[0].next;
        while (index != 0) {
            const value_type & value = this->data_[index].value;
            if (key == value) {
                return index;
            }
            index = this->data_[index].next;
        }
        return index;
    }

    template <typename ... Args>
    void insert(int index, Args && ... args) {
        assert(index > 0);
        assert(index < this->max_capacity());
        assert(this->size_ < this->max_capacity());
        this->data_[index].prev = index - 1;
        this->data_[index].next = index + 1;
        new (&(this->data_[index].value)) value_type(std::forward<Args>(args)...);
        this->size_++;
    }

    void remove(int index) {
        assert(index > 0);
        assert(index < this->capacity());
        assert(this->size_ < this->capacity());
        assert(this->size_ < this->max_capacity());
        this->data_[this->data_[index].prev].next = this->data_[index].next;
        this->data_[this->data_[index].next].prev = this->data_[index].prev;
        assert(this->size_ > 0);
        this->size_--;
    }

    void push_front(int index) {
        assert(index > 0);
        assert(index < this->capacity());
        assert(this->size_ < this->capacity());
        assert(this->size_ < this->max_capacity());
        this->data_[index].prev = 0;
        this->data_[index].next = this->data_[0].next;
        this->data_[this->data_[0].next].prev = index;
        this->data_[0].next = index;
        this->size_++;
    }

    void restore(int index) {
        assert(index > 0);
        assert(index < this->capacity());
        assert(this->size_ < this->capacity());
        assert(this->size_ < this->max_capacity());
        this->data_[this->data_[index].next].prev = index;
        this->data_[this->data_[index].prev].next = index;
        this->size_++;
    }

    value_type & operator [] (int index) {
        assert(index < this->capacity());
        assert(index < this->max_capacity());
        return this->data_[index].value;
    };

    const value_type & operator [] (int index) const {
        assert(index < this->capacity());
        assert(index < this->max_capacity());
        return this->data_[index].value;
    };
};

class Solution {
public:
    static const size_t Rows = SudokuHelper::Rows;
    static const size_t Cols = SudokuHelper::Cols;
    static const size_t Palaces = SudokuHelper::Palaces;
    static const size_t Numbers = SudokuHelper::Numbers;

    struct PosInfo {
        uint8_t row;
        uint8_t col;
        uint8_t palace;
        uint8_t reserve;

        PosInfo() = default;
        PosInfo(size_t row, size_t col, bool is_fast_ctor)
            : row((uint8_t)row), col((uint8_t)col) {
        }
        PosInfo(size_t row, size_t col)
            : row((uint8_t)row), col((uint8_t)col) {
            this->palace = (uint8_t)(row / 3 * 3 + col / 3);
        }
        ~PosInfo() = default;

        bool operator == (const PosInfo & rhs) const {
            return ((this->row == rhs.row) && (this->col == rhs.col));
        }
    };

    struct NumInfo {
        uint8_t palace;
        uint8_t num;
        uint8_t palace_row;
        uint8_t palace_col;

        NumInfo() = default;
        NumInfo(size_t palace, size_t num, bool is_fast_ctor)
            : palace((uint8_t)palace), num((uint8_t)num) {
        }
        NumInfo(size_t palace, size_t num)
            : palace((uint8_t)palace), num((uint8_t)num) {
            this->palace_row = (uint8_t)((palace / 3) * 3);
            this->palace_col = (uint8_t)((palace % 3) * 3);
        }
        ~NumInfo() = default;

        bool operator == (const NumInfo & rhs) const {
            return ((this->palace == rhs.palace) && (this->num == rhs.num));
        }
    };

    struct MoveInfo {
        uint32_t row;
        uint32_t col;
        uint32_t num;

        MoveInfo() = default;
        MoveInfo(size_t row, size_t col, size_t num)
            : row((uint32_t)row), col((uint32_t)col), num((uint32_t)num) {}
        ~MoveInfo() = default;
    };

    enum MoveType {
        ByPalaceNumber,
        ByLocation,
        Unknown
    };

private:
    SmallBitMatrix2<9, 9>    rows;           // [row][num]
    SmallBitMatrix2<9, 9>    cols;           // [col][num]
    SmallBitMatrix2<9, 9>    palaces;        // [palace][num]
    SmallBitMatrix2<81, 9>   usable;         // [row * 9 + col][num]

    //SmallBitMatrix3<9, 9, 3> palace_rows;    // [palace][num][row]
    //SmallBitMatrix3<9, 9, 3> palace_cols;    // [palace][num][col]
    SmallBitMatrix3<9, 9, 9> palace_nums;    // [palace][num][pos]

    //SmallBitMatrix2<3, 9>    palace_row_mask;
    //SmallBitMatrix2<3, 9>    palace_col_mask;

    SmallBitMatrix2<3, 9>    palace_row_rmask;
    SmallBitMatrix2<3, 9>    palace_col_rmask;

#if V6_USE_MOVE_PATH
    std::vector<MoveInfo> move_path;
#endif

#if V6_SEARCH_ALL_STAGE
    std::vector<std::vector<std::vector<char>>> answers;
#endif

public:
    Solution() {
        size_t mask;
#if 0
        // 0x07 = ob000000111 (2)
        mask = size_t(0x07);
        for (size_t row = 0; row < 3; row++) {
            this->palace_row_mask[row] = mask;
            mask <<= 3;
        }

        // 0x49 = 0b001001001 (2)
        mask = size_t(0x49);
        for (size_t col = 0; col < 3; col++) {
            this->palace_col_mask[col] = mask;
            mask <<= 1;
        }
#endif
        // Reverse mask, 0x07 = ob000000111 (2)
        mask = size_t(0x07);
        for (size_t row = 0; row < 3; row++) {
            this->palace_row_rmask[row] = ~mask;
            mask <<= 3;
        }

        // Reverse mask, 0x49 = 0b001001001 (2)
        mask = size_t(0x49);
        for (size_t col = 0; col < 3; col++) {
            this->palace_col_rmask[col] = ~mask;
            mask <<= 1;
        }
    }
    ~Solution() = default;

    int getNextFillCell(SmallFixedDualList<NumInfo, 81> & valid_nums,
                        SmallFixedDualList<PosInfo, 81> & valid_moves,
                        int & out_move_type) {
        assert(valid_nums.size() > 1);
        assert(valid_moves.size() > 1);

        int move_type = MoveType::Unknown;
        int min_index = -1;

        // Find the number that unique position or minimum positions to each palace.
        size_t minPosition = size_t(-1);
        for (int index = valid_nums.begin(); index != valid_nums.end(); index = valid_nums.next(index)) {
            size_t palace = valid_nums[index].palace;
            size_t num    = valid_nums[index].num;
            size_t numPosition = this->palace_nums[palace][num].count();
            if (numPosition < minPosition) {
                if (numPosition == 0) {
                    return -1;
                }
                else if (numPosition == 1) {
                    out_move_type = MoveType::ByPalaceNumber;
                    return index;
                }
                minPosition = numPosition;
                min_index = index;
            }
        }

        if (min_index > 0) {
            move_type = MoveType::ByPalaceNumber;
        }

        // Find the position that unique number or minimum numbers.
        size_t minUsable = minPosition;
        for (int index = valid_moves.begin(); index != valid_moves.end(); index = valid_moves.next(index)) {
            size_t row = valid_moves[index].row;
            size_t col = valid_moves[index].col;
            size_t numUsable = this->usable[row * 9 + col].count();
            if (numUsable < minUsable) {
                if (numUsable == 0) {
                    return -1;
                }
                else if (numUsable == 1) {
                    out_move_type = MoveType::ByLocation;
                    return index;
                }
                minUsable = numUsable;
                min_index = index;
                move_type = MoveType::ByLocation;
            }
        }

        out_move_type = move_type;
        return min_index;
    }

    std::bitset<9> getUsable(size_t row, size_t col) {
        size_t palace = row / 3 * 3 + col / 3;
        return ~(this->rows[row] | this->cols[col] | this->palaces[palace]);
    }

    std::bitset<9> getUsable(size_t row, size_t col, size_t palace) {
        return ~(this->rows[row] | this->cols[col] | this->palaces[palace]);
    }

#define USE_ROW_COL_MASK    1

    void updateUsable(size_t row, size_t col, size_t num) {
        size_t cell_y = row * 9;
        for (size_t x = 0; x < Cols; x++) {
            if (true || (x != col)) {
                this->usable[cell_y + x].reset(num);
            }
        }

        size_t cell_x = col;
        for (size_t y = 0; y < Rows; y++) {
            if (y != row) {
                this->usable[y * 9 + cell_x].reset(num);
            }
        }

        size_t palace_row_idx = row / 3;
        size_t palace_col_idx = col / 3;
        size_t palace = palace_row_idx * 3 + palace_col_idx;
        //this->palace_rows[palace][num].reset();
        //this->palace_cols[palace][num].reset();
        this->palace_nums[palace][num].reset();

        //std::bitset<9> num_bits;
        //size_t num_mask;
        size_t palace_row = row % 3;
        size_t palace_col = col % 3;
        size_t palace_pos = palace_row * 3 + palace_col;
        for (size_t _num = 0; _num < SudokuHelper::Numbers; _num++) {
            if (!this->palaces[palace].test(_num)) {
                if (this->palace_nums[palace][_num].test(palace_pos)) {
#if 0
                    num_bits = this->palace_nums[palace][_num] & this->palace_row_mask[palace_row];
                    num_mask = size_t(1) << palace_pos;
                    if (num_bits == num_mask) {
                        this->palace_rows[palace][_num].reset(palace_row);
                    }
                    num_bits = this->palace_nums[palace][_num] & this->palace_col_mask[palace_col];
                    if (num_bits == num_mask) {
                        this->palace_cols[palace][_num].reset(palace_col);
                    }
#endif
                    this->palace_nums[palace][_num].reset(palace_pos);
                }
            }
        }

        for (size_t idx = 0; idx < 3; idx++) {
            size_t palace_row_id = palace_row_idx * 3 + idx;
            if (idx != palace_col_idx) {
                if (!this->palaces[palace_row_id].test(num)) {
#if 0
                    if (this->palace_nums[palace_row_id][num].test(palace_row * 3 + 0)) {
                        num_bits = this->palace_nums[palace_row_id][num] & this->palace_col_mask[palace_col];
                        num_mask = size_t(1) << (palace_row * 3 + 0);
                        if (num_bits == num_mask) {
                            this->palace_cols[palace_row_id][num].reset(0);
                        }
                    }
                    if (this->palace_nums[palace_row_id][num].test(palace_row * 3 + 1)) {
                        num_bits = this->palace_nums[palace_row_id][num] & this->palace_col_mask[palace_col];
                        num_mask = size_t(1) << (palace_row * 3 + 1);
                        if (num_bits == num_mask) {
                            this->palace_cols[palace_row_id][num].reset(1);
                        }
                    }
                    if (this->palace_nums[palace_row_id][num].test(palace_row * 3 + 2)) {
                        num_bits = this->palace_nums[palace_row_id][num] & this->palace_col_mask[palace_col];
                        num_mask = size_t(1) << (palace_row * 3 + 2);
                        if (num_bits == num_mask) {
                            this->palace_cols[palace_row_id][num].reset(2);
                        }
                    }

                    this->palace_rows[palace_row_id][num].reset(palace_row);
#endif
#if USE_ROW_COL_MASK
                    this->palace_nums[palace_row_id][num] &= this->palace_row_rmask[palace_row];
#else
                    this->palace_nums[palace_row_id][num].reset(palace_row * 3 + 0);
                    this->palace_nums[palace_row_id][num].reset(palace_row * 3 + 1);
                    this->palace_nums[palace_row_id][num].reset(palace_row * 3 + 2);
#endif
                }
            }

            size_t palace_col_id = palace_col_idx + idx * 3;
            if (idx != palace_row_idx) {
                if (!this->palaces[palace_col_id].test(num)) {
#if 0
                    if (this->palace_nums[palace_col_id][num].test(0 * 3 + palace_col)) {
                        num_bits = this->palace_nums[palace_col_id][num] & this->palace_row_mask[palace_row];
                        num_mask = size_t(1) << (0 * 3 + palace_col);
                        if (num_bits == num_mask) {
                            this->palace_rows[palace_col_id][num].reset(0);
                        }
                    }
                    if (this->palace_nums[palace_col_id][num].test(1 * 3 + palace_col)) {
                        num_bits = this->palace_nums[palace_col_id][num] & this->palace_row_mask[palace_row];
                        num_mask = size_t(1) << (1 * 3 + palace_col);
                        if (num_bits == num_mask) {
                            this->palace_rows[palace_col_id][num].reset(1);
                        }
                    }
                    if (this->palace_nums[palace_col_id][num].test(2 * 3 + palace_col)) {
                        num_bits = this->palace_nums[palace_col_id][num] & this->palace_row_mask[palace_row];
                        num_mask = size_t(1) << (2 * 3 + palace_col);
                        if (num_bits == num_mask) {
                            this->palace_rows[palace_col_id][num].reset(2);
                        }
                    }

                    this->palace_cols[palace_col_id][num].reset(palace_col);
#endif
#if USE_ROW_COL_MASK
                    this->palace_nums[palace_col_id][num] &= this->palace_col_rmask[palace_col];
#else
                    this->palace_nums[palace_col_id][num].reset(0 * 3 + palace_col);
                    this->palace_nums[palace_col_id][num].reset(1 * 3 + palace_col);
                    this->palace_nums[palace_col_id][num].reset(2 * 3 + palace_col);
#endif
                }
            }
        }

        size_t cell = cell_y + cell_x;
        size_t pos = palace_row_idx * 3 * 9 + palace_col_idx * 3;
        for (size_t y = 0; y < (Rows / 3); y++) {
            for (size_t x = 0; x < (Cols / 3); x++) {
                if (pos != cell) {
                    this->usable[pos].reset(num);
                }
                pos++;
            }
            pos += (9 - 3);
        }
    }

    template <bool isUndo = true>
    void updateUndoUsable(size_t row, size_t col, size_t _num) {
        size_t cell_y = row * 9;
        size_t palace_row = row / 3 * 3;
        for (size_t x = 0; x < Cols; x++) {
            if (isUndo || (x != col)) {
                size_t palace = palace_row + x / 3;
                this->usable[cell_y + x] = getUsable(row, x, palace);
            }
        }

        size_t cell_x = col;
        size_t palace_col = col / 3;
        for (size_t y = 0; y < Rows; y++) {
            if (y != row) {
                size_t palace = y / 3 * 3 + palace_col;
                this->usable[y * 9 + cell_x] = getUsable(y, col, palace);
            }
        }

        size_t palace = palace_row + palace_col;
        size_t cell = cell_y + cell_x;
        palace_col *= 3;
        size_t pos = palace_row * 9 + palace_col;
        for (size_t y = 0; y < (Rows / 3); y++) {
            for (size_t x = 0; x < (Cols / 3); x++) {
                if (pos != cell) {
                    this->usable[pos] = getUsable(palace_row + y, palace_col + x, palace);
                }
                pos++;
            }
            pos += (9 - 3);
        }

#if 0
        palace_row = (row % 3);
        palace_col = (col % 3);
        size_t palace_pos = palace_row * 3 + palace_col;
        this->palace_rows[palace][_num].set(palace_row);
        this->palace_cols[palace][_num].set(palace_col);
        this->palace_nums[palace][_num].set(palace_pos);

        for (int index = valid_nums.begin(); index != valid_nums.end(); index = valid_nums.next(index)) {
            size_t palace = valid_nums[index].palace;
            size_t num    = valid_nums[index].num;
            const std::bitset<9> & validPos = this->palace_nums[palace][num];
            for (size_t pos = 0; pos < validPos.size(); pos++) {
                // Get usable position
                if (validPos.test(pos)) {
                    row = valid_nums[index].palace_row + (pos / 3);
                    col = valid_nums[index].palace_col + (pos % 3);
                    std::bitset<9> numsUsable = getUsable(row, col, palace);

                    // Get usable positions each number in the same palace.
                    size_t palace_row = (row % 3);
                    size_t palace_col = (col % 3);
                    size_t palace_pos = palace_row * 3 + palace_col;
                    bool isUsable = numsUsable.test(num);
                    if (isUsable) {
                        this->palace_rows[palace][num] |= size_t(1) << (palace_row);
                        this->palace_cols[palace][num] |= size_t(1) << (palace_col);
                        this->palace_nums[palace][num].set(palace_pos);
                    }
                }
            }
        }
#endif
    }

    void fillNum(size_t row, size_t col, size_t num) {
        size_t palace = row / 3 * 3 + col / 3;
        this->rows[row].set(num);
        this->cols[col].set(num);
        this->palaces[palace].set(num);
    }

    void doFillNum(size_t row, size_t col, size_t num) {
        size_t palace = row / 3 * 3 + col / 3;
        this->rows[row].set(num);
        this->cols[col].set(num);
        this->palaces[palace].set(num);
        updateUsable(row, col, num);
#if V6_USE_MOVE_PATH
        this->move_path.push_back(MoveInfo((uint32_t)row, (uint32_t)col, (uint32_t)(num + 1)));
#endif
    }

    void undoFillNum(size_t row, size_t col, size_t num) {
        size_t palace = row / 3 * 3 + col / 3;
        this->rows[row].reset(num);
        this->cols[col].reset(num);
        this->palaces[palace].reset(num);
        updateUndoUsable<true>(row, col, num);
#if V6_USE_MOVE_PATH
        this->move_path.pop_back();
#endif
    }

    bool solve(std::vector<std::vector<char>> & board,
               SmallFixedDualList<NumInfo, 81> & valid_nums,
               SmallFixedDualList<PosInfo, 81> & valid_moves) {
        if (valid_moves.size() <= 1) {
#if V6_SEARCH_ALL_STAGE
            this->answers.push_back(board);
#endif
            return true;
        }

#if 0
        if (valid_moves.size() == 28 || valid_moves.size() == 4) {
            if (valid_moves.size() <= 4)
                SudokuHelper::display_board(board);
        }
#endif

        int move_type;
        int move_idx = getNextFillCell(valid_nums, valid_moves, move_type);
        if (move_idx > 0) {
            // Save palace bitset status
            //BitMatrix3<9, 9, 3> save_palace_rows(this->palace_rows);
            //BitMatrix3<9, 9, 3> save_palace_cols(this->palace_cols);
            BitMatrix3<9, 9, 9> save_palace_nums(this->palace_nums);

            if (move_type == MoveType::ByPalaceNumber) {
                size_t palace = valid_nums[move_idx].palace;
                size_t num    = valid_nums[move_idx].num;
                valid_nums.remove(move_idx);
                // printf(">>>> valid_nums .remove(move_idx  = %d);\n\n", move_idx);
                std::bitset<9> validPos = this->palace_nums[palace][num];
                size_t pos_count = validPos.count();
                size_t count = 0;
                assert(validPos.count() != 0);
                for (size_t pos = 0; pos < validPos.size(); pos++) {
                    // Get usable position
                    if (validPos.test(pos)) {
                        size_t row = valid_nums[move_idx].palace_row + (pos / 3);
                        size_t col = valid_nums[move_idx].palace_col + (pos % 3);
                        doFillNum(row, col, num);

                        int pos_index = valid_moves.find(PosInfo(row, col, false));
                        assert(pos_index > 0);
                        if (!kAllowFindInvalidIndex || (pos_index > 0)) {
                            valid_moves.remove(pos_index);
                            //printf(">>   valid_moves.remove(pos_index = %d); (*)\n\n", pos_index);
                        }

                        // printf(">>   palace: %d, pos: %d, num: %d\n", (int)palace, (int)pos, (int)num);
                        // printf(">>   row: %d, col: %d, num: %d\n\n", (int)row, (int)col, (int)num);

                        board[row][col] = (char)(num + '1');

                        if (kAllowFindInvalidIndex && (pos_index <= 0)) {
                            return true;
                        }

                        if (solve(board, valid_nums, valid_moves)) {
#if (V6_SEARCH_ALL_STAGE == 0)
                            return true;
#endif
                        }

                        board[row][col] = '.';
                        if (!kAllowFindInvalidIndex || (pos_index > 0)) {
                            valid_moves.push_front(pos_index);
                        }

                        undoFillNum(row, col, num);

                        //matrix3_copy(this->palace_rows, save_palace_rows);
                        //matrix3_copy(this->palace_cols, save_palace_cols);
                        matrix3_copy(this->palace_nums, save_palace_nums);

                        count++;
                        if (count >= pos_count)
                            break;
                    }
                }
                valid_nums.push_front(move_idx);
                // printf(">>>> end.\n\n");
            }
            else {
                assert(move_type == MoveType::ByLocation);
                size_t row = valid_moves[move_idx].row;
                size_t col = valid_moves[move_idx].col;
                valid_moves.remove(move_idx);
                // printf(">>>> valid_moves.remove(move_idx = %d);\n\n", move_idx);

                std::bitset<9> validNums = this->usable[row * 9 + col];
                size_t num_count = validNums.count();
                size_t count = 0;
                assert(validNums.count() != 0);
                for (size_t num = 0; num < validNums.size(); num++) {
                    // Get usable numbers
                    if (validNums.test(num)) {
                        doFillNum(row, col, num);

                        size_t palace = valid_moves[move_idx].palace;
                        int num_index = valid_nums.find(NumInfo((uint32_t)palace, (uint32_t)num, false));
                        assert(num_index > 0);
                        if (!kAllowFindInvalidIndex || (num_index > 0)) {
                            valid_nums.remove(num_index);
                            //printf(">>   valid_nums .remove(num_index = %d);\n\n", num_index);
                        }

                        // printf(">>   palace: %d, pos: %d, num: %d\n", (int)palace, (int)((row % 3) * 3 + (col % 3)), (int)num);
                        // printf(">>   row: %d, col: %d, num: %d\n\n", (int)row, (int)col, (int)num);

                        board[row][col] = (char)(num + '1');

                        if (kAllowFindInvalidIndex && (num_index <= 0)) {
                            return true;
                        }

                        if (solve(board, valid_nums, valid_moves)) {
#if (V6_SEARCH_ALL_STAGE == 0)
                            return true;
#endif
                        }

                        board[row][col] = '.';
                        if (!kAllowFindInvalidIndex || (num_index > 0)) {
                            valid_nums.push_front(num_index);
                        }

                        undoFillNum(row, col, num);

                        //matrix3_copy(this->palace_rows, save_palace_rows);
                        //matrix3_copy(this->palace_cols, save_palace_cols);
                        matrix3_copy(this->palace_nums, save_palace_nums);

                        count++;
                        if (count >= num_count)
                            break;
                    }
                }
                valid_moves.push_front(move_idx);
                // printf(">>>> end.\n\n");
            }
        }

        return false;
    }

    void solveSudoku(std::vector<std::vector<char>> & board) {
        SudokuHelper::display_board(board, true);

        jtest::StopWatch sw;
        sw.start();

        SmallFixedDualList<NumInfo, 81> valid_nums;
        SmallFixedDualList<PosInfo, 81> valid_moves;

        int index = 1;
        for (size_t row = 0; row < board.size(); row++) {
            const std::vector<char> & line = board[row];
            for (size_t col = 0; col < line.size(); col++) {
                char val = line[col];
                if (val != '.') {
                    size_t num = val - '1';
                    this->fillNum(row, col, num);
                }
                else {
                    valid_moves.insert(index, (uint32_t)row, (uint32_t)col);
                    index++;
                }
            }   
        }
        valid_moves.finalize();

Find_Next_Step:
        for (size_t row = 0; row < board.size(); row++) {
            const std::vector<char> & line = board[row];
            for (size_t col = 0; col < line.size(); col++) {
                char val = line[col];
                if (val == '.') {
                    // Get usable numbers each position.
                    size_t palace = row / 3 * 3 + col / 3;
                    std::bitset<9> numsUsable = getUsable(row, col, palace);
                    this->usable[row * 9 + col] = numsUsable;

                    // Get usable positions each number in the same palace.
                    size_t palace_row = (row % 3);
                    size_t palace_col = (col % 3);
                    size_t palace_pos = palace_row * 3 + palace_col;
                    for (size_t num = 0; num < SudokuHelper::Numbers; num++) {
                        if (!this->palaces[palace].test(num)) {
                            bool isUsable = numsUsable.test(num);
                            if (isUsable) {
                                //this->palace_rows[palace][num] |= size_t(1) << (palace_row);
                                //this->palace_cols[palace][num] |= size_t(1) << (palace_col);
                                this->palace_nums[palace][num].set(palace_pos);
                            }
                        }
                    }
                }
            }
        }

        // Find and fill the position that unique number.
        for (int index = valid_moves.begin(); index != valid_moves.end(); index = valid_moves.next(index)) {
            size_t row = valid_moves[index].row;
            size_t col = valid_moves[index].col;
            if (this->usable[row * 9 + col].count() == 1) {
                for (size_t num = 0; num < SudokuHelper::Numbers; num++) {
                    if (this->usable[row * 9 + col].test(num)) {
                        doFillNum(row, col, num);
                        board[row][col] = (char)(num + '1');
                        valid_moves.remove(index);
                        goto Find_Next_Step;
                    }
                }
            }
        }
        
        // Find and fill the number that unique position to each palace.
        for (size_t palace = 0; palace < SudokuHelper::Palaces; palace++) {
            for (size_t num = 0; num < SudokuHelper::Numbers; num++) {
                size_t pos_count = this->palace_nums[palace][num].count();
                if (pos_count == 1) {
                    for (size_t pos = 0; pos < SudokuHelper::Numbers; pos++) {
                        if (this->palace_nums[palace][num].test(pos)) {
                            size_t palace_row = palace / 3;
                            size_t palace_col = palace % 3;
                            size_t row = palace_row * 3 + pos / 3;
                            size_t col = palace_col * 3 + pos % 3;

                            doFillNum(row, col, num);
                            board[row][col] = (char)(num + '1');

                            int index = valid_moves.find(PosInfo((uint32_t)row, (uint32_t)col));
                            assert (index != -1);
                            valid_moves.remove(index);
                            goto Find_Next_Step;
                        }
                    }
                }
            }
        }

        int num_index = 1;
        for (size_t palace = 0; palace < SudokuHelper::Palaces; palace++) {
            for (size_t num = 0; num < SudokuHelper::Numbers; num++) {
                size_t pos_count = this->palace_nums[palace][num].count();
                if (pos_count > 1) {
                    valid_nums.insert(num_index, palace, num);
                    num_index++;
                }
            }
        }
        valid_nums.finalize();

        this->solve(board, valid_nums, valid_moves);

        sw.stop();

#if V6_SEARCH_ALL_STAGE
        SudokuHelper::display_answers(this->answers);
#else
        SudokuHelper::display_board(board);
#endif
        printf("Elapsed time: %0.3f ms\n\n", sw.getElapsedMillisec());
    }
};

} // namespace v6
} // namespace Problem_0037
} // namespace LeetCode

#endif // LEETCODE_SUDOKU_SOLVER_V6_H
