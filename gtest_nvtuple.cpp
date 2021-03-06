
#include <named_tuple.h>
#include <iostream>
#include <sstream>

#include <gtest/gtest.h>

namespace nvt = nvtuple_ns;

TEST(NamedValueTuple, NamedTypes) {
    std::stringstream stst;
    stst << "field1"_.str() << '\n';
    stst << "field1"_;
    EXPECT_EQ(stst.str(), "field1\nnamed_type: field1");

    EXPECT_TRUE((std::is_same<decltype("abc"_),
                              nvtuple_ns::named_type<'a', 'b', 'c'> >::value));
}

NVT_FIELD_TYPE("age"_, float)

TEST(NamedValueTuple, NamedValue) {
    std::stringstream stst;
    stst << ("field1"_, 1) << '\n';
    stst << ("field2"_, 1.2) << '\n';
    stst << ("field3"_, "string value") << '\n';
    EXPECT_EQ(stst.str(), "field1: 1\nfield2: 1.2\nfield3: \"string value\"\n");
    EXPECT_TRUE(
        (std::is_same<
            decltype(("z"_, 1.2)),
            nvtuple_ns::named_value<double, const decltype("z"_)> >::value));
    // auto z2 = ("age"_, 14);
    EXPECT_TRUE(
        (std::is_same<
            decltype(("age"_, 1)),
            nvtuple_ns::named_value<float, const decltype("age"_)> >::value));
    EXPECT_TRUE((std::is_same<
                 decltype(("g"_, 1)),
                 nvtuple_ns::named_value<int, const decltype("g"_)> >::value));
    EXPECT_TRUE(
        (std::is_same<
            decltype(("g"_, 1U)),
            nvtuple_ns::named_value<unsigned, const decltype("g"_)> >::value));
    EXPECT_TRUE(
        (std::is_same<decltype(("g"_, "test string value")),
                      nvtuple_ns::named_value<std::string,
                                              const decltype("g"_)> >::value));
    // auto z = ~"age"_;
    EXPECT_TRUE(
        (std::is_same<
            decltype((~"age"_)),
            nvtuple_ns::named_value<float, const decltype("age"_)> >::value));

    //    EXPECT_TRUE(
    //    (std::is_same<
    //        decltype(d1),
    //    nvtuple_ns::named_value<int, const decltype("f"_)> >::value));
}

TEST(NamedValueTuple, NamedValue2) {
    std::stringstream stst;
    auto d1 = ("f"_, 2);
    stst << d1;
    EXPECT_EQ(stst.str(), "f: 2");

    EXPECT_TRUE((std::is_same<decltype("a"_ + "b"_), decltype("ab"_)>::value));
}

TEST(NamedValueTuple, Tuple) {
    std::stringstream stst;
    auto t = nvt::named_tuple{("a"_, 123)};
    stst << t;
    // std::cerr << t << stst.str() << '\n';
    EXPECT_EQ(stst.str(), "(a: 123)");
}

TEST(NamedValueTuple, TupleInA_Tuple) {
    std::stringstream stst;
    auto t = nvt::named_tuple{("a"_, 123),
                              ("b"_, nvt::named_tuple{("x"_, "test me")})};
    stst << t;
    EXPECT_EQ(stst.str(), "(a: 123, b: (x: \"test me\"))");
}

TEST(NamedValueTuple, ValuesNames) {
    auto t = nvt::named_tuple{("a"_, 1), ("b"_, 2.0), ("c"_, "the world")};

    EXPECT_TRUE(std::string("a") == t.names()[0] &&
                std::string("b") == t.names()[1] &&
                std::string("c") == t.names()[2]);
}

TEST(NamedValueTuple, TupleDeepMove) {
    std::stringstream stst1;
    std::stringstream stst1empty;
    std::stringstream stst2;

    auto t1 = nvt::named_tuple{("i"_, 23), ("a"_, "a string to be moved"),
                               ("b"_, nvt::named_tuple{("x"_, "another one")})};

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
              "(i: 23, a: \"a string to be moved\", b: (x: \"another one\"))");
    EXPECT_EQ(stst1empty.str(), "(i: 23, a: \"\", b: (x: \"\"))");
    EXPECT_EQ(stst2.str(),
              "(i: 23, a: \"a string to be moved\", b: (x: \"another one\"))");
}

NVT_FIELD_TYPE("name"_, std::string)
NVT_FIELD_TYPE("uid"_, uint64_t)

NVT_FIELD_TYPE("pid"_, pid_t)
NVT_FIELD_TYPE("dbl"_, double)

using person_t =
    decltype(nvt::named_tuple(~"name"_, ~"uid"_, ~"age"_, ("xyz"_, 3.6)));

TEST(NamedValueTuple, DefaultTypes) {
    std::stringstream strm;

    auto t1 = nvt::named_tuple{("dbl"_, 3)};
    strm << std::fixed << std::setprecision(3);
    strm << t1;
    EXPECT_EQ(strm.str(), "(dbl: 3.000)");

    person_t pr1;
    pr1["name"_] = "John D.";
    strm.str("");
    strm << pr1;
    EXPECT_EQ(strm.str(),
              "(name: \"John D.\", uid: 0, age: 0.000, xyz: 0.000)");
}

