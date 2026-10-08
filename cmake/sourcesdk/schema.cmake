if(NOT SOURCESDK_DIR)
	message(FATAL_ERROR "SOURCESDK_DIR is empty")
endif()

set(SOURCESDK_SCHEMA_DIR "${SOURCESDK_DIR}/cmake/sourcesdk/schema")
set(SOURCESDK_SCHEMA_ATOMIC_CONFIG "${SOURCESDK_SCHEMA_DIR}/atomic_types.kv3")

# Project file format of schemaversionnumbers.h (SCHEMA_PROJECT_VERSION)
set(SOURCESDK_SCHEMA_PROJECT_VERSION 1)

if(SOURCESDK_SCHEMACOMPILER)
	if(CMAKE_VERSION VERSION_LESS 3.20)
		message(FATAL_ERROR "SOURCESDK_SCHEMACOMPILER needs CMake 3.20 or newer (DEPFILE and generator expressions in custom command outputs)")
	endif()

	if(SOURCESDK_SCHEMACOMPILER_EXECUTABLE)
		if(NOT EXISTS "${SOURCESDK_SCHEMACOMPILER_EXECUTABLE}")
			message(FATAL_ERROR "SOURCESDK_SCHEMACOMPILER_EXECUTABLE \"${SOURCESDK_SCHEMACOMPILER_EXECUTABLE}\" does not exist")
		endif()

		set(SOURCESDK_SCHEMACOMPILER_COMMAND "${SOURCESDK_SCHEMACOMPILER_EXECUTABLE}")
		set(SOURCESDK_SCHEMACOMPILER_DEPENDS "${SOURCESDK_SCHEMACOMPILER_EXECUTABLE}")
	else()
		include(${SOURCESDK_DIR}/cmake/sourcesdk/targets/schemacompiler.cmake)

		set(SOURCESDK_SCHEMACOMPILER_COMMAND "$<TARGET_FILE:${SOURCESDK_SCHEMACOMPILER_NAME}>")
		set(SOURCESDK_SCHEMACOMPILER_DEPENDS ${SOURCESDK_SCHEMACOMPILER_NAME})
	endif()
endif()

# A KV3 array of multiline strings: values keep quotes and backslashes as they are
function(sourcesdk_schema_kv3_strings OUT_VAR VALUES)
	set(${OUT_VAR} "[ \"\"\"\n$<JOIN:$<REMOVE_DUPLICATES:${VALUES}>,\n\"\"\"$<COMMA> \"\"\"\n>\n\"\"\" ]" PARENT_SCOPE)
endfunction()

