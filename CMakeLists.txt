cmake_minimum_required(VERSION 3.22)
project(ex2)

#set(CMAKE_C_STANDARD 99)

add_executable(ex2 ThreadHandler.cpp
        test0_sanity.cpp
        uthreads.h
        uthreads.cpp
        Thread.cpp)
