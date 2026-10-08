if(NOT SOURCESDK_DIR)
	message(FATAL_ERROR "SOURCESDK_DIR is empty")
endif()

if(NOT SOURCESDK_SCHEMACOMPILER)
	message(FATAL_ERROR "SOURCESDK_COMPILE_SCHEMA needs SOURCESDK_SCHEMACOMPILER")
endif()

#
# Check target: the schema records of the marked SDK headers, generated and compiled but linked nowhere.
# The game modules register these types themselves, a plugin must not register them a second time
#
set(SOURCESDK_SCHEMA_NAME "${PROJECT_NAME}_schema")

set(SOURCESDK_SCHEMA_SOURCE_FILES
	${SOURCESDK_DIR}/game/server/cstrike15/bot/bot.h
	${SOURCESDK_DIR}/game/server/cstrike15/bot/cs_bot.h
	${SOURCESDK_DIR}/game/server/cstrike15/bot/cs_bot_timers.h
	${SOURCESDK_DIR}/game/server/navlib/cs_nav_pathcost.h
	${SOURCESDK_DIR}/game/server/navlib/nav_area.h
	${SOURCESDK_DIR}/game/server/navlib/nav_mesh.h
	${SOURCESDK_DIR}/game/server/navlib/nav_pathcost.h
	${SOURCESDK_DIR}/game/shared/cstrike15/inbuttonstate.h
	${SOURCESDK_DIR}/game/shared/ehandle.h
	${SOURCESDK_DIR}/game/shared/shareddefs.h
	${SOURCESDK_PUBLIC_DIR}/bitvec.h
	${SOURCESDK_PUBLIC_DIR}/color.h
	${SOURCESDK_PUBLIC_DIR}/const.h
	${SOURCESDK_PUBLIC_DIR}/engine/IEngineService.h
	${SOURCESDK_PUBLIC_DIR}/entity2/entityattributetable.h
	${SOURCESDK_PUBLIC_DIR}/entity2/entitycomponent.h
	${SOURCESDK_PUBLIC_DIR}/entity2/entityidentity.h
	${SOURCESDK_PUBLIC_DIR}/entity2/entityindex.h
	${SOURCESDK_PUBLIC_DIR}/entity2/entityinstance.h
	${SOURCESDK_PUBLIC_DIR}/entity2/entityio.h
	${SOURCESDK_PUBLIC_DIR}/entity2/entitykeyvalues.h
	${SOURCESDK_PUBLIC_DIR}/entity2/entitynetwork.h
	${SOURCESDK_PUBLIC_DIR}/entity2/entitypulse.h
	${SOURCESDK_PUBLIC_DIR}/entity2/scriptcomponent.h
	${SOURCESDK_PUBLIC_DIR}/entityhandle.h
	${SOURCESDK_PUBLIC_DIR}/entitytypes.h
	${SOURCESDK_PUBLIC_DIR}/game/server/iserverentitysubclassutils.h
	${SOURCESDK_PUBLIC_DIR}/gametrace.h
	${SOURCESDK_PUBLIC_DIR}/iclientalphaproperty.h
	${SOURCESDK_PUBLIC_DIR}/iloopmode.h
	${SOURCESDK_PUBLIC_DIR}/kv3lib/keyvalues3.h
	${SOURCESDK_PUBLIC_DIR}/mathlib/aabb.h
	${SOURCESDK_PUBLIC_DIR}/mathlib/mathlib.h
	${SOURCESDK_PUBLIC_DIR}/mathlib/ssemath.h
	${SOURCESDK_PUBLIC_DIR}/mathlib/ssequaternion.h
	${SOURCESDK_PUBLIC_DIR}/mathlib/transform.h
	${SOURCESDK_PUBLIC_DIR}/mathlib/vector.h
	${SOURCESDK_PUBLIC_DIR}/mathlib/vector2d.h
	${SOURCESDK_PUBLIC_DIR}/mathlib/vector4d.h
	${SOURCESDK_PUBLIC_DIR}/mathlib/vectorws.h
	${SOURCESDK_PUBLIC_DIR}/mathlib/vmatrix.h
	${SOURCESDK_PUBLIC_DIR}/networksystem/networkvar.h
	${SOURCESDK_PUBLIC_DIR}/playerslot.h
	${SOURCESDK_PUBLIC_DIR}/pulse/ipulsesystem.h
	${SOURCESDK_PUBLIC_DIR}/resourcefile/resourcehandle.h
	${SOURCESDK_PUBLIC_DIR}/resourcefile/resourcetype.h
	${SOURCESDK_PUBLIC_DIR}/shake.h
	${SOURCESDK_PUBLIC_DIR}/soundflags.h
	${SOURCESDK_PUBLIC_DIR}/splitscreenslot.h
	${SOURCESDK_PUBLIC_DIR}/tier0/basetypes.h
	${SOURCESDK_PUBLIC_DIR}/tier0/bufferstring.h
	${SOURCESDK_PUBLIC_DIR}/tier0/keyvalues.h
	${SOURCESDK_PUBLIC_DIR}/tier0/uniqueid.h
	${SOURCESDK_PUBLIC_DIR}/tier0/utlbinaryblock.h
	${SOURCESDK_PUBLIC_DIR}/tier0/utlstring.h
	${SOURCESDK_PUBLIC_DIR}/tier0/utlstringtoken.h
	${SOURCESDK_PUBLIC_DIR}/tier0/utlsymbol.h
	${SOURCESDK_PUBLIC_DIR}/tier1/smartptr.h
	${SOURCESDK_PUBLIC_DIR}/tier1/utldict.h
	${SOURCESDK_PUBLIC_DIR}/tier1/utlhashtable.h
	${SOURCESDK_PUBLIC_DIR}/tier1/utlincrementalvector.h
	${SOURCESDK_PUBLIC_DIR}/tier1/utlmap.h
	${SOURCESDK_PUBLIC_DIR}/tier1/utlsymbollarge.h
	${SOURCESDK_PUBLIC_DIR}/tier1/utlvector.h
	${SOURCESDK_PUBLIC_DIR}/variant.h
	${SOURCESDK_PUBLIC_DIR}/vphysics_interface.h
)

