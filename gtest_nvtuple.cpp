
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
    // t2["i"_] = std::move (t1["i"_]);  //Does not work yet.
    // t2["a"_] = std::move (t1["a"_]);  //Does not work yet.
    // t2["b"_] = std::move (t1["b"_]);  //Does not work yet.

    stst2 << t2;
    // std::cerr << "  copy  t2: " << t2 << '\n';
    // std::cerr << "  stst2 t2: " << stst2.str() << '\n';

    stst1empty << t1;
    // std::cerr << "  empty t1: " << t1 << '\n';
    // std::cerr << "  stst1et1: " << stst1empty.str() << '\n';

    // decltype(t1)  t2 ( std::move (t) );  //FIXME: does not work yet.
    // decltype(t1)  t2; t2 = std::move (t); //FIXME: does not work also.
    // t2["a"_] = std::move (t1["a"_]);  //Does not work yet.
    // t2["a"_] = t1["a"_];
    // std::cerr << "\nmoved out t: " << t << '\n';
    // std::cerr << "\nt2:          " << t2 << '\n';
    EXPECT_EQ(stst1.str(),
              "(i: 23, a: a string to be moved, b: (x: another one))");
    EXPECT_EQ(stst1empty.str(), "(i: 23, a: , b: (x: ))");
    EXPECT_EQ(stst2.str(),
              "(i: 23, a: a string to be moved, b: (x: another one))");
}
