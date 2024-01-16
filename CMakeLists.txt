cmake_minimum_required(VERSION 3.27)

project(skeleton_cpp_mqtt_cli)

set(CMAKE_CXX_STANDARD 14)

add_executable(skeleton_cpp_mqtt_cli main.cpp)

target_link_libraries(skeleton_cpp_mqtt_cli PRIVATE ws2_32)
