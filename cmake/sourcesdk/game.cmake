set(SOURCESDK_GAME_DIRS
	artifact
	csgo
	deadlock
	dota2
	tf2
	underlords
)

function(setting_game_dir)
	list(LENGTH SOURCESDK_GAME_DIRS LENGTH)

	foreach(INDEX RANGE ${LENGTH})
		list(GET SOURCESDK_GAME_DIRS ${INDEX} GAME_DIR)

		if(SOURCESDK_GAME_DIR STREQUAL ${GAME_DIR})
			string(TOUPPER ${GAME_DIR} GAME_DIR_UPPER)
			set(SOURCESDK_GAME_DIR_UPPER ${GAME_DIR_UPPER} PARENT_SCOPE)
			set(SOURCESDK_GAME_DIR_VALUE ${INDEX} PARENT_SCOPE)

			return()
		endif()
	endforeach()

	message(FATAL_ERROR "Unsupported game directory")
endfunction()

setting_game_dir()

list(APPEND SOURCESDK_COMPILE_DEFINITIONS
	${SOURCESDK_GAME_DIR_UPPER}=${SOURCESDK_GAME_DIR_VALUE}
)
