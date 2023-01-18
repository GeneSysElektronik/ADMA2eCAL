set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

project(debug_interpreter)

find_package(eCAL REQUIRED)

add_executable(${PROJECT_NAME})

set(source_files
  src/main.cpp
  src/fill_adma_proto_object.h
  src/fill_adma_proto_object.cpp
  src/get_value.h
)

set(proto_files
  ${CMAKE_CURRENT_SOURCE_DIR}/proto/header.proto
  ${CMAKE_CURRENT_SOURCE_DIR}/proto/Adma.proto
)

target_sources(${PROJECT_NAME} PRIVATE
  ${source_files}
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    protobuf::libprotobuf
    eCAL::core
)

protobuf_target_cpp(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/proto
  ${proto_files}
)

include(sourcetree.cmake)
create_source_tree(${source_files})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
