cmake_minimum_required(VERSION 3.19)
project(TwoBot)

set(CMAKE_CXX_STANDARD 17)


add_library(TwoBot STATIC
        src/twobot.hh
        src/twobot.cc
        src/apiset.cc
)

target_include_directories(TwoBot PUBLIC src/)

add_subdirectory(lib)
target_link_libraries(TwoBot PUBLIC nlohmann_json::nlohmann_json brynet httplib::httplib)
target_compile_definitions(TwoBot PUBLIC _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING)

add_executable(TwoBot-demo demo/main.cpp)
target_link_libraries(TwoBot-demo TwoBot)

if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
endif()