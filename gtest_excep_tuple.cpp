
#include <exception_tuple.h>
#include <named_tuple.h>

#include <iostream>
#include <sstream>

#include <gtest/gtest.h>

#include <iterator>
#include <regex>
#include <string>

namespace nvt = nvtuple_ns;

TEST(ExceptionTuples, BasicException) {
    std::stringstream ostr;
    std::regex pathex("/.*/");

    try {
        throw NVT_EXCEPTION();

    } catch (std::exception& e) {
        ostr << "got exception: " << e.what();
    }
    auto res = std::regex_replace(ostr.str(), pathex, "");
    EXPECT_EQ(
        res,
        "got exception: (file: \"gtest_excep_tuple.cpp\", line: 21, func: "
        "\"virtual void ExceptionTuples_BasicException_Test::TestBody()\")");
}

TEST(ExceptionTuples, ExceptionArgs) {
    std::stringstream ostr;
    std::regex pathex("/.*/");

    try {
        throw NVT_EXCEPTION(("iarg"_, 123), ("darg"_, 3.25),
                            ("sarg"_, "this is an example"));
    } catch (std::exception& e) {
        ostr << "got exception: " << e.what();
    }
    auto res = std::regex_replace(ostr.str(), pathex, "");
    EXPECT_EQ(
        res,
        "got exception: (file: \"gtest_excep_tuple.cpp\", line: 39, func: "
        "\"virtual void ExceptionTuples_ExceptionArgs_Test::TestBody()\", "
        "iarg: 123, darg: 3.25, sarg: \"this is an example\")");
}
