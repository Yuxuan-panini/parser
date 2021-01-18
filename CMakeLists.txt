cmake_minimum_required(VERSION 3.12)
project(Parser)

set(CMAKE_CXX_STANDARD 11)

add_executable(Parser main.cpp Symbol.h Symbol.cpp LL1.h LL1.cpp TypeDefs.h)