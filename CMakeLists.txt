cmake_minimum_required(VERSION 3.26)
project(SimpleAStar C)

set(CMAKE_C_STANDARD 23)

add_subdirectory(vendor)
add_executable(SimpleAStar main.c)
target_link_libraries(SimpleAStar raylib)