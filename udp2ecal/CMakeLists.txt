
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

project(adma2ecal)

find_package(asio REQUIRED)
find_package(eCAL REQUIRED)

add_executable(adma2ecal)

set(source_files
  src/main.cpp
  src/adma_sender.h
  src/adma_sender.cpp
  src/udp_data_receiver.h
  src/udp_data_receiver.cpp
)

set(proto_files
  ${CMAKE_CURRENT_SOURCE_DIR}/proto/header.proto
  ${CMAKE_CURRENT_SOURCE_DIR}/proto/Adma.proto
)

target_sources(adma2ecal PRIVATE
  ${source_files}
)

target_include_directories(adma2ecal PRIVATE src)

target_link_libraries(adma2ecal
  PRIVATE
    protobuf::libprotobuf
    eCAL::core
    $<BUILD_INTERFACE:asio::asio>
)

protobuf_target_cpp(adma2ecal
  ${CMAKE_CURRENT_SOURCE_DIR}/proto
  ${proto_files}
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        ASIO_STANDALONE
        ASIO_DISABLE_VISIBILITY
        _WIN32_WINNT=0x0601
)

include(sourcetree.cmake)
create_source_tree(${source_files})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
