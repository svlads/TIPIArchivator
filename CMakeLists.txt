cmake_minimum_required(VERSION 3.16)
project(arch)

set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_CXX_FLAGS "-fsanitize=address -fsanitize=undefined -DGLIBCXX_DEBUG")

add_executable(arch main.cpp PrefixTree.cpp Queue.hpp Code.cpp Stack.cpp)