if(NOT SOURCESDK_DIR)
	message(FATAL_ERROR "SOURCESDK_DIR is empty")
endif()

if(NOT SOURCESDK_SCHEMACOMPILER)
	message(FATAL_ERROR "SOURCESDK_COMPILE_SCHEMA needs SOURCESDK_SCHEMACOMPILER")
endif()

#
# Check target: the schema records of the marked SDK headers, generated and compiled but linked nowhere.
# The game modules register these types themselves, a plugin must not register them a second time
#
set(SOURCESDK_SCHEMA_NAME "${PROJECT_NAME}_schema")

# Headers that are not self-contained; entityinstance.h completes CEntityInstance for the destructor of CEntityOwnerPtr
set(SOURCESDK_SCHEMA_PRE_INCLUDE
	tier0/platform.h
	tier0/memalloc.h
	mathlib/vector.h
	entity2/entityidentity.h
	entity2/entityinstance.h
)

add_library(${SOURCESDK_SCHEMA_NAME} STATIC)
add_library(${PROJECT_NAME}::${SOURCESDK_SCHEMA_NAME} ALIAS ${SOURCESDK_SCHEMA_NAME})

set_target_properties(${SOURCESDK_SCHEMA_NAME} PROPERTIES
	C_STANDARD 17
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF

	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

if(WINDOWS)
	if(SOURCESDK_MSVC_RUNTIME_LIBRARY)
		set_target_properties(${SOURCESDK_SCHEMA_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	endif()
elseif(MACOS)
	set_target_properties(${SOURCESDK_SCHEMA_NAME} PROPERTIES OSX_ARCHITECTURES "x86_64")
endif()

target_link_libraries(${SOURCESDK_SCHEMA_NAME} PRIVATE ${PROJECT_NAME})

# The transfer bodies of the SDK classes are hand-written or live in the game
sourcesdk_target_schema(${SOURCESDK_SCHEMA_NAME}
	SOURCES ${SOURCESDK_SCHEMA_SOURCE_FILES}
	PROJECT_NAME ${PROJECT_NAME}
	PRE_INCLUDE ${SOURCESDK_SCHEMA_PRE_INCLUDE}
	NO_CODEGEN_TAGS
)
