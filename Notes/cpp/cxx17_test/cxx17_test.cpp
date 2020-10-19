// cxx17_test.cpp

#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <cstring>
#include <array>

#include <iostream>

#define GetSourceRootDir(source_filename, relative_filename) \
    parseSourceRootDir(source_filename, sizeof(source_filename), \
                       relative_filename, sizeof(relative_filename))

#define GetSourceRootDirOffset(source_filename, relative_filename) \
    getSourceRootDirOffset(source_filename, sizeof(source_filename), \
                           relative_filename, sizeof(relative_filename))

constexpr size_t cstrlen(const char * str)
{
    const char * start = str;
    while (*str != '\0') {
        str++;
    }
    return (str - start);
}

static constexpr size_t kSourceRootDirSize = 512;
static std::array<char, kSourceRootDirSize> sourceRootDir = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const char * parseSourceRootDir(const char * source_filename, const size_t len_source,
                                const char * relative_filename, const size_t len_relative)
{
    if (len_source == 0 || len_relative == 0) {
        return &sourceRootDir[0];
    }

    ptrdiff_t pos = -1;
    if (len_source >= len_relative) {
        const char * source = source_filename + len_source - 1;
        const char * relative = relative_filename + len_relative - 1;
        while (relative > relative_filename) {
            if (*relative == *source) {
                relative--;
                source--;
            }
            else if (*relative == '\\' || *relative == '/') {
                if (*source == '\\' || *source == '/') {
                    relative--;
                    source--;
                }
                else {
                    break;
                }
            }
            else {
                break;
            }
        }
        pos = source - source_filename;
        for (ptrdiff_t i = 0; i < pos; i++) {
            sourceRootDir[i] = source_filename[i];
        }
    }
    else {
        for (ptrdiff_t i = ptrdiff_t(len_source - 1); i >= 0; i--) {
            char ch = source_filename[i];
            if (ch == '\\' || ch == '/') {
                pos = i;
                break;
            }
        }
        if (pos == -1) {
            pos = len_source;
        }

        for (ptrdiff_t i = 0; i < pos; i++) {
            sourceRootDir[i] = source_filename[i];
        }
    }

    return &sourceRootDir[0];
}

constexpr size_t getSourceRootDirOffset(const char * source_filename, const size_t len_source,
                                        const char * relative_filename, const size_t len_relative)
{
    if (len_source == 0 || len_relative == 0) {
        return size_t(-1);
    }

    ptrdiff_t pos = -1;
    if (len_source >= len_relative) {
        const char * source = source_filename + len_source - 1;
        const char * relative = relative_filename + len_relative - 1;
        while (relative > relative_filename) {
            if (*relative == *source) {
                relative--;
                source--;
            }
            else if (*relative == '\\' || *relative == '/') {
                if (*source == '\\' || *source == '/') {
                    relative--;
                    source--;
                }
                else {
                    break;
                }
            }
            else {
                break;
            }
        }

        pos = source - source_filename;
    }
    else {
        for (ptrdiff_t i = ptrdiff_t(len_source - 1); i >= 0; i--) {
            char ch = source_filename[i];
            if (ch == '\\' || ch == '/') {
                pos = i;
                break;
            }
        }
        if (pos == -1) {
            pos = len_source;
        }
    }

    return size_t(pos);
}

/*********************************************************************************/

//
// C++ 14
//
constexpr char pathCharToUpper(char c)
{
    if (c == '\\')
        return '/';
    else
        return ((c >= 'a' && c <= 'z') ? (c - 0x20) : c);
}

constexpr bool isPathSeparator(char c)
{
    return (c == '/' || c == '\\');
}

constexpr bool lastCharIsPathSeparator(const char * first, const char * last)
{
    ptrdiff_t size = last - first;
    if (size > 0) {
        const char * tail = last - 1;
        return isPathSeparator(*tail);
    }
    return false;
}

