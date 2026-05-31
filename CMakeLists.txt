cmake_minimum_required(VERSION 3.16)
project(mini_ibus LANGUAGES CXX)

# C++17 standard, no exceptions, no RTTI
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warning flags (compiler-specific)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti)
elseif(MSVC)
  add_compile_options(/W4 /EHs-c- /GR-)
endif()

# External dependencies (optional at this stage)
find_package(Protobuf QUIET)
find_package(spdlog QUIET)
find_package(GTest QUIET)
find_package(nlohmann_json QUIET)

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
