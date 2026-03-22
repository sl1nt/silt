cmake_minimum_required(VERSION 4.1)
project(silt)

set(CMAKE_CXX_STANDARD 23)

find_package(raylib REQUIRED)

add_executable(silt main.cpp
        Event.hpp
        ResourceManager.hpp)

target_link_libraries(silt PRIVATE raylib)
