set(PLATFORM_COMPILER_OPTIONS
	${PLATFORM_COMPILER_OPTIONS}

	-Wall
	-Wno-array-bounds
	-Wno-attributes -Wno-ignored-attributes
	-Wno-conversion -Wno-overloaded-virtual
	-Wno-delete-non-virtual-dtor -Wno-non-virtual-dtor
	-Wno-deprecated-declarations
	-Wno-dynamic-exception-spec
	-Wno-invalid-offsetof -Wno-invalid-noreturn
	-Wno-sign-compare
	-Wno-unused -Wno-register
	-Wno-narrowing

	-fvisibility=default -fPIC

	-fno-strict-aliasing -fno-threadsafe-statics
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(PLATFORM_COMPILER_OPTIONS
		${PLATFORM_COMPILER_OPTIONS}

		-g3 -ggdb
	)
endif()

set(PLATFORM_LINK_OPTIONS
	${PLATFORM_LINK_OPTIONS}
)

set(PLATFORM_COMPILE_DEFINITIONS
	${PLATFORM_COMPILE_DEFINITIONS}

	_GLIBCXX_USE_CXX11_ABI=0
)
