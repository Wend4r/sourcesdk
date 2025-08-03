set(PLATFORM_COMPILE_OPTIONS
	${PLATFORM_COMPILE_OPTIONS}

	-Wall
	-Wno-array-bounds
	-Wno-attributes -Wno-ignored-attributes
	-Wno-conversion -Wno-overloaded-virtual
	-Wno-delete-non-virtual-dtor -Wno-non-virtual-dtor
	-Wno-invalid-offsetof -Wno-invalid-noreturn
	-Wno-sign-compare
	-Wno-unused -Wno-register
	-Wno-narrowing

	-mtune=generic -mmmx -msse -msse2 -msse3 -msse4 -msse4.1 -msse4.2
	-fvisibility=default -fPIC

	-fno-strict-aliasing -fno-threadsafe-statics
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(PLATFORM_COMPILE_OPTIONS
		${PLATFORM_COMPILE_OPTIONS}

		-g3 -ggdb
	)
endif()

set(PLATFORM_LINK_OPTIONS
	${PLATFORM_LINK_OPTIONS}
)

if(SOURCESDK_CONFIGURE_EXPORT_MAP)
	set(PLATFORM_LINK_OPTIONS
		${PLATFORM_LINK_OPTIONS}

		-Wl,--version-script,${SOURCESDK_SYMBOLS_DIR}/version_script.lds
	)
endif()

if(SOURCESDK_LINK_USE_MOLD)
	set(PLATFORM_LINK_OPTIONS
		${PLATFORM_LINK_OPTIONS}

		-fuse-ld=mold
	)
endif()

if(SOURCESDK_LINK_STRIP_SYMBOLS)
	set(PLATFORM_LINK_OPTIONS
		${PLATFORM_LINK_OPTIONS}

		-s
	)
endif()

set(PLATFORM_COMPILE_DEFINITIONS
	${PLATFORM_COMPILE_DEFINITIONS}

	bswap_16=__bswap_16
	bswap_32=__bswap_32
	bswap_64=__bswap_64

	_GLIBCXX_USE_CXX11_ABI=0
)
