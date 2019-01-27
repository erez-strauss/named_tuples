cmake_minimum_required(VERSION 3.11)
project(named_tuple)

include_directories(.)

set(CMAKE_CXX_STANDARD 17)

add_executable(named_tuple_example    named_tuple_example.cpp named_tuple.h)
