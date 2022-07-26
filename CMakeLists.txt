cmake_minimum_required(VERSION 3.19)
project(TwoBot)

set(CMAKE_CXX_STANDARD 17)


add_library(TwoBot STATIC
        src/twobot.hh
        src/twobot.cc
# 第三方库
        src/httplib.hh

)
target_include_directories(TwoBot PUBLIC src/)

add_executable(TwoBot-demo demo/main.cpp)
target_link_libraries(TwoBot-demo TwoBot)