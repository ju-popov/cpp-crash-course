cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(CppCrashCourse)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CPP_STANDARD 20)
set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Catch2 3 CONFIG REQUIRED)

add_executable(ch4 src/ch4.cpp)
add_executable(ch5 src/ch5.cpp)
add_executable(ch6.1 src/ch6.1.cpp)
add_executable(ch6.2 src/ch6.2.cpp)

add_executable(ch10 src/ch10.cpp)
target_link_libraries(ch10 PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
