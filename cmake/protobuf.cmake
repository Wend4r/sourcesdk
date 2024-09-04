if(NOT PROTOBUF_DIR)
	message(FATAL_ERROR "PROTOBUF_DIR is empty")
endif()

set(PROTOBUF_NAME "protobuf")
set(PROTOBUF_LIBPPROTOBUF_LITE_NAME "libprotobuf-lite")
set(PROTOBUF_LIBPPROTOBUF_NAME "libprotobuf")
set(PROTOBUF_LIBPROTOC_NAME "libprotoc")
set(PROTOBUF_PROTOC_NAME "protoc")

set(${PROTOBUF_NAME}_INSTALL OFF CACHE BOOL "Install protobuf binaries and files")
set(${PROTOBUF_NAME}_BUILD_PROTOC_BINARIES OFF CACHE BOOL "Build libprotoc and protoc compiler")
set(${PROTOBUF_NAME}_BUILD_TESTS OFF CACHE BOOL "Build tests")

add_subdirectory(${PROTOBUF_DIR} ${PROTOBUF_BINARY_DIR})