std::string funcA(const decltype(nvt::named_tuple{("x"_, 1), ("y"_, 2)}) args =
                      nvt::named_tuple{("x"_, 1), ("y"_, 2)}) {
    std::stringstream strm;
    strm << args;
    // std::cerr << "funcA: " << args << '\n';
    return strm.str();
}

TEST(NamedValueTuple, ArgumentsAsA_Tuple) {
    EXPECT_EQ(funcA(), "(x: 1, y: 2)");
    // EXPECT_EQ(funcA(("x"_=5)), "(x: 5, y: 2)"); -- does not work as the
    // default argument is not being used, see funcB()
    EXPECT_EQ(funcA(("x"_, 5)), "(x: 5, y: 0)");
    EXPECT_EQ(funcA({("y"_, 10), ("x"_, 5)}), "(x: 5, y: 10)");
}

template<typename... NV>
std::string funcB(const NV... v) {
    auto ma = nvt::named_tuple{("x"_, 1), ("y"_, 2)};
    (..., (ma << v));

    std::stringstream strm;
    strm << ma;
    // std::cerr << "funcB: " << ma << '\n';
    return strm.str();
}

TEST(NamedValueTuple, NamedValuesArguments) {
    EXPECT_EQ(funcB(), "(x: 1, y: 2)");
    EXPECT_EQ(funcB(("x"_, 5)), "(x: 5, y: 2)");
    EXPECT_EQ(funcB(("y"_, 20), ("x"_, -10)), "(x: -10, y: 20)");
    EXPECT_EQ(funcB(("y"_, 21), ("x"_, -10)), "(x: -10, y: 21)");
}

template<typename... NV>
std::string funcC(NV&&... v) {
    auto ma = nvt::named_tuple{("x"_, 1), ("y"_, 2), ("z"_, "default string")};
    (..., (ma << std::move(std::forward<NV>(v))));

    std::stringstream strm;
    strm << ma;
    // std::cerr << "funcC: " << ma << '\n';
    return strm.str();
}

TEST(NamedValueTuple, NamedValuesArgumentsMove) {
    EXPECT_EQ(funcC(), "(x: 1, y: 2, z: \"default string\")");
    EXPECT_EQ(funcC(("x"_, 5)), "(x: 5, y: 2, z: \"default string\")");
    EXPECT_EQ(funcC(("y"_, 20), ("x"_, -10)),
              "(x: -10, y: 20, z: \"default string\")");
    EXPECT_EQ(funcC(("y"_, 21), ("x"_, -10)),
              "(x: -10, y: 21, z: \"default string\")");
    EXPECT_EQ(funcC(("z"_, "moved out string")),
              "(x: 1, y: 2, z: \"moved out string\")");

    auto x1 = ("z"_, "to be empty");
    EXPECT_EQ(funcC(x1), "(x: 1, y: 2, z: \"to be empty\")");
    EXPECT_EQ(funcC(x1), "(x: 1, y: 2, z: \"\")");

    auto x2 = ("z"_, "to be empty");
    EXPECT_EQ(funcC(std::move(x2)), "(x: 1, y: 2, z: \"to be empty\")");
    EXPECT_EQ(funcC(x2), "(x: 1, y: 2, z: \"\")");
}

template<typename... NV>
inline typename std::enable_if<(sizeof...(NV) == 0), std::string>::type funcD(
    NV&&...) {
    return {};
}

template<typename... NV>
inline typename std::enable_if<(sizeof...(NV) > 0), std::string>::type funcD(
    NV&&... v) {
    auto ma = nvt::named_tuple<NV...>(std::move(v)...);

    std::stringstream strm;
    strm << ma;
    // std::cerr << "funcD: " << ma << '\n';
    return strm.str();
}

template<typename... NV>
inline typename std::enable_if<(sizeof...(NV) > 0), std::string>::type funcD(
    const NV&... v) {
    auto ma = nvt::named_tuple<NV...>(v...);

    std::stringstream strm;
    strm << ma;
    // std::cerr << "funcD: " << ma << '\n';
    return strm.str();
}

TEST(NamedValueTuple, FuncD_toString) {
    // auto empty = nvt::named_tuple<>();
    // std::cerr << "empty1: " << empty << '\n';
    // EXPECT_EQ(funcD(), "()");
    // EXPECT_EQ(funcD(std::move(empty)), "()");
    // EXPECT_EQ(funcD(empty), "()");  // FIXME:
    // EXPECT_EQ(funcD("a"_ , 3.1), "(a: 3.1)");
    EXPECT_EQ(funcD(("a"_, 3), ("x"_, 3.6), ("z"_, "abc")),
              "(a: 3, x: 3.6, z: \"abc\")");
}
