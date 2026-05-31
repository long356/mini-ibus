# tests/unit/CMakeLists.txt - Unit test targets

if(GTest_FOUND OR GTEST_FOUND)
  add_executable(errors_test
    errors_test.cc
  )
  target_link_libraries(errors_test PRIVATE minibus_core GTest::GTest GTest::Main)
  target_include_directories(errors_test PRIVATE ${CMAKE_SOURCE_DIR})
  add_test(NAME errors_test COMMAND errors_test)

  add_executable(address_test
    address_test.cc
  )
  target_link_libraries(address_test PRIVATE minibus_core GTest::GTest GTest::Main)
  target_include_directories(address_test PRIVATE ${CMAKE_SOURCE_DIR})
  add_test(NAME address_test COMMAND address_test)

  add_executable(logging_test
    logging_test.cc
  )
  target_link_libraries(logging_test PRIVATE minibus_core GTest::GTest GTest::Main)
  target_include_directories(logging_test PRIVATE ${CMAKE_SOURCE_DIR})
  add_test(NAME logging_test COMMAND logging_test)

  if(Protobuf_FOUND)
    add_executable(proto_test
      proto_test.cc
    )
    target_link_libraries(proto_test PRIVATE minibus_core minibus_proto GTest::GTest GTest::Main)
    target_include_directories(proto_test PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/proto)
    add_test(NAME proto_test COMMAND proto_test)
  endif()
endif()
