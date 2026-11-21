if(NOT PROTOBUF_LIBPPROTOBUF_LITE_NAME)
	message(FATAL_ERROR "PROTOBUF_LIBPPROTOBUF_LITE_NAME is empty")
endif()

set_target_properties(${PROTOBUF_LIBPPROTOBUF_LITE_NAME} PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

if(WINDOWS)
	if(SOURCESDK_MSVC_RUNTIME_LIBRARY)
		set_target_properties(${PROTOBUF_LIBPPROTOBUF_LITE_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	endif()
endif()

target_compile_options(${PROTOBUF_LIBPPROTOBUF_LITE_NAME} PRIVATE ${PLATFORM_COMPILE_OPTIONS})

# Protobuf is third-party code: its warnings are not ours to fix, and -Wall from the platform options asks for them
if(NOT MSVC)
	target_compile_options(${PROTOBUF_LIBPPROTOBUF_LITE_NAME} PRIVATE -w)
endif()

target_link_options(${PROTOBUF_LIBPPROTOBUF_LITE_NAME} PRIVATE ${PLATFORM_LINK_OPTIONS})

target_compile_definitions(${PROTOBUF_LIBPPROTOBUF_LITE_NAME} PRIVATE ${PLATFORM_COMPILE_DEFINITIONS})
target_include_directories(${PROTOBUF_LIBPPROTOBUF_LITE_NAME} PRIVATE ${PLATFORM_INCLUDE_DIRS})
