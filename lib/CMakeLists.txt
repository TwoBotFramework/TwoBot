add_subdirectory(json)
add_subdirectory(httplib)
add_library(asio2 INTERFACE)
target_include_directories(asio2 INTERFACE asio2/include)