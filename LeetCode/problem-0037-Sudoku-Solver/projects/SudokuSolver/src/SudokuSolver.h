
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
#include <type_traits>

#define MATRIX_USE_STD_BITSET       0
#define MATRIX_USE_SMALL_BITMAP     1
#define MATRIX_USE_BITMAP           2
#define MATRIX_USE_BITSET           3

#if defined(_MSC_VER)
  #define MATRIX_BITSET_MODE        MATRIX_USE_STD_BITSET
#else
  #define MATRIX_BITSET_MODE        MATRIX_USE_STD_BITSET
#endif

namespace LeetCode {
namespace Problem_37 {

template <size_t Bits>
class SmallBitMap {
public:
    typedef typename std::conditional<(Bits <= sizeof(uint32_t) * 8), uint32_t, size_t>::type  unit_type;

    static const size_t kUnitBits = 8 * sizeof(unit_type);
    static const size_t kUnits = (Bits + kUnitBits - 1) / kUnitBits;
    static const size_t kBytes = kUnits * kUnitBits;

private:
    unit_type bits_[kUnits];

public:
    SmallBitMap() = default;
    ~SmallBitMap() = default;

    size_t size() const        { return Bits; }

          char * data()        { return (char *)      bits_; }
    const char * data() const  { return (const char *)bits_; }

    size_t bytes() const { return kBytes; }
    size_t units() const { return kUnits; }
    size_t unit_length() const { return sizeof(unit_type); }

    void clear() {
        ::memset(this->bits_, 0, kUnits * sizeof(unit_type));
    }

    bool test(size_t pos) const {
        assert(pos < Bits);
        size_t index = pos / kUnitBits;
        size_t shift = pos % kUnitBits;
        return ((this->bits_[index] & unit_type(size_t(1) << shift)) != 0);
    }

    size_t value(size_t pos) const {
        assert(pos < Bits);
        size_t index = pos / kUnitBits;
        size_t shift = pos % kUnitBits;
        return (this->bits_[index] & unit_type(size_t(1) << shift));
    }

    void set() {
        for (size_t i = 0; i < kUnits; i++) {
            this->bits_[i] = unit_type(-1);
        }
    }

    void set(size_t pos) {
        assert(pos < Bits);
        size_t index = pos / kUnitBits;
        size_t shift = pos % kUnitBits;
        this->bits_[index] |= unit_type(size_t(1) << shift);
    }

    void set(size_t pos, bool value) {
        if (value)
            this->set(pos);
        else
            this->reset(pos);
    }

    void reset() {
        for (size_t i = 0; i < kUnits; i++) {
            this->bits_[i] = 0;
        }
    }

    void reset(size_t pos) {
        assert(pos < Bits);
        size_t index = pos / kUnitBits;
        size_t shift = pos % kUnitBits;
        this->bits_[index] &= unit_type(~(size_t(1) << shift));
    }

    void flip() {
        for (size_t i = 0; i < kUnits; i++) {
            this->bits_[i] ^= unit_type(-1);
        }
    }

    void flip(size_t pos) {
        assert(pos < Bits);
        size_t index = pos / kUnitBits;
        size_t shift = pos % kUnitBits;
        this->bits_[index] ^= unit_type(~(size_t(1) << shift));
    }

    void trim() {
		this->bits_[kUnits - 1] &= unit_type(size_t(1) << (Bits % kUnitBits)) - 1;
    }
};

template <size_t Bits>
class BitMap {
public:
    typedef typename std::conditional<(Bits <= sizeof(uint32_t) * 8), uint32_t, size_t>::type  unit_type;

    static const size_t kUnitBits = 8 * sizeof(unit_type);
    static const size_t kUnits = (Bits + kUnitBits - 1) / kUnitBits;
    static const size_t kBytes = kUnits * kUnitBits;

private:
    unit_type * bits_;

public:
    BitMap() : bits_(nullptr) {
        this->bits_ = new unit_type[kUnits];
    }
    ~BitMap() {
        if (this->bits_ != nullptr) {
            delete[] this->bits_;
            this->bits_ = nullptr;
        }
    }

