# tests/unit/CMakeLists.txt - Unit test targets

if(GTest_FOUND OR GTEST_FOUND)
  add_executable(errors_test
    errors_test.cc
  )
  target_link_libraries(errors_test PRIVATE minibus_core GTest::GTest GTest::Main)
  target_include_directories(errors_test PRIVATE ${CMAKE_SOURCE_DIR})
  add_test(NAME errors_test COMMAND errors_test)
endif()
