
// See README.md for more details about named_tuple.

// Author: Erez Strauss <erez@erezstrauss.com>

#include <named_tuple.h>
#include <iostream>

//using namespace named_tuple;  // To be implemented.

// Example 1 shows named tuple creation and printing
void example_1()
{
    auto t1 = std::make_tuple(("fieldA"_ = 123), ("B"_ = 100.111), ("lastone"_ = 999));
    std::cout << "example 1:\n  " << t1 << '\n';
}

// Example 2 shows named_tuple and field modification using operator[]
void example_2()
{
    auto t2 = NamedTuple {("fieldA"_ = 123), ("B"_ = 100.111), ("lastone"_ = 999) };
    std::cout << "example 2:\n  " << t2 << '\n';
    t2["fieldA"_] = 321;
    std::cout << "  changed fieldA: " << t2 << '\n';
}


template<typename ... TS>
class Data : public std::tuple<std::decay_t<TS>...> {
public:
  Data(TS&& ... ts) noexcept
    : std::tuple<std::decay_t<TS>...>(std::forward<TS>(ts)...) {}
};

// Example 3 - shows simple tuple printing.
void example_3()
{
  Data<std::string,double> simple_tuple ( "hello", 555 );

  std::cout << "example 3:\n  Data: " << simple_tuple << '\n';

}

// Example 4 - shows Named types and Named values
void example_4()
{
    auto abc = "abc"_;
    std::cout << "example 4:\n  abc: " << abc << '\n';
    auto nv = NamedValue<int, decltype("xyz"_)> { 42 };
    std::cout << "   " << nv << '\n';
}

int main()
{

    example_1();
    example_2();
    example_3();
    example_4();

    auto z1 = "abc"_;
    std::cout << "deep tuple: " << std::make_tuple("Hello", 0.1, std::make_tuple(1,2,3,"four",5.5), 'Z') << std::endl;
  
    auto t1 = std::make_tuple(("ax"_ = 5), ("by"_ = 3.4), ("str1"_=std::string("hello world")), ("str2"_="blabla2"));
    std::cout << "named tuple: " << t1 << '\n';
    auto deep1 = std::make_tuple(("a1"_=33), "deeptuple"_ = t1);
    std::cout << "deep deep 1: " << deep1 << '\n';


    auto tx = std::make_tuple(("r"_ = 5), ("i"_ = 1));
    std::cout << "tx two int: " << tx << " sizeof(tx): " << sizeof(tx) << '\n';
    std::cout << "tx : " << std::get<0>(tx) << " sizeof(tx): " << sizeof(tx) << '\n';
    (int&)std::get<0>(tx) = 3;
    std::cout << "tx two int: " << tx << " sizeof(tx): " << sizeof(tx) << '\n';

    auto onevalue = std::make_tuple(("field1"_ = 2.1)) ;
    std::cout << "one value named tuple: " << onevalue << '\n';
    auto nt1 = NamedTuple { ("fieldX"_ = 123), ("fieldY"_ = "test me") };
    //auto nt1 = NamedTuple { ("fieldX"_ = 123) };

    //std::cout << simple_tuple(onevalue) << '\n';
    std::cout << "named tuple: " << nt1 << '\n';
    // std::cout << "nt1.find_field_index : " << nt1.named_type_find< NamedValue<int, decltype("fieldX"_)>, NamedValue<int, decltype("fieldX"_)> > () << '\n';
    std::cout << "nt1.find_field_index : " << nt1.named_type_find< decltype("fieldX"_), NamedValue<int, decltype("fieldX"_)> > () << '\n';

    std::cout << "n1.get_index<>(): " << nt1.get_index< decltype("fieldX"_) >() << '\n';

    std::cout << (nt1.get<decltype("fieldX"_)>() ) << '\n';
    std::cout << nt1["fieldX"_] << '\n';
    std::cout << nt1["fieldY"_] << '\n';

    return 0;
}
