cmake_minimum_required(VERSION 2.4)
project(infect)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

set(SOURCE_FILES
        graph.c
        graph.h
        main.c
        infect.c
        infect.h
        parameters.h)

add_executable(infect ${SOURCE_FILES})

target_link_libraries(infect ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(infect m)

