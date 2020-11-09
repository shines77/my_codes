
#ifndef LEETCODE_SUDOKU_SOLVER_H
#define LEETCODE_SUDOKU_SOLVER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <bitset>

#define MATRIX_USE_STD_BITSET   1
#define MATRIX_USE_BITMAP       0

namespace LeetCode {
namespace Problem_0037 {

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
        assert(row < Rows);
        return data_[row].test(col);
    }

    size_t value(size_t row, size_t col) {
        assert(row < Rows);
        return data_[row].value(col);
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

struct SudokuHelper {
    static const size_t Rows = 9;
    static const size_t Cols = 9;
    static const size_t Numbers = 9;

    static void display_board(const std::vector<std::vector<char>> & board,
                              bool is_input = false,
                              int idx = -1) {
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
            if (idx == -1)
                printf("The answer is:\n");
            else
                printf("The answer # %d is:\n", idx + 1);
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

} // namespace Problem_0037
} // namespace LeetCode

#endif // LEETCODE_SUDOKU_SOLVER_H
