cmake_minimum_required(VERSION 3.12)
project(HelloWorld)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(ch4 src/ch4.cpp)
add_executable(ch5 src/ch5.cpp)
add_executable(ch6.1 src/ch6.1.cpp)
add_executable(ch6.2 src/ch6.2.cpp)
add_executable(ch10 src/ch10.cpp)