    size_t size() const        { return Bits; }

          char * data()        { return (char *)      bits_; }
    const char * data() const  { return (const char *)bits_; }

    size_t bytes() const { return kBytes; }
    size_t units() const { return kUnits; }
    size_t unit_length() const { return sizeof(unit_type); }

    void clear() {
        if (this->bits_) {
            ::memset(this->bits_, 0, kUnits * sizeof(unit_type));
        }
    }

    bool test(size_t pos) const {
        assert(pos < Bits);
        size_t index = pos / kUnitBits;
        size_t shift = pos % kUnitBits;
        return ((this->bits_[index] & unit_type(size_t(1) << shift)) != 0);
    }

    size_t value(size_t pos) const {
        assert(pos < Bits);
        size_t index = pos / kUnitBits;
        size_t shift = pos % kUnitBits;
        return (this->bits_[index] & unit_type(size_t(1) << shift));
    }

    void set() {
        for (size_t i = 0; i < kUnits; i++) {
            this->bits_[i] = unit_type(-1);
        }
    }

    void set(size_t pos) {
        assert(pos < Bits);
        size_t index = pos / kUnitBits;
        size_t shift = pos % kUnitBits;
        this->bits_[index] |= unit_type(size_t(1) << shift);
    }

    void set(size_t pos, bool value) {
        if (value)
            this->set(pos);
        else
            this->reset(pos);
    }

    void reset() {
        for (size_t i = 0; i < kUnits; i++) {
            this->bits_[i] = 0;
        }
    }

    void reset(size_t pos) {
        assert(pos < Bits);
        size_t index = pos / kUnitBits;
        size_t shift = pos % kUnitBits;
        this->bits_[index] &= unit_type(~(size_t(1) << shift));
    }

    void flip() {
        for (size_t i = 0; i < kUnits; i++) {
            this->bits_[i] ^= unit_type(-1);
        }
    }

    void flip(size_t pos) {
        assert(pos < Bits);
        size_t index = pos / kUnitBits;
        size_t shift = pos % kUnitBits;
        this->bits_[index] ^= unit_type(~(size_t(1) << shift));
    }

    void trim() {
		this->bits_[kUnits - 1] &= unit_type(size_t(1) << (Bits % kUnitBits)) - 1;
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
        this->bytes_ = new uint8_t[kBytes];
    }
    ~BitSet() {
        if (this->bytes_ != nullptr) {
            delete[] this->bytes_;
            this->bytes_ = nullptr;
        }
    }

    size_t size() const        { return Bits; }

          char * data()        { return (      char *)bytes_; }
    const char * data() const  { return (const char *)bytes_; }

    size_t bytes() const { return kBytes; }

    void clear() {
        if (this->bytes_) {
            ::memset(this->bytes_, 0, kBytes * sizeof(uint8_t));
        }
    }

    bool test(size_t pos) const {
        assert(pos < Bits);
        return (this->bytes_[pos] != 0);
    }

    uint8_t value(size_t pos) const {
        assert(pos < Bits);
        return this->bytes_[pos];
    }

    void set() {
        for (size_t i = 0; i < kBytes; i++) {
            this->bytes_[i] = uint8_t(-1);
        }
    }

    void set(size_t pos) {
        assert(pos < Bits);
        this->bytes_[pos] = uint8_t(-1);
    }

    void set(size_t pos, bool value) {
        if (value)
            this->set(pos);
        else
            this->reset(pos);
    }

    void reset() {
        for (size_t i = 0; i < kBytes; i++) {
            this->bytes_[i] = 0;
        }
    }

    void reset(size_t pos) {
        assert(pos < Bits);
        this->bytes_[pos] = 0;
    }

    void flip() {
        for (size_t i = 0; i < kBytes; i++) {
            this->bytes_[i] ^= uint8_t(-1);
        }
    }

    void flip(size_t pos) {
        assert(pos < Bits);
        this->bytes_[pos] ^= uint8_t(-1);
    }

    void trim() {
		/* Not support */
    }

    uint8_t & operator [] (size_t pos) {
        assert(pos < Bits);
        return this->bytes_[pos];
    }

