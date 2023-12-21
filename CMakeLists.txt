# Defines the minimum CMake version required for the build
cmake_minimum_required(VERSION 3.12)

# Sets the CMake toolchain file and project name
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
project(CppCrashCourse)

# Enables compiler commands export and defines C++ standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CPP_STANDARD 20)
set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Adds executable targets for each source file
add_executable(ch4 src/ch04.cpp)
add_executable(ch5 src/ch05.cpp)
add_executable(ch6.1 src/ch06.1.cpp)
add_executable(ch6.2 src/ch06.2.cpp)

# Configuration for Catch2 testing framework
find_package(Catch2 3 CONFIG REQUIRED)
add_executable(ch10.1 src/ch10.1.cpp)
# Linking of Catch2 libraries to the executable target
target_link_libraries(ch10.1 PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)

# Enable CMake testing modules
enable_testing()

# Configuration for GTest testing framework
find_package(GTest CONFIG REQUIRED)
add_executable(ch10.2 src/ch10.2.cpp)
# Linking of GTest libraries to the executable target
target_link_libraries(ch10.2 PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
# Define the test to be run by CMake test driver
add_test(NAME GTestTestInMain COMMAND ch10.2)

# Configuration for Boost testing framework
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
add_executable(ch10.3 src/ch10.3.cpp)
# Linking of Boost libraries to the executable target
target_link_libraries(ch10.3 PRIVATE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
# Define the test to be run by Boost test driver
add_test(NAME BoostTestTestInMain COMMAND ch10.3)
