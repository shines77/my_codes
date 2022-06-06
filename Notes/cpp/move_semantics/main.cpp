
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

class Foo {
private:
    std::string key_;
    int         value_;

public:
    Foo(int value = 0) noexcept : value_(value) {
        printf("Foo(int value = %d);\n", value);
    }

    Foo(const Foo & src) noexcept : key_(src.key_), value_(src.value_) {
        printf("Foo(const Foo & src);\n");
    }

    template <typename T>
    Foo(T && src) noexcept : key_(std::move(src.key_)), value_(src.value_) {
        static constexpr bool isMoveSemantics = std::is_rvalue_reference<decltype(src)>::value;
        printf("Foo(Foo && src);\n");
        printf("isMoveSemantics = %d\n", (int)isMoveSemantics);
    }

    void printValue() {
        printf("key = \"%s\"\n", key_.c_str());
        printf("value = %d\n", value_);
    }

    void println() {
        printf("\n");
    }

    void insert(const std::string & key, int value) {
        printf("Foo::insert(const std::string & key, int value);\n");
        key_ = key;
        value_ = value;
    }

    void insert(std::string && key, int value) {
        printf("Foo::insert(std::string && key, int value);\n");
        key_ = std::move(key);
        value_ = value;
    }

    ~Foo() {
        //
    }
};

void class_ctor_MoveSemantics_test()
{
    printf("-----------------------------------------------------------------------\n");

    {
        Foo f1(1);
        Foo foo(f1);
        foo.printValue();
        foo.println();
    }

    {
        const Foo f2(2);
        Foo foo(f2);
        foo.printValue();
        foo.println();
    }

    {
        Foo foo(Foo(3));
        foo.printValue();
        foo.println();
    }

    {
        Foo foo(*new Foo(4));
        foo.printValue();
        foo.println();
    }

    {
        Foo foo = Foo(5);
        foo.printValue();
        foo.println();
    }

    {
        Foo foo(std::move(Foo(6)));
        foo.printValue();
        foo.println();
    }

    {
        Foo foo = std::move(Foo(7));
        foo.printValue();
        foo.println();
    }
}

void value_transfer_MoveSemantics_test()
{
    printf("-----------------------------------------------------------------------\n");

    Foo foo;
    foo.insert(std::string("test1"), 1);
    foo.printValue();
    foo.println();

    const std::string & str = std::string("test2");
    foo.insert(str, 2);
    foo.printValue();
    foo.println();

    foo.insert(std::move(str), 3);
    foo.printValue();
    foo.println();
}

int main(int argc, char * argv[])
{
    class_ctor_MoveSemantics_test();
    value_transfer_MoveSemantics_test();
    return 0;
}