constexpr const char * getSourceFileName_cxx14(const char * source_path,
                                               const char * source_prefix,
                                               bool include_prefix)
{
    const char * filename = source_path;
    const char * path = source_path;
    while (*path != '\0') {
        const char * target = source_prefix;
        //// Find first char equal target
        //do {
        //    if (pathCharToUpper(*path) != pathCharToUpper(*target))
        //        path++;
        //    else
        //        break;
        //} while (*path != '\0');
        const char * start_pos = path;
        do {
            if (pathCharToUpper(*path) != pathCharToUpper(*target)) {
                if (*target != '\0') {
                    path++;
                    break;
                }
                else {
                    // Target reach the end of and path is not a null terminator.
                    if (target <= source_prefix) {
                        return source_path;
                    }
                    bool lastTargetIsSep = lastCharIsPathSeparator(source_prefix, target);
                    bool curPathIsSep = isPathSeparator(*path);
                    if ((lastTargetIsSep && !curPathIsSep) ||
                        (!lastTargetIsSep && curPathIsSep)) {
                        if (include_prefix) {
                            filename = path - (target - source_prefix);
                        }
                        else {
                            bool lastPathIsSep = lastCharIsPathSeparator(source_path, path);
                            if (lastTargetIsSep && lastPathIsSep)
                                filename = path - 1;
                            else
                                filename = path;
                        }
                    }
                    // In fact, start_pos = path - (target - source_prefix)
                    path = start_pos + 1;
                    break;
                }
            }
            else {
                if (*target != '\0') {
                    path++;
                    target++;
                }
                else {
                    // Target reach the end of and both path and target is null terminator.
                    if (target <= source_prefix) {
                        return source_path;
                    }
                    bool lastTargetIsSep = lastCharIsPathSeparator(source_prefix, target);
                    bool curPathIsSep = isPathSeparator(*path);
                    if ((lastTargetIsSep && !curPathIsSep) ||
                        (!lastTargetIsSep && curPathIsSep)) {
                        if (include_prefix) {
                            filename = path - (target - source_prefix);
                        }
                        else {
                            bool lastPathIsSep = lastCharIsPathSeparator(source_path, path);
                            if (lastTargetIsSep && lastPathIsSep)
                                filename = path - 1;
                            else
                                filename = path;
                        }
                    }
                    // In fact, start_pos = path - (target - source_prefix)
                    path = start_pos + 1;
                    break;
                }
            }
        } while (1);
    }
    return filename;
}

/*********************************************************************************/

//
// C++ 14
//
// How to extract the source filename without path and suffix at compile time?
// https://stackoverflow.com/questions/31050113/how-to-extract-the-source-filename-without-path-and-suffix-at-compile-time
//
constexpr const char * getSourceBaseFileName_cxx14(const char * path) {
    const char * file = path;
    while (*path != '\0') {
        if (*path == '/' || *path == '\\') {
            path++;
            file = path;
        }
        path++;
    }
    return file;
}

/*********************************************************************************/

//
// C++ 14
//
// How to extract the source filename without path and suffix at compile time?
// https://stackoverflow.com/questions/31050113/how-to-extract-the-source-filename-without-path-and-suffix-at-compile-time
//
namespace detail {
    constexpr bool is_path_sep(char c) {
        return (c == '/' || c == '\\');
    }

    constexpr const char * strip_path(const char * path) {
        auto lastname = path;
        for (auto p = path ; *p ; ++p) {
            if (is_path_sep(*p) && *(p + 1))
                lastname = p + 1;
        }
        return lastname;
    }

    struct basename_impl {
        constexpr basename_impl(const char * begin, const char * end)
            : _begin(begin), _end(end) {
        }

        void write(std::ostream & os) const {
            os.write(_begin, _end - _begin);
        }

        std::string as_string() const {
            return std::string(_begin, _end);
        }

        const char * const _begin;
        const char * const _end;
    };

    inline std::ostream & operator<<(std::ostream & os, const basename_impl & bi) {
        bi.write(os);
        return os;
    }

    inline std::string to_string(const basename_impl & bi) {
        return bi.as_string();
    }

    constexpr const char * last_dot_of(const char * p) {
        const char * last_dot = nullptr;
        for ( ; *p ; ++p) {
            if (*p == '.')
                last_dot = p;
        }
        return (last_dot ? last_dot : p);
    }
}

/*********************************************************************************/

//
// C++ 11
// Msvc 2015 update 3, gcc 5.4, clang 3.8.0
//
// __FILE__ macro shows full path
// See: https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
//
template <typename T, std::size_t S>
inline
constexpr std::size_t getSourceFileNameOffset(const T (& str)[S], std::size_t i = S - 1)
{
    return (str[i] == '/' || str[i] == '\\') ? (i + 1) : ((i > 0) ? getSourceFileNameOffset(str, i - 1) : 0);
}

template <typename T>
inline
constexpr std::size_t getSourceFileNameOffset(T (& str)[1])
{
    return 0;
}

/*********************************************************************************/

