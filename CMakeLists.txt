cmake_minimum_required(VERSION 3.15)
project(FF14RotationOptimizer)

set(CMAKE_CXX_STANDARD 17)

add_executable(FF14Rot
    actions.hpp
    actions.cpp
    jobstate.hpp
    jobstate.cpp
    main.cpp)
