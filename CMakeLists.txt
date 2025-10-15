cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

if(POLICY CMP0092)
	cmake_policy(SET CMP0092 NEW) # Don't add -W3 warning level by default.
endif()

project(sourcesdk
	VERSION 2
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

set(SOURCESDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(DEVTOOLS_DIR "${SOURCESDK_DIR}/devtools")
set(DEVTOOLS_BIN_DIR "${DEVTOOLS_DIR}/bin")
set(THIRDPARTY_DIR "${SOURCESDK_DIR}/thirdparty")
set(GAME_PROTOBUFS_DIR "${THIRDPARTY_DIR}/game_protobufs")
set(PROTOBUF_DIR "${THIRDPARTY_DIR}/protobuf")

set(GAME_PROTOBUFS_NAME "Game Protobufs")
set(PROTOBUF_NAME "Protocol Buffer")

if(NOT EXISTS "${GAME_PROTOBUFS_DIR}")
	message(FATAL_ERROR "Directory \"${GAME_PROTOBUFS_DIR}\" (${GAME_PROTOBUFS_NAME}) hasn't found. Did you clone recursively?")
endif()

if(NOT EXISTS "${PROTOBUF_DIR}")
	message(FATAL_ERROR "Directory \"${PROTOBUF_DIR}\" (${PROTOBUF_NAME}) hasn't found. Did you clone recursively?")
endif()

set(SOURCESDK_GAME_TARGET "cs2" CACHE STRING "Game name. See CMakeGameManifests.json")
option(SOURCESDK_AM_DEFINES "Compile with AlliedModders definition set" OFF)
option(SOURCESDK_COMPILE_PROTOBUF "Compile Protocol Buffers" ON)
option(SOURCESDK_CONFIGURE_EXPORT_MAP "Configure export symbols/map (Unix only)" ON)
option(SOURCESDK_CREATE_INTEFACE_OVERRIDE "Enable it if you are using your own CreateInteface" OFF)
option(SOURCESDK_LINK_TIER0 "Link with tier0" ON)
option(SOURCESDK_LINK_STEAMWORKS "Link with Steam API" ON)
option(SOURCESDK_LINK_STRIP_CPP_EXPORTS "Strip C++/STL export funcitons from linked libraries (e.g. tier0 and steam_api)" OFF)
option(SOURCESDK_LINK_STRIP_SYMBOLS "Strip debug symbols from built targets" OFF)
option(SOURCESDK_LINK_ENABLE_RPATH "Embed $ORIGIN into ELF RPATH/RUNPATH so binaries load project .so next to the executable" ON)
option(SOURCESDK_LINK_USE_MOLD "Link with the mold linker (if exists)" OFF)
option(SOURCESDK_MALLOC_OVERRIDE "Valve memory system" OFF)
option(SOURCESDK_MSVC_RUNTIME_LIBRARY "MultiThreaded assembly of run-time library by MSVC" ON)
option(SOURCESDK_USE_ABI0 "Enable use of the older C++ ABI, which was the default in GCC versions before GCC 5" ON)

set(SOURCESDK_COMMON_DIR "${SOURCESDK_DIR}/common")
set(SOURCESDK_ENTITY2_DIR "${SOURCESDK_DIR}/entity2")
set(SOURCESDK_INTERFACES_DIR "${SOURCESDK_DIR}/interfaces")
set(SOURCESDK_MATHLIB_DIR "${SOURCESDK_DIR}/mathlib")
set(SOURCESDK_PUBLIC_DIR "${SOURCESDK_DIR}/public")
set(SOURCESDK_SYMBOLS_DIR "${SOURCESDK_DIR}/sym")
set(SOURCESDK_TIER1_DIR "${SOURCESDK_DIR}/tier1")

set(SOURCESDK_NEEDED_PROTOS
	netmessages
	network_connection
	networkbasetypes
	networksystem_protomessages
	gameevents
	usermessages
)

include(cmake/platform/shared.cmake)

if(WINDOWS)
	include(cmake/platform/windows.cmake)
elseif(LINUX)
	include(cmake/platform/linux.cmake)
elseif(MACOS)
	include(cmake/platform/macos.cmake)
endif()

if(SOURCESDK_COMPILE_PROTOBUF)
	include(cmake/protobuf.cmake)
	include(cmake/protobuf/targets/libprotobuf-lite.cmake)
	include(cmake/protobuf/targets/libprotobuf.cmake)
	include(cmake/protobuf/targets/libprotoc.cmake)
	include(cmake/protobuf/targets/protoc.cmake)
endif()

include(cmake/sourcesdk.cmake)
include(cmake/sourcesdk/manifests.cmake)
include(cmake/sourcesdk/manifests/proto.cmake)
include(cmake/sourcesdk/proto.cmake)
include(cmake/sourcesdk/proto/clean_prev.cmake)
include(cmake/sourcesdk/proto/generate.cmake)
include(cmake/sourcesdk/targets/game/server.cmake)
include(cmake/sourcesdk/targets/game/shared.cmake)
include(cmake/sourcesdk/targets/common.cmake)
include(cmake/sourcesdk/targets/engine.cmake)
include(cmake/sourcesdk/targets/entity2.cmake)
include(cmake/sourcesdk/targets/interfaces.cmake)
include(cmake/sourcesdk/targets/mathlib.cmake)

if(SOURCESDK_COMPILE_PROTOBUF)
	include(cmake/sourcesdk/targets/protos.cmake)
endif()

include(cmake/sourcesdk/targets/public.cmake)
include(cmake/sourcesdk/targets/steamworks.cmake)
include(cmake/sourcesdk/targets/tier0.cmake)
include(cmake/sourcesdk/targets/tier1.cmake)

set(SOURCESDK_GAME_TARGET_PREV "${SOURCESDK_GAME_TARGET}" CACHE INTERNAL "Previous value of SOURCESDK_GAME_TARGET")

add_library(${PROJECT_NAME} STATIC ${SOURCESDK_SOURCE_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 17
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF

	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

if(WINDOWS)
	if(SOURCESDK_MSVC_RUNTIME_LIBRARY)
		set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	endif()
elseif(MACOS)
	set_target_properties(${PROJECT_NAME} PROPERTIES OSX_ARCHITECTURES "x86_64")
endif()

target_compile_options(${PROJECT_NAME} PUBLIC ${PLATFORM_COMPILE_OPTIONS} ${SOURCESDK_COMPILER_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${PLATFORM_LINK_OPTIONS} ${SOURCESDK_LINK_OPTIONS})

target_compile_definitions(${PROJECT_NAME} PUBLIC ${PLATFORM_COMPILE_DEFINITIONS} ${SOURCESDK_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${PLATFORM_INCLUDE_DIRS} ${SOURCESDK_INCLUDE_DIRS})

set(SOURCESDK_LINK_LIBRARIES
	${PLATFORM_LINK_LIBRARIES}
	${SOURCESDK_LINK_LIBRARIES}
	${SOURCESDK_ENTITY2_NAME}
	${SOURCESDK_INTERFACES_NAME}
	${SOURCESDK_MATHLIB_NAME}
	${SOURCESDK_PROTOS_NAME}
	${SOURCESDK_TIER1_NAME}
)

if(SOURCESDK_LINK_TIER0)
	list(APPEND SOURCESDK_LINK_LIBRARIES ${SOURCESDK_TIER0_NAME})
endif()

if(SOURCESDK_LINK_STEAMWORKS)
	list(APPEND SOURCESDK_LINK_LIBRARIES ${SOURCESDK_STEAM_NAME})
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC ${PLATFORM_LINK_LIBRARIES} ${SOURCESDK_LINK_LIBRARIES})

if(SOURCESDK_COMPILE_PROTOBUF)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${PROTOBUF_LIBPPROTOBUF_NAME})
endif()
