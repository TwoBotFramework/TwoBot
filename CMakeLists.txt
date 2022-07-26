cmake_minimum_required(VERSION 3.19)
project(TwoBot)

set(CMAKE_CXX_STANDARD 17)


add_library(TwoBot STATIC  src/twobot.hh src/twobot.cc)

add_executable(TwoBot-demo demo/main.cpp)