//
// C++ 11
//
// How to extract the source filename without path and suffix at compile time?
// https://stackoverflow.com/questions/31050113/how-to-extract-the-source-filename-without-path-and-suffix-at-compile-time
//
constexpr const char * str_end(const char * str) {
    return ((*str) ? str_end(str + 1) : str);
}

constexpr bool str_slant(const char * str) {
    return (*str == '/' || *str == '\\') ? true : ((*str) ? str_slant(str + 1) : false);
}

constexpr const char * r_slant(const char * str) {
    return (*str == '/' || *str == '\\') ? (str + 1) : r_slant(str - 1);
}

constexpr const char * getSourceFileName_cxx11(const char * str) {
    return str_slant(str) ? r_slant(str_end(str)) : str;
}

/*********************************************************************************/

//
// C++ 11
//
// How to extract the source filename without path and suffix at compile time?
// https://stackoverflow.com/questions/31050113/how-to-extract-the-source-filename-without-path-and-suffix-at-compile-time
//
constexpr bool is_null_terminator(char c) {
    return (c == '\0');
}

constexpr bool is_not_null_terminator(char c) {
    return (c != '\0');
}

constexpr bool is_path_separator(char c) {
    return (c == '/' || c == '\\');
}

constexpr const char * next_str(const char * str) {
    return (is_not_null_terminator(*str) ? next_str(str + 1) : str);
}

constexpr bool find_first_of_separator(const char * str) {
    return (is_path_separator(*str) ? true : (is_not_null_terminator(*str) ? find_first_of_separator(str + 1) : false));
}

constexpr const char * find_separator(const char * str) {
    return (is_path_separator(*str) ? (str + 1) : find_separator(str - 1));
}

constexpr const char * getSourceFileName_cxx11_new(const char* str) {
    return (find_first_of_separator(str) ? find_separator(next_str(str)) : str);
}

/*********************************************************************************/

int main()
{
    std::cout << "Hello World!\n\n";
    std::cout << "__FILE__       : " << __FILE__ << "\n\n";

    static const char * source_root_dir = GetSourceRootDir(__FILE__, "/cxx17_test/cxx17_test.cpp");
    static constexpr size_t kSourceRootDir = GetSourceRootDirOffset(__FILE__, "/cxx17_test/cxx17_test.cpp");

    constexpr const char * src_filename_cxx14_1 = getSourceFileName_cxx14(__FILE__, "/cxx17_test/", false);
    constexpr const char * src_filename_cxx14_2 = getSourceBaseFileName_cxx14(__FILE__);

    // the filename with extension but no path
    constexpr auto filename_cxx14_3 = detail::strip_path(__FILE__);
    constexpr auto basename_cxx14_3 = detail::basename_impl(filename_cxx14_3, detail::last_dot_of(filename_cxx14_3));

    //
    // gcc: at least gcc 6.1 + -O1
    // msvc: put result into constexpr variable
    // clang: persists on not compile time evaluation
    //
    constexpr const char * src_filename_cxx11_1 = &__FILE__[getSourceFileNameOffset(__FILE__)];

    //
    // c++ 11
    // https://stackoverflow.com/questions/31050113/how-to-extract-the-source-filename-without-path-and-suffix-at-compile-time
    //
    constexpr const char * src_filename_cxx11_2 = getSourceFileName_cxx11(__FILE__);
    constexpr const char * src_filename_cxx11_3 = getSourceFileName_cxx11_new(__FILE__);

    std::cout << "src_filename_cxx14_1: " << src_filename_cxx14_1 << "\n\n";
    std::cout << "src_filename_cxx14_2: " << src_filename_cxx14_2 << "\n\n";
    std::cout << "filename_cxx14_3:     " << filename_cxx14_3 << "\n\n";
    std::cout << "basename_cxx14_3:     " << basename_cxx14_3 << "\n\n";
    std::cout << "src_filename_cxx11_1: " << src_filename_cxx11_1 << "\n\n";
    std::cout << "src_filename_cxx11_2: " << src_filename_cxx11_2 << "\n\n";
    std::cout << "src_filename_cxx11_3: " << src_filename_cxx11_3 << "\n\n";
    std::cout << "source_root_dir_offset: " << kSourceRootDir << "\n\n";
    std::cout << "source_root_dir: " << source_root_dir << "\n\n";

    std::array<int, kSourceRootDir> test = { 0 };
    for (auto iter = test.cbegin(); iter != test.cend(); ++iter) {
        std::cout << *iter << ", ";
    }
    std::cout << std::endl << std::endl;
}
