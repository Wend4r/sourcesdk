include(CTest)

set(SOURCESDK_TEST_COMMON_SOURCES
	common/runner.cpp
	common/source2_main.cpp
)

set(SOURCESDK_TEST_COMMON_HEADERS
	common/assert.h
	common/macros.h
	common/runner.h
	common/source2_main.h
)

function(sourcesdk_setup_test_target target_name)
	if(NOT TARGET ${SOURCESDK_TIER0_NAME})
		message(FATAL_ERROR "${target_name} requires ${SOURCESDK_TIER0_NAME}")
	endif()

	target_compile_features(${target_name} PRIVATE cxx_std_23)
	target_compile_options(${target_name} PRIVATE ${PLATFORM_COMPILE_OPTIONS} ${SOURCESDK_COMPILER_OPTIONS})
	target_link_options(${target_name} PRIVATE ${PLATFORM_LINK_OPTIONS} ${SOURCESDK_LINK_OPTIONS})

	target_include_directories(${target_name} PRIVATE
		${SOURCESDK_INCLUDE_DIRS}
		${SOURCESDK_MATHLIB_INCLUDE_DIRS}
		${CMAKE_SOURCE_DIR}/public
		${CMAKE_SOURCE_DIR}/public/tier0
		${CMAKE_SOURCE_DIR}/public/tier1
		${CMAKE_SOURCE_DIR}/public/mathlib
		${CMAKE_CURRENT_SOURCE_DIR}
	)

	target_link_libraries(${target_name} PRIVATE
		${SOURCESDK_TIER1_NAME}
	)

	if(EXISTS "${SOURCESDK_TIER0_LIB_FILENAME}")
		target_link_libraries(${target_name} PRIVATE
			${SOURCESDK_TIER0_NAME}
		)
	endif()

	target_compile_definitions(${target_name} PRIVATE
		${PLATFORM_COMPILE_DEFINITIONS}
		${SOURCESDK_COMPILE_DEFINITIONS}
	)

	if(WINDOWS)
		target_compile_definitions(${target_name} PRIVATE
			PLATFORM_WINDOWS=1
		)
	elseif(MACOS)
		set_target_properties(${target_name} PROPERTIES OSX_ARCHITECTURES "x86_64")

		target_compile_definitions(${target_name} PRIVATE
			PLATFORM_OSX=1
		)
	else()
		target_compile_definitions(${target_name} PRIVATE
			PLATFORM_LINUX=1
		)
	endif()

	if(EXISTS "${SOURCESDK_TIER0_LIB_FILENAME}")
		add_custom_command(
			TARGET ${target_name}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				$<TARGET_FILE:${SOURCESDK_TIER0_NAME}>
				$<TARGET_FILE_DIR:${target_name}>
			VERBATIM
		)
	else()
		message(WARNING "${SOURCESDK_TIER0_NAME} runtime library was not found at \"${SOURCESDK_TIER0_LIB_FILENAME}\". ${target_name} will be linked, but direct/CTest execution may need the runtime library next to the executable.")
	endif()

	add_test(
		NAME ${target_name}
		COMMAND $<TARGET_FILE:${target_name}>
	)
endfunction()

function(sourcesdk_add_cpp_test target_prefix main_source test_source)
	get_filename_component(test_name_we "${test_source}" NAME_WE)

	if(target_prefix STREQUAL "")
		set(target_name "${test_name_we}_tests")
	else()
		set(target_name "${target_prefix}_${test_name_we}_tests")
	endif()

	add_executable(${target_name}
		${SOURCESDK_TEST_COMMON_SOURCES}
		${SOURCESDK_TEST_COMMON_HEADERS}
		${main_source}
		${test_source}
	)

	sourcesdk_setup_test_target(${target_name})
endfunction()

set(SOURCESDK_CONTAINER_TEST_SOURCES
	bufferstring.cpp
	utlarray.cpp
	utlblockmemory.cpp
	utlbuffer.cpp
	utldict.cpp
	utlfixedmemory.cpp
	utlflags.cpp
	utlhash.cpp
	utlhashtable.cpp
	utlleanvector.cpp
	utllinkedlist.cpp
	utlmap.cpp
	utlmemory.cpp
	utlmultilist.cpp
	utlpair.cpp
	utlpriorityqueue.cpp
	utlqueue.cpp
	utlrbtree.cpp
	utlscratchmemory.cpp
	utlsignalslot.cpp
	utlsortvector.cpp
	utlstack.cpp
	utlstring.cpp
	utlstringmap.cpp
	utlstringtoken.cpp
	utlsymbol.cpp
	utlvector.cpp
)

foreach(test_source IN LISTS SOURCESDK_CONTAINER_TEST_SOURCES)
	sourcesdk_add_cpp_test("" containers_main.cpp ${test_source})
endforeach()

set(SOURCESDK_SMOKE_TEST_SOURCES
	tier0_utl_headers.cpp
	tier1_utl_headers.cpp
	keyvalues3_headers.cpp
)

foreach(test_source IN LISTS SOURCESDK_SMOKE_TEST_SOURCES)
	sourcesdk_add_cpp_test(smoke smoke_main.cpp ${test_source})
endforeach()

add_executable(keyvalues3_tests
	${SOURCESDK_TEST_COMMON_SOURCES}
	${SOURCESDK_TEST_COMMON_HEADERS}

	keyvalues3_main.cpp
	keyvalues3.cpp
)

sourcesdk_setup_test_target(keyvalues3_tests)
