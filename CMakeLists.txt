cmake_minimum_required(VERSION 4.1)
project(silt)

set(CMAKE_CXX_STANDARD 23)

add_executable(silt main.cpp
        Event.hpp
        ResourceManager.hpp)
