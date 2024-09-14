cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

if(POLICY CMP0092)
	cmake_policy(SET CMP0092 NEW) # Don't add -W3 warning level by default.
endif()

project(sourcesdk
	VERSION 2.0.0
	DESCRIPTION "Source SDK"
	HOMEPAGE_URL "https://github.com/Wend4r/sourcesdk"
	LANGUAGES C CXX
)

string(REGEX REPLACE "^[^-]*-" "" PROJECT_NAME_SUBSTRING "${PROJECT_NAME}")
string(TOLOWER "${PROJECT_NAME_SUBSTRING}" PROJECT_NAME_LOWER)
string(TOUPPER "${PROJECT_NAME_SUBSTRING}" PROJECT_NAME_UPPER)

set(PROJECT_OUTPUT_NAME "${PROJECT_NAME_SUBSTRING}")

set(PROJECT_AUTHOR "AlliedModders (Wend4r's fork)")
set(PROJECT_DESCRIPTION_FULL "Source SDK toolkit")
string(TIMESTAMP PROJECT_BUILD_DATE "%Y-%m-%d")
string(TIMESTAMP PROJECT_BUILD_TIME "%H:%M:%S")

set(DEVTOOLS_DIR "devtools")
set(DEVTOOLS_BIN_DIR "${DEVTOOLS_DIR}/bin")
set(THIRDPARTY_DIR "thirdparty")
set(PROTOBUF_DIR "${THIRDPARTY_DIR}/protobuf")
set(SOURCESDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

option(SOURCESDK_MALLOC_OVERRIDE "Valve memory system" OFF)
option(SOURCESDK_CREATE_INTEFACE_OVERRIDE "Enable it if you are using your own CreateInteface" OFF)

set(SOURCESDK_PROTO_DIRS
	common
	game/shared
)

set(SOURCESDK_PROTO_FILES
	common/connectionless_netmessages
	common/engine_gcmessages
	common/netmessages
	common/networkbasetypes
	common/network_connection
	common/steamnetworkingsockets_messages_certs
	common/steamnetworkingsockets_messages_udp
	common/steamnetworkingsockets_messages
	common/valveextensions
	game/shared/usermessages
)

set(SOURCESDK_ENTITY2_DIR "${SOURCESDK_DIR}/entity2")
set(SOURCESDK_INTERFACES_DIR "${SOURCESDK_DIR}/interfaces")
set(SOURCESDK_MATHLIB_DIR "${SOURCESDK_DIR}/mathlib")
set(SOURCESDK_TIER1_DIR "${SOURCESDK_DIR}/tier1")

include(cmake/platform/shared.cmake)

if(WINDOWS)
	include(cmake/platform/windows.cmake)
elseif(LINUX)
	include(cmake/platform/linux.cmake)
elseif(MACOS)
	include(cmake/platform/macos.cmake)
endif()

include(cmake/protobuf.cmake)
include(cmake/protobuf/targets/libprotobuf-lite.cmake)
include(cmake/protobuf/targets/libprotobuf.cmake)
include(cmake/protobuf/targets/libprotoc.cmake)
include(cmake/protobuf/targets/protoc.cmake)

include(cmake/sourcesdk.cmake)
include(cmake/sourcesdk/proto.cmake)
include(cmake/sourcesdk/proto/generate.cmake)
include(cmake/sourcesdk/targets/game/server.cmake)
include(cmake/sourcesdk/targets/game/shared.cmake)
include(cmake/sourcesdk/targets/common.cmake)
include(cmake/sourcesdk/targets/engine.cmake)
include(cmake/sourcesdk/targets/entity2.cmake)
include(cmake/sourcesdk/targets/interfaces.cmake)
include(cmake/sourcesdk/targets/mathlib.cmake)
include(cmake/sourcesdk/targets/public.cmake)
include(cmake/sourcesdk/targets/steamworks.cmake)
include(cmake/sourcesdk/targets/tier0.cmake)
include(cmake/sourcesdk/targets/tier1.cmake)

add_library(${PROJECT_NAME} ${SOURCESDK_SOURCE_FILES} ${SOURCESDK_PROTO_SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 17
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF

	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

if(WINDOWS)
	set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(MACOS)
	set_target_properties(${PROJECT_NAME} PROPERTIES OSX_ARCHITECTURES "x86_64")
endif()

target_compile_options(${PROJECT_NAME} PUBLIC ${PLATFORM_COMPILE_OPTIONS} ${SOURCESDK_COMPILER_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${PLATFORM_LINK_OPTIONS} ${SOURCESDK_LINK_OPTIONS})

target_compile_definitions(${PROJECT_NAME} PUBLIC ${PLATFORM_COMPILE_DEFINITIONS} ${SOURCESDK_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${PLATFORM_INCLUDE_DIRS} ${SOURCESDK_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC ${PLATFORM_LINK_LIBRARIES} ${PROTOBUF_LIBPPROTOBUF_NAME} ${SOURCESDK_LINK_LIBRARIES} ${SOURCESDK_ENTITY2_NAME} ${SOURCESDK_INTERFACES_NAME} ${SOURCESDK_MATHLIB_NAME} ${SOURCESDK_TIER1_NAME})