# Headers that are not self-contained; entityinstance.h completes CEntityInstance for the destructor of CEntityOwnerPtr
set(SOURCESDK_SCHEMA_PRE_INCLUDE
	tier0/platform.h
	tier0/memalloc.h
	mathlib/vector.h
	entity2/entityidentity.h
	entity2/entityinstance.h
)

add_library(${SOURCESDK_SCHEMA_NAME} STATIC)
add_library(${PROJECT_NAME}::${SOURCESDK_SCHEMA_NAME} ALIAS ${SOURCESDK_SCHEMA_NAME})

set_target_properties(${SOURCESDK_SCHEMA_NAME} PROPERTIES
	C_STANDARD 17
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF

	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

if(WINDOWS)
	if(SOURCESDK_MSVC_RUNTIME_LIBRARY)
		set_target_properties(${SOURCESDK_SCHEMA_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	endif()
elseif(MACOS)
	set_target_properties(${SOURCESDK_SCHEMA_NAME} PROPERTIES OSX_ARCHITECTURES "x86_64")
endif()

target_link_libraries(${SOURCESDK_SCHEMA_NAME} PRIVATE ${PROJECT_NAME})

# The transfer bodies of the SDK classes are hand-written or live in the game
sourcesdk_target_schema(${SOURCESDK_SCHEMA_NAME}
	SOURCES ${SOURCESDK_SCHEMA_SOURCE_FILES}
	PROJECT_NAME ${PROJECT_NAME}
	PRE_INCLUDE ${SOURCESDK_SCHEMA_PRE_INCLUDE}
	NO_CODEGEN_TAGS
)
