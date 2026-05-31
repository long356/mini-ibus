cmake_minimum_required(VERSION 3.16)
project(mini_ibus LANGUAGES CXX)

# C++17 standard, no exceptions, no RTTI
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warning flags (compiler-specific)
add_compile_options(
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic>
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# No exceptions, no RTTI (compiler-specific)
add_compile_options(
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fno-exceptions -fno-rtti>
  $<$<CXX_COMPILER_ID:MSVC>:/EHs-c- /GR->
)

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
