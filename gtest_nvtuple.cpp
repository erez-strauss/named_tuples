
#include <named_tuple.h>
#include <iostream>
#include <sstream>

#include <gtest/gtest.h>

namespace nvt = nvtuple_ns;

TEST(NamedValueTuple, SimleTest1) { EXPECT_EQ(true, true); }

TEST(NamedValueTuple, Tuple) {
    std::stringstream stst;
    auto t = nvt::named_tuple{("a"_ = 123)};
    stst << t;
    // std::cerr << t << stst.str() << '\n';
    EXPECT_EQ(stst.str(), "(a: 123)");
}

TEST(NamedValueTuple, TupleInA_Tuple) {
    std::stringstream stst;
    auto t = nvt::named_tuple{("a"_ = 123),
                              ("b"_ = nvt::named_tuple{("x"_ = "test me")})};
    stst << t;
    // std::cerr << t << stst.str() << '\n';
    EXPECT_EQ(stst.str(), "(a: 123, b: (x: test me))");
}

TEST(NamedValueTuple, TupleDeepMove) {
    std::stringstream stst1;
    std::stringstream stst1empty;
    std::stringstream stst2;

    auto t1 =
        nvt::named_tuple{("i"_ = 23), ("a"_ = "a string to be moved"),
                         ("b"_ = nvt::named_tuple{("x"_ = "another one")})};

    stst1 << t1;
    // std::cerr << "  start t1: " << t1 << '\n';
    // std::cerr << "  stst1 t1: " << stst1.str() << '\n';

    // decltype(t1) t2 { std::move(t1) };  // Works fine.
    decltype(t1) t2;
    t2 = std::move(t1);  // also works.
    // t2["i"_] = std::move (t1["i"_]);  //works fine.
    // t2["a"_] = std::move (t1["a"_]);  //
    // t2["b"_] = std::move (t1["b"_]);  //

    stst2 << t2;
    // std::cerr << "  copy  t2: " << t2 << '\n';
    // std::cerr << "  stst2 t2: " << stst2.str() << '\n';

    stst1empty << t1;
    // std::cerr << "  empty t1: " << t1 << '\n';
    // std::cerr << "  stst1et1: " << stst1empty.str() << '\n';

    EXPECT_EQ(stst1.str(),
              "(i: 23, a: a string to be moved, b: (x: another one))");
    EXPECT_EQ(stst1empty.str(), "(i: 23, a: , b: (x: ))");
    EXPECT_EQ(stst2.str(),
              "(i: 23, a: a string to be moved, b: (x: another one))");
}

std::string funcA(const decltype(nvt::named_tuple{
                      ("x"_ = 1), ("y"_ = 2)}) args = nvt::named_tuple{
                      ("x"_ = 1), ("y"_ = 2)}) {
    std::stringstream strm;
    strm << args;
    // std::cerr << "funcA: " << args << '\n';
    return strm.str();
}

TEST(NamedValueTuple, ArgumentsAsA_Tuple) {
    EXPECT_EQ(funcA(), "(x: 1, y: 2)");
    // EXPECT_EQ(funcA(("x"_=5)), "(x: 5, y: 2)"); -- does not work as the
    // default argument is not being used, see funcB()
    EXPECT_EQ(funcA(("x"_ = 5)), "(x: 5, y: 0)");
    EXPECT_EQ(funcA({("y"_ = 10), ("x"_ = 5)}), "(x: 5, y: 10)");
}

template<typename... NV>
std::string funcB(const NV... v) {
    auto ma = nvt::named_tuple{("x"_ = 1), ("y"_ = 2)};
    (..., (ma << v));

    std::stringstream strm;
    strm << ma;
    // std::cerr << "funcB: " << ma << '\n';
    return strm.str();
}

TEST(NamedValueTuple, NamedValuesArguments) {
    EXPECT_EQ(funcB(), "(x: 1, y: 2)");
    EXPECT_EQ(funcB(("x"_ = 5)), "(x: 5, y: 2)");
    EXPECT_EQ(funcB(("y"_ = 20), ("x"_ = -10)), "(x: -10, y: 20)");
    EXPECT_EQ(funcB(("y"_ = 21), ("x"_ = -10)), "(x: -10, y: 21)");
}

template<typename... NV>
std::string funcC(NV&&... v) {
    auto ma =
        nvt::named_tuple{("x"_ = 1), ("y"_ = 2), ("z"_ = "default string")};
    (..., (ma << std::move(std::forward<NV>(v))));

    std::stringstream strm;
    strm << ma;
    // std::cerr << "funcC: " << ma << '\n';
    return strm.str();
}

TEST(NamedValueTuple, NamedValuesArgumentsMove) {
    EXPECT_EQ(funcC(), "(x: 1, y: 2, z: default string)");
    EXPECT_EQ(funcC(("x"_ = 5)), "(x: 5, y: 2, z: default string)");
    EXPECT_EQ(funcC(("y"_ = 20), ("x"_ = -10)),
              "(x: -10, y: 20, z: default string)");
    EXPECT_EQ(funcC(("y"_ = 21), ("x"_ = -10)),
              "(x: -10, y: 21, z: default string)");
    EXPECT_EQ(funcC(("z"_ = "moved out string")),
              "(x: 1, y: 2, z: moved out string)");

    auto x1 = ("z"_ = "to be empty");
    EXPECT_EQ(funcC(x1), "(x: 1, y: 2, z: to be empty)");
    EXPECT_EQ(funcC(x1), "(x: 1, y: 2, z: )");

    auto x2 = ("z"_ = "to be empty");
    EXPECT_EQ(funcC(std::move(x2)), "(x: 1, y: 2, z: to be empty)");
    EXPECT_EQ(funcC(x2), "(x: 1, y: 2, z: )");
}

template<typename... NV>
std::string funcD(NV&&... v) {
    auto ma = nvt::named_tuple<NV...>(std::forward<NV>(std::move(v))...);

    std::stringstream strm;
    strm << ma;
    // std::cerr << "funcD: " << ma << '\n';
    return strm.str();
}

TEST(NamedValueTuple, FuncD_toString) {
    auto empty = nvt::named_tuple<>();
    std::cerr << "empty1: " << empty << '\n';
    EXPECT_EQ(funcD(), "()");
    // EXPECT_EQ(funcD(std::move(empty)), "()");  //FIXME:
    // EXPECT_EQ(funcD(empty), "()");  //FIXME:
    EXPECT_EQ(funcD("a"_ = 3.1), "(a: 3.1)");
    EXPECT_EQ(funcD(("a"_ = 3), ("x"_ = 3.6), ("z"_ = "abc")),
              "(a: 3, x: 3.6, z: abc)");
}