#
# sourcesdk_target_schema( <target>
#	SOURCES <file>...			Files whose schema types the target registers
#	[PROJECT_NAME <name>]			Schema project name, a C identifier; defaults to the target name
#	[TYPE_SCOPE GLOBAL|MODULE]		Defaults to MODULE
#	[PRE_INCLUDE <header>...]		Included before every input, for headers that are not self-contained
#	[PCH <header>]				Compiled once ahead of the inputs
#	[TAG_HEADERS <header>...]		Meta tag declarations besides schemasystem/schemametatags.h
#	[IMPORT_MODULES <module>...]		Modules whose type scopes hold the external classes
#	[UNITY_BATCH_SIZE <n>]			Headers parsed per translation unit, 0 is one per input
#	[OUTPUT_DIR <dir>]			Defaults to ${CMAKE_CURRENT_BINARY_DIR}/schema, one target per directory
#	[NO_CODEGEN_TAGS]			Records only, the DECLARE_SCHEMA_CODEGEN_TAG scripts do not run
# )
#
function(sourcesdk_target_schema TARGET)
	if(NOT SOURCESDK_SCHEMACOMPILER)
		message(FATAL_ERROR "sourcesdk_target_schema( ${TARGET} ): turn SOURCESDK_SCHEMACOMPILER on, optionally with SOURCESDK_SCHEMACOMPILER_EXECUTABLE pointing at a prebuilt schemacompiler")
	endif()

	cmake_parse_arguments(SCHEMA
		"NO_CODEGEN_TAGS"
		"PROJECT_NAME;TYPE_SCOPE;PCH;UNITY_BATCH_SIZE;OUTPUT_DIR"
		"SOURCES;PRE_INCLUDE;TAG_HEADERS;IMPORT_MODULES"
		${ARGN}
	)

	if(SCHEMA_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "sourcesdk_target_schema( ${TARGET} ): unknown arguments \"${SCHEMA_UNPARSED_ARGUMENTS}\"")
	endif()

	if(NOT TARGET ${TARGET})
		message(FATAL_ERROR "sourcesdk_target_schema( ${TARGET} ): no such target")
	endif()

	if(NOT SCHEMA_SOURCES)
		message(FATAL_ERROR "sourcesdk_target_schema( ${TARGET} ): SOURCES is empty")
	endif()

	if(NOT SCHEMA_PROJECT_NAME)
		set(SCHEMA_PROJECT_NAME "${TARGET}")
	endif()

	if(NOT SCHEMA_PROJECT_NAME MATCHES "^[A-Za-z_][A-Za-z0-9_]*$")
		message(FATAL_ERROR "sourcesdk_target_schema( ${TARGET} ): PROJECT_NAME \"${SCHEMA_PROJECT_NAME}\" must be a C identifier")
	endif()

	if(NOT SCHEMA_TYPE_SCOPE)
		set(SCHEMA_TYPE_SCOPE "MODULE")
	endif()

	if(NOT SCHEMA_TYPE_SCOPE MATCHES "^(GLOBAL|MODULE)$")
		message(FATAL_ERROR "sourcesdk_target_schema( ${TARGET} ): TYPE_SCOPE must be GLOBAL or MODULE")
	endif()

	if(NOT SCHEMA_UNITY_BATCH_SIZE)
		set(SCHEMA_UNITY_BATCH_SIZE 0)
	endif()

	if(NOT SCHEMA_OUTPUT_DIR)
		set(SCHEMA_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/schema")
	endif()

	get_filename_component(SCHEMA_OUTPUT_DIR "${SCHEMA_OUTPUT_DIR}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")

	# The project file, cache and PCH of a target are named by the directory alone
	get_property(SCHEMA_OUTPUT_DIRS GLOBAL PROPERTY SOURCESDK_SCHEMA_OUTPUT_DIRS)

	if("${SCHEMA_OUTPUT_DIR}" IN_LIST SCHEMA_OUTPUT_DIRS)
		message(FATAL_ERROR "sourcesdk_target_schema( ${TARGET} ): \"${SCHEMA_OUTPUT_DIR}\" is the output of another target, set OUTPUT_DIR")
	endif()

	set_property(GLOBAL APPEND PROPERTY SOURCESDK_SCHEMA_OUTPUT_DIRS "${SCHEMA_OUTPUT_DIR}")
	get_target_property(SCHEMA_TARGET_SOURCE_DIR ${TARGET} SOURCE_DIR)
	get_property(SCHEMA_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

	# Configurations differ in their defines, each gets its own outputs
	if(SCHEMA_MULTI_CONFIG)
		set(SCHEMA_CONFIG_DIR "${SCHEMA_OUTPUT_DIR}/$<CONFIG>")
	else()
		set(SCHEMA_CONFIG_DIR "${SCHEMA_OUTPUT_DIR}")
	endif()

	set(SCHEMA_INPUTS)
	set(SCHEMA_OUTPUTS)
	set(SCHEMA_INPUTS_TEXT)

	foreach(SCHEMA_SOURCE IN LISTS SCHEMA_SOURCES)
		get_filename_component(SCHEMA_SOURCE "${SCHEMA_SOURCE}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
		file(RELATIVE_PATH SCHEMA_NAME "${SCHEMA_TARGET_SOURCE_DIR}" "${SCHEMA_SOURCE}")

		# Outside the target directory: the file name keeps the outputs inside OUTPUT_DIR
		if(SCHEMA_NAME MATCHES "^\\.\\./" OR IS_ABSOLUTE "${SCHEMA_NAME}")
			get_filename_component(SCHEMA_NAME "${SCHEMA_SOURCE}" NAME)
		endif()

		get_filename_component(SCHEMA_EXTENSION "${SCHEMA_SOURCE}" LAST_EXT)
		string(REGEX REPLACE "\\.[^./]*$" "" SCHEMA_STEM "${SCHEMA_NAME}")
		string(SUBSTRING "${SCHEMA_EXTENSION}" 1 -1 SCHEMA_EXTENSION)

		set(SCHEMA_OUTPUT "${SCHEMA_CONFIG_DIR}/${SCHEMA_STEM}_${SCHEMA_EXTENSION}_schema.cpp")

		if(SCHEMA_EXTENSION MATCHES "^(c|cc|cpp|cxx)$")
			set(SCHEMA_IS_CPP "true")

			# The generated unit includes it, as VPC did
			set_source_files_properties("${SCHEMA_SOURCE}" TARGET_DIRECTORY ${TARGET} PROPERTIES HEADER_FILE_ONLY ON)
		else()
			set(SCHEMA_IS_CPP "false")
		endif()

		list(APPEND SCHEMA_INPUTS "${SCHEMA_SOURCE}")
		list(APPEND SCHEMA_OUTPUTS "${SCHEMA_OUTPUT}")
		string(APPEND SCHEMA_INPUTS_TEXT "\t\t{ input = \"${SCHEMA_SOURCE}\" output = \"${SCHEMA_OUTPUT}\" name = \"${SCHEMA_NAME}\" is_cpp = ${SCHEMA_IS_CPP} },\n")
	endforeach()

	list(REMOVE_DUPLICATES SCHEMA_INPUTS)

	set(SCHEMA_REGISTRATION "${SCHEMA_CONFIG_DIR}/${SCHEMA_PROJECT_NAME}_schema_registration.cpp")
	set(SCHEMA_PROJECT_FILE "${SCHEMA_OUTPUT_DIR}/schema_project.$<CONFIG>.kv3")
	set(SCHEMA_STAMP "${SCHEMA_CONFIG_DIR}/${SCHEMA_PROJECT_NAME}.schema.stamp")
	set(SCHEMA_DEPFILE "${SCHEMA_CONFIG_DIR}/${SCHEMA_PROJECT_NAME}.schema.d")

	if(WINDOWS)
		set(SCHEMA_PLATFORM "windows")
	elseif(MACOS)
		set(SCHEMA_PLATFORM "macos")
	else()
		set(SCHEMA_PLATFORM "linux")
	endif()

	if(MSVC)
		set(SCHEMA_COMPILER "msvc")
	else()
		set(SCHEMA_COMPILER "gnu")
	endif()

	if(SCHEMA_NO_CODEGEN_TAGS)
		set(SCHEMA_EMIT_CODEGEN "false")
	else()
		set(SCHEMA_EMIT_CODEGEN "true")
	endif()

	# Parser flags come from the target itself, usage requirements of its dependencies included
	sourcesdk_schema_kv3_strings(SCHEMA_DEFINES_TEXT "$<TARGET_PROPERTY:${TARGET},COMPILE_DEFINITIONS>")
	sourcesdk_schema_kv3_strings(SCHEMA_INCLUDES_TEXT "$<TARGET_PROPERTY:${TARGET},INCLUDE_DIRECTORIES>")
	sourcesdk_schema_kv3_strings(SCHEMA_OPTIONS_TEXT "$<TARGET_PROPERTY:${TARGET},COMPILE_OPTIONS>")
	sourcesdk_schema_kv3_strings(SCHEMA_PRE_INCLUDE_TEXT "${SCHEMA_PRE_INCLUDE}")
	sourcesdk_schema_kv3_strings(SCHEMA_TAG_HEADERS_TEXT "schemasystem/schemametatags.h;${SCHEMA_TAG_HEADERS}")
	sourcesdk_schema_kv3_strings(SCHEMA_IMPORT_MODULES_TEXT "${SCHEMA_IMPORT_MODULES}")

	set(SCHEMA_STANDARD "$<TARGET_PROPERTY:${TARGET},CXX_STANDARD>")

	file(GENERATE
		OUTPUT "${SCHEMA_PROJECT_FILE}"
		CONTENT "<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	version = ${SOURCESDK_SCHEMA_PROJECT_VERSION}
	project = \"${SCHEMA_PROJECT_NAME}\"
	target = \"${TARGET}\"
	config = \"$<CONFIG>\"
	scope = \"${SCHEMA_TYPE_SCOPE}\"
	platform = \"${SCHEMA_PLATFORM}\"
	compiler = \"${SCHEMA_COMPILER}\"
	standard = \"$<IF:$<BOOL:${SCHEMA_STANDARD}>,${SCHEMA_STANDARD},17>\"
	defines = ${SCHEMA_DEFINES_TEXT}
	includes = ${SCHEMA_INCLUDES_TEXT}
	options = ${SCHEMA_OPTIONS_TEXT}
	pre_include = ${SCHEMA_PRE_INCLUDE_TEXT}
	tag_headers = ${SCHEMA_TAG_HEADERS_TEXT}
	import_modules = ${SCHEMA_IMPORT_MODULES_TEXT}
	pch = \"${SCHEMA_PCH}\"
	atomic_config = \"${SOURCESDK_SCHEMA_ATOMIC_CONFIG}\"
	output_dir = \"${SCHEMA_CONFIG_DIR}\"
	registration = \"${SCHEMA_REGISTRATION}\"
	unity_batch_size = ${SCHEMA_UNITY_BATCH_SIZE}
	emit_codegen = ${SCHEMA_EMIT_CODEGEN}
	inputs =
	[
${SCHEMA_INPUTS_TEXT}	]
}
"
		TARGET ${TARGET}
	)

	# The generator rewrites only changed outputs and lists every header it read in the depfile
	add_custom_command(
		OUTPUT "${SCHEMA_STAMP}"
		BYPRODUCTS ${SCHEMA_OUTPUTS} "${SCHEMA_REGISTRATION}"
		COMMAND ${SOURCESDK_SCHEMACOMPILER_COMMAND}
			--project "${SCHEMA_PROJECT_FILE}"
			--config "$<CONFIG>"
			--stamp "${SCHEMA_STAMP}"
			--depfile "${SCHEMA_DEPFILE}"
		DEPENDS ${SCHEMA_INPUTS} "${SCHEMA_PROJECT_FILE}" "${SOURCESDK_SCHEMA_ATOMIC_CONFIG}" ${SOURCESDK_SCHEMACOMPILER_DEPENDS}
		DEPFILE "${SCHEMA_DEPFILE}"
		COMMENT "Generating schema bindings of ${TARGET}"
		VERBATIM
	)

	target_sources(${TARGET} PRIVATE ${SCHEMA_OUTPUTS} "${SCHEMA_REGISTRATION}" "${SCHEMA_STAMP}")
endfunction()
