# minimum CMake version to run this file
cmake_minimum_required(VERSION 3.12)

# setting the path for the cmake toolchain file
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

# name of the project
project(CppCrashCourse)

# enabling export of compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# setting the CMake C++ standard to be used
set(CPP_STANDARD 20)
set(CMAKE_CXX_STANDARD ${CPP_STANDARD})

# if the specified CMake version cannot be found, make it a fatal error
set(CMAKE_CXX_STANDARD_REQUIRED True)

# locating the required Catch2, GTest and Boost packages (version 3 and up for Catch2) for the specified components and configurations
find_package(Catch2 3 CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# enabling testing for the current directory and below
enable_testing()

# defining a function to add the specified executable target and link necessary libraries to it
function(add_executable_and_link_libraries TARGET_NAME SOURCE_FILE LIBRARIES)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    target_link_libraries(${TARGET_NAME} PRIVATE ${LIBRARIES})
endfunction()

add_executable_and_link_libraries("ch04" "src/ch04.cpp" "")

add_executable_and_link_libraries("ch05" "src/ch05.cpp" "")

add_executable_and_link_libraries("ch06.1" "src/ch06.1.cpp" "")

add_executable_and_link_libraries("ch06.2" "src/ch06.2.cpp" "")

add_executable_and_link_libraries("ch10.1" "src/ch10.1.cpp" Catch2::Catch2 Catch2::Catch2WithMain)

add_executable_and_link_libraries("ch10.2" "src/ch10.2.cpp" GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(NAME GTestTestInMain COMMAND ch10.2)

add_executable_and_link_libraries("ch10.3" "src/ch10.3.cpp" Boost::unit_test_framework)
add_test(NAME BoostTestTestInMain COMMAND ch10.3)
