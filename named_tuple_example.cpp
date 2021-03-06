
// See README.md for more details about named_tuple.

// Author: Erez Strauss <erez@erezstrauss.com>

#include <named_tuple.h>
#include <array>
#include <iostream>
#include <sstream>

namespace nvt = nvtuple_ns;

#define EXAMPLE(NAME, CODE)                                                \
    do {                                                                   \
        const std::string xname = NAME;                                    \
        std::string codestring = #CODE;                                    \
        codestring = codestring.substr(7);                                 \
        codestring.resize(codestring.size() - 4);                          \
        std::stringstream outstring;                                       \
        CODE;                                                              \
        std::cout << xname << (xname.empty() ? "" : "\n") << "=== code:\n" \
                  << codestring << "\n=== output:\n"                       \
                  << outstring.str() << "\n===\n";                         \
    } while (0)

// Example 1 shows named tuple creation and printing
void example_1() {
    auto t1 =
        std::make_tuple(("fieldA"_, 123), ("B"_, 100.111), ("lastone"_, 999));
    std::cout << "example 1:\n  " << t1 << '\n';
    EXAMPLE(
        "First one", ([&]() {
            outstring << nvt::named_tuple{("first"_, 12), ("second"_, 4.99)};
        }()));
    EXAMPLE(
        "Multi line", ([&]() {
            outstring << nvt::named_tuple{("first"_, 12), ("second"_, 4.99)};
        }()));
    EXAMPLE("fields names", ([&]() {
                auto x = nvt::named_tuple{("first"_, 12), ("second"_, 4.99)};
                for (auto& nm : x.names()) outstring << "'" << nm << "'\n";
            }()));
}

// Example 2 shows named_tuple and field modification using operator[]
void example_2() {
    auto t2 =
        nvt::named_tuple{("fieldA"_, 123), ("B"_, 100.111), ("lastone"_, 999)};
    std::cout << "example 2:\n  " << t2 << '\n';
    t2["fieldA"_] = 321;
    std::cout << "  changed fieldA: " << t2 << '\n';

    auto pfoo = [](auto x) {
        std::cout << "    name: '" << x.get_value_name()
                  << "' value: " << x.get() << '\n';
    };
    t2.foreach (pfoo);
}

template<typename... TS>
class Data : public std::tuple<std::decay_t<TS>...> {
   public:
    Data(TS&&... ts) noexcept
        : std::tuple<std::decay_t<TS>...>(std::forward<TS>(ts)...) {}
};

// Example 3 - shows simple tuple printing.
void example_3() {
    Data<std::string, double> simple_tuple("hello", 555);

    std::cout << "example 3:\n  Data: " << simple_tuple << '\n';
}

// Example 4 - shows Named types and Named values
void example_4() {
    auto abc = "abc"_;
    std::cout << "example 4:\n  abc: " << abc << '\n';
    auto nv = nvt::named_value<int, decltype("xyz"_)>{42};
    std::cout << "   " << nv << '\n';
}

// Example 5 - if two fields have the same name - static asset will avoid, try
// to change fieldB to fieldA
void example_5() {
    // auto t5 = named_tuple {("fieldA"_ = 123), ("B"_ = 100.111), ("fieldA"_
    // = 4.5) }; - fails at compile time, as it detect two fields with same
    // named-type.
    auto t5 =
        nvt::named_tuple{("fieldA"_, 123), ("B"_, 100.111), ("fieldB"_, 4.5)};
    std::cout << "example 5:\n  " << t5 << '\n';
}

template<>
inline std::ostream& operator<<(
    std::ostream& os,
    const nvt::named_value<std::string, decltype("address"_)>& nv) {
    os << nv.get_value_name() << ": '" << nv.get() << "'";
    return os;
}

// Example 6 shows defining a named tuple type Person, and using overrided
// operator << for specific field name.

void example_6() {
    using Person =
        nvt::named_tuple<nvt::named_value<unsigned, decltype("id"_)>,
                         nvt::named_value<float, decltype("age"_)>,
                         nvt::named_value<std::string, decltype("name"_)>,
                         nvt::named_value<std::string, decltype("address"_)> >;
    Person per;
    per["id"_] = 111;
    per["age"_] = 16.5;
    per["name"_] = "Bob";
    per["address"_] = "101 Main St. Big City";
    std::cout << "example 6:\n  " << per << '\n';

    auto naked_tuple = per.simple_tuple();
    std::cout << "  get<0>(naked_tuple)  " << std::get<0>(naked_tuple) << '\n';
    std::cout << "  get<1>(naked_tuple)  " << std::get<1>(naked_tuple) << '\n';
    std::cout << "  get<2>(naked_tuple)  " << std::get<2>(naked_tuple) << '\n';
    std::cout << "  get<3>(naked_tuple)  " << std::get<3>(naked_tuple) << '\n';
    static_assert(sizeof(naked_tuple) == sizeof(per),
                  "std::uple size != named_tuple size !");
}

