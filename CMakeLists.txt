cmake_minimum_required(VERSION 3.16)
project(mini_ibus LANGUAGES CXX)

# C++17 standard, no exceptions, no RTTI
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags for project targets (not global, to avoid breaking deps)
set(MINIBUS_CXX_FLAGS "")
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(MINIBUS_CXX_FLAGS -Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti)
elseif(MSVC)
  set(MINIBUS_CXX_FLAGS /W4 /EHs-c- /GR-)
endif()

# External dependencies (optional at this stage)
find_package(Protobuf QUIET)
find_package(spdlog QUIET)
find_package(GTest QUIET)
find_package(nlohmann_json QUIET)

# Fallback: fetch GTest via FetchContent if not installed
if(NOT GTest_FOUND AND NOT GTEST_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  )
  FetchContent_MakeAvailable(googletest)
endif()

# Normalize GTest target names (FetchContent vs find_package)
if(TARGET GTest::gtest)
  # v1.11+ style
elseif(TARGET GTest::GTest)
  # Older find_package style
elseif(TARGET gtest)
  add_library(GTest::gtest ALIAS gtest)
  add_library(GTest::gtest_main ALIAS gtest_main)
endif()

# Testing
enable_testing()

# Subdirectories
add_subdirectory(proto)
add_subdirectory(core)
add_subdirectory(addons)
add_subdirectory(ibusd)
add_subdirectory(tools)
add_subdirectory(examples)
add_subdirectory(tests)
