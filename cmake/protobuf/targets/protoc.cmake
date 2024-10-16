if(NOT ${${PROTOBUF_NAME}_BUILD_PROTOC_BINARIES})
	return()
endif()

if(NOT PROTOBUF_PROTOC_NAME)
	message(FATAL_ERROR "PROTOBUF_PROTOC_NAME is empty")
endif()

set_target_properties(${PROTOBUF_PROTOC_NAME} PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

if(WINDOWS)
	if(SOURCESDK_MSVC_RUNTIME_LIBRARY)
		set_target_properties(${PROTOBUF_PROTOC_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	endif()
elseif(MACOS)
	set_target_properties(${PROTOBUF_PROTOC_NAME} PROPERTIES OSX_ARCHITECTURES "x86_64")
endif()

target_compile_options(${PROTOBUF_PROTOC_NAME} PRIVATE ${PLATFORM_COMPILE_OPTIONS})
target_link_options(${PROTOBUF_PROTOC_NAME} PRIVATE ${PLATFORM_LINK_OPTIONS})

target_compile_definitions(${PROTOBUF_PROTOC_NAME} PRIVATE ${PLATFORM_COMPILE_DEFINITIONS})
target_include_directories(${PROTOBUF_PROTOC_NAME} PRIVATE ${PLATFORM_INCLUDE_DIRS})
