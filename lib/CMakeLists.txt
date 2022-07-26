add_subdirectory(json)
add_subdirectory(httplib)
add_library(websocketpp INTERFACE)
target_include_directories(websocketpp INTERFACE ./websocketpp/websocketpp)