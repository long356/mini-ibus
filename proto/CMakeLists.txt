# proto/CMakeLists.txt - Protobuf message definitions

if(Protobuf_FOUND)
  protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    message.proto
    topology.proto
  )

  add_library(minibus_proto STATIC ${PROTO_SRCS} ${PROTO_HDRS})
  target_link_libraries(minibus_proto PUBLIC protobuf::libprotobuf)
  target_include_directories(minibus_proto PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
  )
endif()
