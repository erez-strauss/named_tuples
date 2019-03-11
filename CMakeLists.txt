cmake_minimum_required(VERSION 3.11)
project(named_tuple)

include_directories(.)

# set(CMAKE_CXX_STANDARD 20)
set( CMAKE_CXX_FLAGS "-std=c++2a -W -Wall -Wshadow -Wpedantic -ggdb3 -O3 " )

add_executable(named_tuple_example    named_tuple_example.cpp named_tuple.h)

add_executable(exception_tuple_example    exception_tuple_example.cpp exception_tuple.h named_tuple.h)

add_executable(gtest_nvtuple    gtest_nvtuple.cpp named_tuple.h)
target_link_libraries(gtest_nvtuple  LINK_PRIVATE pthread gtest_main gtest)

add_executable(gtest_excep_tuple    gtest_excep_tuple.cpp exception_tuple.h named_tuple.h)
target_link_libraries(gtest_excep_tuple  LINK_PRIVATE pthread gtest_main gtest)
