set(PLATFORM_COMPILER_OPTIONS
	${PLATFORM_COMPILER_OPTIONS}

	-Wall
	-Wno-attributes -Wno-ignored-attributes
	-Wno-conversion -Wno-overloaded-virtual
	-Wno-delete-non-virtual-dtor -Wno-non-virtual-dtor
	-Wno-invalid-offsetof -Wno-invalid-noreturn
	-Wno-sign-compare
	-Wno-unused -Wno-register
	-Wno-narrowing
	-Wno-ordered-compare-function-pointers

	-mtune=generic -mmmx -msse -msse2 -msse3 -msse4 -msse4.1
	-fvisibility=default -fPIC

	-fno-strict-aliasing -fno-threadsafe-statics
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(PLATFORM_COMPILER_OPTIONS
		${PLATFORM_COMPILER_OPTIONS}

		-g3 -ggdb
	)
endif()

set(PLATFORM_LINKER_OPTIONS
	${PLATFORM_LINKER_OPTIONS}
)

set(PLATFORM_COMPILE_DEFINITIONS
	${PLATFORM_COMPILE_DEFINITIONS}

	-Dbswap_16=__bswap_16
	-Dbswap_32=__bswap_32
	-Dbswap_64=__bswap_64

	-D_GLIBCXX_USE_CXX11_ABI=0
)
