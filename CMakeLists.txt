cmake_minimum_required(VERSION 3.13)
project(http_proxy)

set(CMAKE_CXX_STANDARD 14)

add_executable(http_proxy main.cpp Request.cpp Request.h)