    const uint8_t operator [] (size_t pos) const {
        assert(pos < Bits);
        return this->bytes_[pos];
    }
};

template <size_t Rows, size_t Cols>
class SmallBitMatrix {
private:
#if (MATRIX_BITSET_MODE == MATRIX_USE_SMALL_BITMAP)
    typedef SmallBitMap<Cols>   bitmap_type;
#elif (MATRIX_BITSET_MODE == MATRIX_USE_BITMAP)
    typedef BitMap<Cols>        bitmap_type;
#elif (MATRIX_BITSET_MODE == MATRIX_USE_BITSET)
    typedef BitSet<Cols>        bitmap_type;
#else
    typedef std::bitset<Cols>   bitmap_type;
#endif
    size_t rows_;
    bitmap_type array_[Rows];

public:
    SmallBitMatrix() : rows_(Rows) {}
    ~SmallBitMatrix() {}

    size_t rows() const { return this->rows_; }
    size_t cols() const { return Cols; }

    size_t size() const { return Rows; }
    size_t total_size() const { return (Rows * Cols); }

    void setRows(size_t rows) {
        this->rows_ = rows;
    }

    void clear() {
#if (MATRIX_BITSET_MODE != MATRIX_USE_STD_BITSET)
        for (size_t row = 0; row < Rows; row++) {
            this->array_[row].clear();
        }
#endif
    }

    bool test(size_t row, size_t col) {
        assert(row < Rows);
        return this->array_[row].test(col);
    }

    size_t value(size_t row, size_t col) {
        assert(row < Rows);
#if (MATRIX_BITSET_MODE != MATRIX_USE_STD_BITSET)
        return this->array_[row].value(col);
#else
        return (size_t)(this->array_[row].test(col));
#endif
    }

    void set() {
        for (size_t row = 0; row < Rows; row++) {
            this->array_[row].set();
#if (MATRIX_BITSET_MODE != MATRIX_USE_STD_BITSET)
            this->array_[row].trim();
#endif
        }
    }

    void reset() {
        for (size_t row = 0; row < Rows; row++) {
            this->array_[row].reset();
        }
    }

    void flip() {
        for (size_t row = 0; row < Rows; row++) {
            this->array_[row].flip();
        }
    }

    void trim() {
#if (MATRIX_BITSET_MODE != MATRIX_USE_STD_BITSET)
        for (size_t row = 0; row < Rows; row++) {
            this->array_[row].trim();
        }
#endif
    }

    bitmap_type & operator [] (size_t pos) {
        assert(pos < Rows);
        return this->array_[pos];
    }

    const bitmap_type & operator [] (size_t pos) const {
        assert(pos < Rows);
        return this->array_[pos];
    }
};

template <size_t Rows, size_t Cols>
class SmallBitMatrix2 {
private:
    typedef std::bitset<Cols>   bitset_type;

    bitset_type array_[Rows];

public:
    SmallBitMatrix2() = default;
    ~SmallBitMatrix2() = default;

    size_t rows() const { return Rows; }
    size_t cols() const { return Cols; }

    size_t size() const { return Rows; }
    size_t total_size() const { return (Rows * Cols); }

    bool test(size_t row, size_t col) {
        assert(row < Rows);
        return this->array_[row].test(col);
    }

    void set() {
        for (size_t row = 0; row < Rows; row++) {
            this->array_[row].set();
        }
    }

    void reset() {
        for (size_t row = 0; row < Rows; row++) {
            this->array_[row].reset();
        }
    }

    void flip() {
        for (size_t row = 0; row < Rows; row++) {
            this->array_[row].flip();
        }
    }

    bitset_type & operator [] (size_t pos) {
        assert(pos < Rows);
        return this->array_[pos];
    }

    const bitset_type & operator [] (size_t pos) const {
        assert(pos < Rows);
        return this->array_[pos];
    }
};

template <size_t Depths, size_t Rows, size_t Cols>
class SmallBitMatrix3 {
private:
    typedef SmallBitMatrix2<Rows, Cols>  matrix_type;

