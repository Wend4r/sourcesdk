if(NOT SOURCESDK_DIR)
	message(FATAL_ERROR "SOURCESDK_DIR is empty")
endif()

set(SOURCESDK_GAME_SHARED_INCLUDE_DIR "${SOURCESDK_DIR}/game/shared")
set(SOURCESDK_PUBLIC_GAME_SHARED_INCLUDE_DIR "${SOURCESDK_DIR}/public/game/shared")

list(APPEND SOURCESDK_INCLUDE_DIRS
	${SOURCESDK_GAME_SHARED_INCLUDE_DIR}
	${SOURCESDK_PUBLIC_GAME_SHARED_INCLUDE_DIR}
)