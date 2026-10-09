if(NOT SOURCESDK_DIR)
	message(FATAL_ERROR "SOURCESDK_DIR is empty")
endif()

list(APPEND SOURCESDK_INCLUDE_DIRS
	${SOURCESDK_PUBLIC_DIR}
)

# Headers with schema types, the input of the sourcesdk_schema check target
list(APPEND SOURCESDK_SCHEMA_SOURCE_FILES
	${SOURCESDK_PUBLIC_DIR}/bitvec.h
	${SOURCESDK_PUBLIC_DIR}/color.h
	${SOURCESDK_PUBLIC_DIR}/const.h
	${SOURCESDK_PUBLIC_DIR}/entityhandle.h
	${SOURCESDK_PUBLIC_DIR}/entitytypes.h
	${SOURCESDK_PUBLIC_DIR}/gametrace.h
	${SOURCESDK_PUBLIC_DIR}/iclientalphaproperty.h
	${SOURCESDK_PUBLIC_DIR}/iloopmode.h
	${SOURCESDK_PUBLIC_DIR}/playerslot.h
	${SOURCESDK_PUBLIC_DIR}/pulse/ipulsesystem.h
	${SOURCESDK_PUBLIC_DIR}/resourcefile/resourcehandle.h
	${SOURCESDK_PUBLIC_DIR}/resourcefile/resourcetype.h
	${SOURCESDK_PUBLIC_DIR}/shake.h
	${SOURCESDK_PUBLIC_DIR}/soundflags.h
	${SOURCESDK_PUBLIC_DIR}/splitscreenslot.h
	${SOURCESDK_PUBLIC_DIR}/variant.h
	${SOURCESDK_PUBLIC_DIR}/vphysics_interface.h
)