    matrix_type matrix_[Depths];

public:
    SmallBitMatrix3() = default;
    ~SmallBitMatrix3() = default;

    size_t depths() const { return Depths; }
    size_t rows() const { return Rows; }
    size_t cols() const { return Cols; }

    size_t size() const { return Depths; }
    size_t matrix2d_size() const { return (Rows * Cols); }
    size_t total_size() const { return (Depths * Rows * Cols); }

    bool test(size_t depth, size_t row, size_t col) {
        assert(depth < Depths);
        return this->matrix_[depth][row].test(col);
    }

    void set() {
        for (size_t depth = 0; depth < Depths; depth++) {
            this->matrix_[depth].set();
        }
    }

    void reset() {
        for (size_t depth = 0; depth < Depths; depth++) {
            this->matrix_[depth].reset();
        }
    }

    void flip() {
        for (size_t depth = 0; depth < Depths; depth++) {
            this->matrix_[depth].flip();
        }
    }

    matrix_type & operator [] (size_t pos) {
        assert(pos < Depths);
        return this->matrix_[pos];
    }

    const matrix_type & operator [] (size_t pos) const {
        assert(pos < Depths);
        return this->matrix_[pos];
    }
};

template <size_t Rows, size_t Cols>
class BitMatrix2 {
private:
    typedef std::bitset<Cols>   bitset_type;

    std::vector<bitset_type> array_;

public:
    BitMatrix2() {
        this->array_.resize(Rows);
    }

    BitMatrix2(const BitMatrix2 & src) {
        this->array_.reserve(Rows);
        for (size_t row = 0; row < Rows; row++) {
            this->array_.push_back(src[row]);
        }
    }

    BitMatrix2(const SmallBitMatrix2<Rows, Cols> & src) {
        this->array_.reserve(Rows);
        for (size_t row = 0; row < Rows; row++) {
            this->array_.push_back(src[row]);
        }
    }

    ~BitMatrix2() = default;

    BitMatrix2 & operator = (const BitMatrix2 & rhs) {
        if (&rhs != this) {
            for (size_t row = 0; row < Rows; row++) {
                this->array_[row] = rhs[row];
            }
        }
    }

    BitMatrix2 & operator = (const SmallBitMatrix2<Rows, Cols> & rhs) {
        for (size_t row = 0; row < Rows; row++) {
            this->array_[row] = rhs[row];
        }
    }

    size_t rows() const { return Rows; }
    size_t cols() const { return Cols; }

    size_t size() const { return Rows; }
    size_t total_size() const { return (Rows * Cols); }

    bool test(size_t row, size_t col) {
        assert(row < Rows);
        return this->array_[row].test(col);
    }

    void set() {
        for (size_t row = 0; row < Rows; row++) {
            this->array_[row].set();
        }
    }

    void reset() {
        for (size_t row = 0; row < Rows; row++) {
            this->array_[row].reset();
        }
    }

    void flip() {
        for (size_t row = 0; row < Rows; row++) {
            this->array_[row].flip();
        }
    }

    bitset_type & operator [] (size_t pos) {
        assert(pos < Rows);
        return this->array_[pos];
    }

    const bitset_type & operator [] (size_t pos) const {
        assert(pos < Rows);
        return this->array_[pos];
    }
};

template <size_t Depths, size_t Rows, size_t Cols>
class BitMatrix3 {
private:
    typedef BitMatrix2<Rows, Cols>  matrix_type;

    std::vector<matrix_type> matrix_;

public:
    BitMatrix3() {
        this->matrix_.resize(Depths);
    }

    BitMatrix3(const BitMatrix3 & src) {
        this->matrix_.reserve(Depths);
        for (size_t depth = 0; depth < Depths; depth++) {
            this->matrix_.push_back(src[depth]);
        }
    }

    BitMatrix3(const SmallBitMatrix3<Depths, Rows, Cols> & src) {
        this->matrix_.reserve(Depths);
        for (size_t depth = 0; depth < Depths; depth++) {
            this->matrix_.push_back(src[depth]);
        }
    }

    ~BitMatrix3() = default;

