include(CTest)

find_package(Threads REQUIRED)

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

set(SOURCESDK_TEST_LINK_OPTIONS
	${PLATFORM_LINK_OPTIONS}
	${SOURCESDK_LINK_OPTIONS}
)

if(MACOS AND SOURCESDK_CONFIGURE_EXPORT_MAP)
	list(REMOVE_ITEM SOURCESDK_TEST_LINK_OPTIONS
		-Wl,-exported_symbols_list,${SOURCESDK_SYMBOLS_DIR}/exported_symbols.lds
	)
endif()

function(sourcesdk_setup_test_target target_name)
	if(NOT TARGET ${SOURCESDK_TIER0_NAME})
		message(FATAL_ERROR "${target_name} requires ${SOURCESDK_TIER0_NAME}")
	endif()

	target_compile_features(${target_name} PRIVATE cxx_std_23)
	target_compile_options(${target_name} PRIVATE ${PLATFORM_COMPILE_OPTIONS} ${SOURCESDK_COMPILER_OPTIONS})
	target_link_options(${target_name} PRIVATE ${SOURCESDK_TEST_LINK_OPTIONS})

	target_include_directories(${target_name} PRIVATE
		${SOURCESDK_INCLUDE_DIRS}
		${SOURCESDK_MATHLIB_INCLUDE_DIRS}
		${CMAKE_SOURCE_DIR}/public
		${CMAKE_SOURCE_DIR}/public/tier0
		${CMAKE_SOURCE_DIR}/public/tier1
		${CMAKE_SOURCE_DIR}/public/kv3lib
		${CMAKE_SOURCE_DIR}/public/mathlib
		${CMAKE_CURRENT_SOURCE_DIR}
	)

	target_link_libraries(${target_name} PRIVATE
		${SOURCESDK_KV3LIB_NAME}
		${SOURCESDK_TIER1_NAME}
		Threads::Threads
		${CMAKE_DL_LIBS}
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
	commandbuffer.cpp
	construct.cpp
	fasttimer.cpp
	generichash.cpp
	memblockallocator.cpp
	mempool.cpp
	minidump.cpp
	refcount.cpp
	stringpool.cpp
	strtools.cpp
	tier0_allocators.cpp
	tier0_compression.cpp
	tier0_coroutine.cpp
	tier0_dbg.cpp
	tier0_eventdispatcher.cpp
	tier0_fileio.cpp
	tier0_hashes.cpp
	tier0_jsonparser.cpp
	tier0_keyvalues.cpp
	tier0_platform.cpp
	tier0_quantizedfloatencoder.cpp
	tier0_snapshots.cpp
	tier0_stack.cpp
	tier0_stackhelpers.cpp
	tier0_stdlib.cpp
	tier0_strings.cpp
	tier0_threadtools.cpp
	tier0_vertexkdtree.cpp
	tier0_vprof.cpp
	tslist.cpp
	utlarray.cpp
	utlbinaryblock.cpp
	utlblockmemory.cpp
	utlblockvector.cpp
	utlbuffer.cpp
	utldict.cpp
	utlfixedmemory.cpp
	utlflags.cpp
	utlhandletable.cpp
	utlhash.cpp
	utlhashmaplarge.cpp
	utlhashtable.cpp
	utlintrusivelist.cpp
	utlleanvector.cpp
	utllinkedlist.cpp
	utllrucache.cpp
	utlmap.cpp
	utlmemory.cpp
	utlmultilist.cpp
	utlntree.cpp
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
	utlsymbollarge.cpp
	utltshash.cpp
	utlvector.cpp
	vswap.cpp
)

foreach(test_source IN LISTS SOURCESDK_CONTAINER_TEST_SOURCES)
	sourcesdk_add_cpp_test("" containers_main.cpp ${test_source})
endforeach()

# The minidumps of tier0 stop the threads of the process, which stalls under the load of the other tests
set_tests_properties(minidump_tests PROPERTIES RUN_SERIAL TRUE)

set(SOURCESDK_SMOKE_TEST_SOURCES
	tier0_headers.cpp
	tier0_utl_headers.cpp
	tier1_utl_headers.cpp
	keyvalues3_headers.cpp
	jobthread.cpp
)

foreach(test_source IN LISTS SOURCESDK_SMOKE_TEST_SOURCES)
	sourcesdk_add_cpp_test(smoke smoke_main.cpp ${test_source})
endforeach()

set(SOURCESDK_MATHLIB_TEST_SOURCES
	mathlib.cpp
	vec3d.cpp
)

foreach(test_source IN LISTS SOURCESDK_MATHLIB_TEST_SOURCES)
	sourcesdk_add_cpp_test("" containers_main.cpp ${test_source})

	get_filename_component(test_name_we "${test_source}" NAME_WE)
	target_link_libraries(${test_name_we}_tests PRIVATE ${SOURCESDK_MATHLIB_NAME})
endforeach()

add_executable(keyvalues3_tests
	${SOURCESDK_TEST_COMMON_SOURCES}
	${SOURCESDK_TEST_COMMON_HEADERS}

	keyvalues3_main.cpp
	keyvalues3.cpp
)

target_compile_definitions(keyvalues3_tests PRIVATE
	SOURCESDK_KEYVALUES3_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/keyvalues3"
)

sourcesdk_setup_test_target(keyvalues3_tests)

sourcesdk_add_cpp_test("" keyvalues3_main.cpp keyvalues3_binary.cpp)
sourcesdk_add_cpp_test("" keyvalues3_main.cpp kv3transfer.cpp)
sourcesdk_add_cpp_test("" keyvalues3_main.cpp kv3transfer_schema.cpp)
sourcesdk_add_cpp_test("" keyvalues3_main.cpp schemametatag.cpp)

sourcesdk_add_cpp_test("" containers_main.cpp convar.cpp)

target_link_libraries(convar_tests PRIVATE
	${SOURCESDK_TIER0_PERPROJECT_NAME}
)

sourcesdk_add_cpp_test("" containers_main.cpp convar_lifecycle.cpp)

target_link_libraries(convar_lifecycle_tests PRIVATE
	${SOURCESDK_TIER0_PERPROJECT_NAME}
)

sourcesdk_add_cpp_test("" containers_main.cpp vscriptbinding.cpp)
