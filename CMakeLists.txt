cmake_minimum_required(VERSION 3.13)
project(untitled C)

set(CMAKE_C_STANDARD 99)

find_package (Threads)

add_executable(untilted Main.c)

target_link_libraries(untilted rt)
target_link_libraries (untilted ${CMAKE_THREAD_LIBS_INIT})