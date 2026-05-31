# core/CMakeLists.txt - Core library (minibus_core)

add_library(minibus_core STATIC
  errors.h
  errors.cc
  address.h
  address.cc
  logging.h
  logging.cc
  message_codec.h
  message_codec.cc
)

target_include_directories(minibus_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

if(MINIBUS_CXX_FLAGS)
  target_compile_options(minibus_core PRIVATE ${MINIBUS_CXX_FLAGS})
endif()

if(spdlog_FOUND)
  target_compile_definitions(minibus_core PUBLIC MINIBUS_HAS_SPDLOG)
  target_link_libraries(minibus_core PUBLIC spdlog::spdlog)
endif()
