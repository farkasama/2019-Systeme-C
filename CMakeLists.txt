cmake_minimum_required(VERSION 3.13)
project(untitled C)

set(CMAKE_C_STANDARD 99)

find_package(Threads)

add_executable(untitled Main.c)

target_link_libraries(untitled rt)
target_link_libraries(untitled ${CMAKE_THREAD_LIBS_INIT})