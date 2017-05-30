cmake_minimum_required(VERSION 2.0)
project(infect)

set(CMAKE_CXX_STANDARD 11)

find_package (Threads)
set(SOURCE_FILES
        graph.c
        graph.h
        main.c infect.c infect.h parameters.h)
set(CMAKE_CXX_FLAGS " -g")

add_executable(infect ${SOURCE_FILES} infect.c infect.h parameters.h)

target_link_libraries (infect ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (infect m)