void example_7() {
    using Person =
        nvt::named_tuple<nvt::named_value<unsigned, decltype("id"_)>,
                         nvt::named_value<float, decltype("age"_)>,
                         nvt::named_value<std::string, decltype("name"_)>,
                         nvt::named_value<std::string, decltype("address"_)> >;
    Person per;
    per["id"_] = 111;
    per["age"_] = 16.5;
    per["name"_] = "Bob";
    per["address"_] = "101 Main St. Big City";
    std::cout << "example 7:\n   " << per << '\n';
    auto update = nvt::named_tuple{("id"_, 333), ("age"_, 99.9)};
    std::cout << "   " << (per << update) << '\n';
}

void example_8() {
    using Person =
        nvt::named_tuple<nvt::named_value<unsigned, decltype("id"_)>,
                         nvt::named_value<float, decltype("age"_)>,
                         nvt::named_value<std::string, decltype("name"_)>,
                         nvt::named_value<std::string, decltype("address"_)> >;
    Person per;
    per["id"_] = 111;
    per["age"_] = 16.5;
    per["name"_] = "Bob";
    per["address"_] = "101 Main St. Big City";
    std::cout << "example 8:\n   " << per << '\n';
    std::cout << "   " << (per << ("age"_, 3) << ("id"_, 1001)) << '\n';
}

void example_9() {
    auto t1 = nvt::named_tuple{("addr"_, (void*)example_9), ("size"_, 999),
                               ("fldx"_, .333)};
    auto t2 = nvt::named_tuple{("size"_, 888), ("addr"_, (void*)example_8)};
    std::cout << "example 8:"
              << "\n   t1: " << t1 << "\n   t2: " << t2;
    t1 << t2;  // t1 = t2 , but << is better as in mean update with fields from
               // t2
    std::cout << "\n   t1: " << t1 << "\n   t2: " << t2 << '\n';
}

void example_10() {
    auto t1 = nvt::named_tuple{("a"_, 10), ("b"_, "nameA")};
    using Data = nvt::named_tuple<nvt::named_value<std::string, decltype("b"_)>,
                                  nvt::named_value<int, decltype("a"_)> >;

    auto t2 = Data(t1);
    std::cout << "example 10 constructor with compatible named_tuple:\n   "
              << t1 << "\n   " << t2 << '\n';
}

void example_11() {
    using MemRegion =
        nvt::named_tuple<nvt::named_value<void*, decltype("addr"_)>,
                         nvt::named_value<uint64_t, decltype("size"_)> >;

    auto t1 = MemRegion{("size"_, 100), ("addr"_, (void*)example_11)};
    std::cout << "example 11 constructor with compatible named_values:\n   "
              << t1 << "\n";
}

int main() {
    example_1();
    example_2();
    example_3();
    example_4();
    example_5();
    example_6();
    example_7();
    example_8();
    example_9();
    example_10();
    example_11();

    std::cout << "deep tuple: "
              << std::make_tuple("Hello", 0.1,
                                 std::make_tuple(1, 2, 3, "four", 5.5), 'Z')
              << std::endl;

    auto t1 = std::make_tuple(("ax"_, 5), ("by"_, 3.4),
                              ("str1"_, std::string("hello world")),
                              ("str2"_, "blabla2"));
    std::cout << "named tuple: " << t1 << '\n';
    auto deep1 = std::make_tuple(("a1"_, 33), "deeptuple"_, t1);
    std::cout << "deep deep 1: " << deep1 << '\n';

    auto tx = std::make_tuple(("r"_, 5), ("i"_, 1));
    std::cout << "tx two int: " << tx << " sizeof(tx): " << sizeof(tx) << '\n';
    std::cout << "tx : " << std::get<0>(tx) << " sizeof(tx): " << sizeof(tx)
              << '\n';
    (int&)std::get<0>(tx) = 3;
    std::cout << "tx two int: " << tx << " sizeof(tx): " << sizeof(tx) << '\n';

    auto onevalue = std::make_tuple(("field1"_, 2.1));
    std::cout << "one value named tuple: " << onevalue << '\n';
    auto nt1 = nvt::named_tuple{("fieldX"_, 123), ("fieldY"_, "test me")};

    std::cout << "named tuple: " << nt1 << '\n';

    std::cout << "n1.get_index<>(): " << nt1.get_index<decltype("fieldX"_)>()
              << '\n';

    std::cout << nt1["fieldX"_] << '\n';
    std::cout << nt1["fieldY"_] << '\n';

    auto ttree = nvt::named_tuple{
        ("f1"_, 123),
        ("subntup"_,
         nvt::named_tuple{("s1"_, 345),
                          ("s2"_, 564)}),  // FIXME: named tuple as field
        ("subtup"_,
         std::make_tuple(("s1"_, 345), ("s2"_, 564))),  // works just fine :-)
        ("f3"_, "test me")};
    std::cout << "tree: " << ttree << '\n';

    auto ft = nvt::named_tuple{("x"_, nvt::named_tuple{("y"_, 1)})};
    std::cout << "ft " << ft << '\n';
    return 0;
}
