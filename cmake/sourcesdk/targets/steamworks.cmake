if(NOT SOURCESDK_DIR)
	message(FATAL_ERROR "SOURCESDK_DIR is empty")
endif()

append_sourcesdk_shared_library(SOURCESDK_LINK_LIBRARIES steam_api)
