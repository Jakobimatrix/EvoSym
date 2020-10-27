
cmake_minimum_required(VERSION 3.10.2)
project(evosym VERSION 1.0.0)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

#add_subdirectory(src/utils)
add_subdirectory(src/Settings)
add_subdirectory(src/homography)
add_subdirectory(src/timer)
add_subdirectory(src/executable)
