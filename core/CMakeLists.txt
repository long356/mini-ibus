# core/CMakeLists.txt - Core library (minibus_core)

add_library(minibus_core STATIC
  errors.h
  errors.cc
  address.h
  address.cc
)

target_include_directories(minibus_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
