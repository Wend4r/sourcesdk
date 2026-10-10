if(NOT SOURCESDK_DIR)
	message(FATAL_ERROR "SOURCESDK_DIR is empty")
endif()

if(CMAKE_VERSION VERSION_LESS 3.20)
	message(FATAL_ERROR "SOURCESDK_COMPILE_SCHEMA needs CMake 3.20 or newer")
endif()

#
# sourcesdk_schema_find_protos( <out_var> HEADERS <file>... INCLUDE_DIRS <dir>... PROTO_DIRS <dir>... )
#
# Follows the #include chain of the headers and returns the protos whose <name>.pb.h they reach, with the
# protos those import. Conditional includes count as well, and headers outside the include roots
# (system and protobuf ones) end the chain
#
function(sourcesdk_schema_find_protos OUT_VAR)
	cmake_parse_arguments(SCAN "" "" "HEADERS;INCLUDE_DIRS;PROTO_DIRS" ${ARGN})

	set(SCAN_QUEUE ${SCAN_HEADERS})
	set(SCAN_VISITED)
	set(SCAN_PROTOS)

	while(SCAN_QUEUE)
		list(POP_FRONT SCAN_QUEUE SCAN_FILE)

		if(SCAN_FILE IN_LIST SCAN_VISITED)
			continue()
		endif()

		list(APPEND SCAN_VISITED "${SCAN_FILE}")
		get_filename_component(SCAN_FILE_DIR "${SCAN_FILE}" DIRECTORY)
		file(STRINGS "${SCAN_FILE}" SCAN_INCLUDES REGEX "^[ \t]*#[ \t]*include[ \t]*[<\"][^>\"]+[>\"]")

		foreach(SCAN_INCLUDE IN LISTS SCAN_INCLUDES)
			string(REGEX REPLACE "^[ \t]*#[ \t]*include[ \t]*[<\"]([^>\"]+)[>\"].*$" "\\1" SCAN_NAME "${SCAN_INCLUDE}")

			if(SCAN_NAME MATCHES "^([A-Za-z0-9_]+)\\.pb\\.h$")
				if(NOT CMAKE_MATCH_1 IN_LIST SCAN_PROTOS)
					list(APPEND SCAN_PROTOS "${CMAKE_MATCH_1}")
					file(RELATIVE_PATH SCAN_RELATIVE "${SOURCESDK_DIR}" "${SCAN_FILE}")
					message(STATUS "Schema: ${CMAKE_MATCH_1} proto for ${SCAN_RELATIVE}")
				endif()

				continue()
			endif()

			foreach(SCAN_DIR IN ITEMS "${SCAN_FILE_DIR}" ${SCAN_INCLUDE_DIRS})
				get_filename_component(SCAN_CANDIDATE "${SCAN_DIR}/${SCAN_NAME}" ABSOLUTE)

				if(EXISTS "${SCAN_CANDIDATE}" AND NOT IS_DIRECTORY "${SCAN_CANDIDATE}")
					if(NOT SCAN_CANDIDATE IN_LIST SCAN_VISITED)
						list(APPEND SCAN_QUEUE "${SCAN_CANDIDATE}")
					endif()

					break()
				endif()
			endforeach()
		endforeach()
	endwhile()

	# Imports of the game protos; google/protobuf ones come with the protobuf library
	set(SCAN_PROTO_QUEUE ${SCAN_PROTOS})

	while(SCAN_PROTO_QUEUE)
		list(POP_FRONT SCAN_PROTO_QUEUE SCAN_PROTO)

		foreach(SCAN_DIR IN LISTS SCAN_PROTO_DIRS)
			if(NOT EXISTS "${SCAN_DIR}/${SCAN_PROTO}.proto")
				continue()
			endif()

			file(STRINGS "${SCAN_DIR}/${SCAN_PROTO}.proto" SCAN_IMPORTS REGEX "^[ \t]*import[ \t]+\"[A-Za-z0-9_]+\\.proto\"")

			foreach(SCAN_IMPORT IN LISTS SCAN_IMPORTS)
				string(REGEX REPLACE "^[ \t]*import[ \t]+\"([A-Za-z0-9_]+)\\.proto\".*$" "\\1" SCAN_IMPORT "${SCAN_IMPORT}")

				if(NOT SCAN_IMPORT IN_LIST SCAN_PROTOS)
					list(APPEND SCAN_PROTOS "${SCAN_IMPORT}")
					list(APPEND SCAN_PROTO_QUEUE "${SCAN_IMPORT}")
				endif()
			endforeach()

			break()
		endforeach()
	endwhile()

	set(${OUT_VAR} ${SCAN_PROTOS} PARENT_SCOPE)
endfunction()

#
# Runs after the target modules: they fill SOURCESDK_SCHEMA_SOURCE_FILES and SOURCESDK_INCLUDE_DIRS.
# The protos the SDK already generated stay as they are, the rest are generated here and join the protos target
#
sourcesdk_schema_find_protos(SOURCESDK_SCHEMA_PROTOS
	HEADERS ${SOURCESDK_SCHEMA_SOURCE_FILES}
	INCLUDE_DIRS ${SOURCESDK_INCLUDE_DIRS}
	PROTO_DIRS ${SOURCESDK_PROTO_DIRS}
)

set(SOURCESDK_SCHEMA_PROTO_FILENAMES)
set(SOURCESDK_SCHEMA_PROTO_SOURCES)

foreach(SOURCESDK_SCHEMA_PROTO IN LISTS SOURCESDK_SCHEMA_PROTOS)
	if(NOT SOURCESDK_SCHEMA_PROTO IN_LIST SOURCESDK_PROTOS)
		list(APPEND SOURCESDK_SCHEMA_PROTO_FILENAMES "${SOURCESDK_SCHEMA_PROTO}.proto")
		list(APPEND SOURCESDK_SCHEMA_PROTO_SOURCES "${SOURCESDK_PROTO_OUTPUT_DIR}/${SOURCESDK_SCHEMA_PROTO}.pb.cc")
	endif()
endforeach()

if(SOURCESDK_SCHEMA_PROTO_FILENAMES)
	if(NOT COMMAND sourcesdk_compile_protos)
		message(FATAL_ERROR "The marked SDK headers need protos (${SOURCESDK_SCHEMA_PROTO_FILENAMES}), but the SDK generates none")
	endif()

	sourcesdk_compile_protos(
		"${SOURCESDK_SCHEMA_PROTO_FILENAMES}"
		"${SOURCESDK_PROTO_ARGS}"
		"${SOURCESDK_PROTO_DIR}"
		"${SOURCESDK_PROTO_OUTPUT_DIR}"
		"${SOURCESDK_LOGS_PROTOS_DATE_DIRECTORY}"
		"${SOURCESDK_LOGS_PROTOS_DATE_ERRORS_DIRECTORY}"
		"schema "
	)

	if(TARGET ${SOURCESDK_PROTOS_NAME})
		target_sources(${SOURCESDK_PROTOS_NAME} PRIVATE ${SOURCESDK_SCHEMA_PROTO_SOURCES})
	endif()
endif()