    BitMatrix3 & operator = (const BitMatrix3 & rhs) {
        if (&rhs != this) {
            for (size_t depth = 0; depth < Depths; depth++) {
                this->matrix_[depth] = rhs[depth];
            }
        }
    }

    BitMatrix3 & operator = (const SmallBitMatrix3<Depths, Rows, Cols> & rhs) {
        for (size_t depth = 0; depth < Depths; depth++) {
            this->matrix_[depth] = rhs[depth];
        }
    }

    size_t depths() const { return Depths; }
    size_t rows() const { return Rows; }
    size_t cols() const { return Cols; }

    size_t size() const { return Depths; }
    size_t matrix2d_size() const { return (Rows * Cols); }
    size_t total_size() const { return (Depths * Rows * Cols); }

    bool test(size_t depth, size_t row, size_t col) {
        assert(depth < Depths);
        return this->matrix_[depth][row].test(col);
    }

    void set() {
        for (size_t depth = 0; depth < Depths; depth++) {
            this->matrix_[depth].set();
        }
    }

    void reset() {
        for (size_t depth = 0; depth < Depths; depth++) {
            this->matrix_[depth].reset();
        }
    }

    void flip() {
        for (size_t depth = 0; depth < Depths; depth++) {
            this->matrix_[depth].flip();
        }
    }

    matrix_type & operator [] (size_t pos) {
        assert(pos < Depths);
        return this->matrix_[pos];
    }

    const matrix_type & operator [] (size_t pos) const {
        assert(pos < Depths);
        return this->matrix_[pos];
    }
};

template <size_t Rows, size_t Cols>
static void matrix2_copy(SmallBitMatrix2<Rows, Cols> & dest,
                         const BitMatrix2<Rows, Cols> & src)
{
    for (size_t row = 0; row < Rows; row++) {
        dest[row] = src[row];
    }
}

template <size_t Rows, size_t Cols>
static void matrix2_copy(BitMatrix2<Rows, Cols> & dest,
                         const SmallBitMatrix2<Rows, Cols> & src)
{
    for (size_t row = 0; row < Rows; row++) {
        dest[row] = src[row];
    }
}

template <size_t Depths, size_t Rows, size_t Cols>
static void matrix3_copy(SmallBitMatrix3<Depths, Rows, Cols> & dest,
                         const BitMatrix3<Depths, Rows, Cols> & src)
{
    for (size_t depth = 0; depth < Depths; depth++) {
        for (size_t row = 0; row < Rows; row++) {
            dest[depth][row] = src[depth][row];
        }
    }
}

template <size_t Depths, size_t Rows, size_t Cols>
static void matrix3_copy(BitMatrix3<Depths, Rows, Cols> & dest,
                         const SmallBitMatrix3<Depths, Rows, Cols> & src)
{
    for (size_t depth = 0; depth < Depths; depth++) {
        for (size_t row = 0; row < Rows; row++) {
            dest[depth][row] = src[depth][row];
        }
    }
}

struct SudokuHelper {
    static const size_t Rows = 9;
    static const size_t Cols = 9;
    static const size_t Palaces = 9;
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
        printf("  ------- ------- -------\n");
        for (size_t row = 0; row < Rows; row++) {
            printf(" | ");
            for (size_t col = 0; col < Cols; col++) {
                char val = board[row][col];
                if (val == ' ' || val == '-')
                    val = '.';
                if (col < Cols - 1)
                    printf("%c ", val);
                else
                    printf("%c ", val);
                if ((col % 3) == 2)
                    printf("| ");
            }
            printf("\n");
            if ((row % 3) == 2) {
                printf("  ------- ------- -------\n");
            }
        }
        printf("\n");
    }

    static void display_answers(std::vector<std::vector<std::vector<char>>> & answers) {
        printf("Total answers: %d\n\n", (int)answers.size());
        int i = 0;
        for (auto answer : answers) {
            SudokuHelper::display_board(answer, false, i);
            i++;
        }
    }
};

} // namespace Problem_37
} // namespace LeetCode

#endif // LEETCODE_SUDOKU_SOLVER_H
