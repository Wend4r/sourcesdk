if(NOT PROTOBUFS_DIR)
	message(FATAL_ERROR "PROTOBUFS_DIR is empty")
endif()

set(SOURCESDK_PROTO_DIR "${PROTOBUFS_DIR}/${SOURCESDK_GAME_DIR}")
set(SOURCESDK_PROTO_OUTPUT_DIR "${SOURCESDK_DIR}/common")

set(SOURCESDK_PROTO_DIRS
	${SOURCESDK_PROTO_DIR}
)

if(NOT EXISTS "${SOURCESDK_PROTO_DIR}")
	message(FATAL_ERROR "Game \"${SOURCESDK_GAME_DIR}\" is not supported")
endif()

set(SOURCESDK_SKIP_PROTOS
	steammessages_base # steammessages instead
	base_gcmessages_csgo # base_gcmessages instead
)

file(GLOB SOURCESDK_PROTO_FILENAMES "${SOURCESDK_PROTO_DIR}/*.proto")

set(SOURCESDK_PROTOS
	${SOURCESDK_PROTOS}
)

foreach(PROTO_FILENAME ${SOURCESDK_PROTO_FILENAMES})
	get_filename_component(PROTO "${PROTO_FILENAME}" NAME_WLE)

	if(NOT PROTO IN_LIST SOURCESDK_SKIP_PROTOS)
		list(APPEND SOURCESDK_PROTOS "${PROTO}")
	endif()
endforeach()
