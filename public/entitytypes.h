#ifndef ENTITYTYPES_H
#define ENTITYTYPES_H

#if _WIN32
#pragma once
#endif

#include "tier0/basetypes.h"
#include "tier0/smartptr.h"

#include "schemacompiler/schemaon.h"

class CEntityInstance;

enum EntityFlags_t : uint32
{
	EF_NONE = 0,
	EF_IS_INVALID_EHANDLE = 1 << 0,
	EF_SPAWN_IN_PROGRESS = 1 << 1,
	EF_IN_STAGING_LIST = 1 << 2,
	EF_IN_POST_DATA_UPDATE = 1 << 3,
	EF_DELETE_IN_PROGRESS = 1 << 4,
	EF_IN_STASIS = 1 << 5,
	EF_IS_ISOLATED_ALLOCATION_NETWORKABLE = 1 << 6,
	EF_IS_DORMANT = 1 << 7,
	EF_IS_PRE_SPAWN = 1 << 8,
	EF_MARKED_FOR_DELETE = 1 << 9,
	EF_IS_CONSTRUCTION_IN_PROGRESS = 1 << 10,
	EF_IS_ISOLATED_ALLOCATION = 1 << 11,
	EF_HAS_BEEN_UNSERIALIZED = 1 << 12,
	EF_IS_SUSPENDED = 1 << 13,
	EF_IS_ANONYMOUS_ALLOCATION = 1 << 14,
	EF_SUSPEND_OUTSIDE_PVS = 1 << 15,
};
DEFINE_ENUM_BITWISE_OPERATORS( EntityFlags_t );

schema struct GameTime_t
{
public:
	TYPEMETA( MIsBoxedFloatType );
	DECLARE_SCHEMA_DATA_CLASS( GameTime_t );

	GameTime_t( float value = 0.0f ) : m_Value( value ) {}

	float GetTime() const { return m_Value; }
	void SetTime( float value ) { m_Value = value; }

	operator float() const { return m_Value; }

private:
	float m_Value;
};

schema struct GameTick_t
{
public:
	TYPEMETA( MIsBoxedIntegerType );
	DECLARE_SCHEMA_DATA_CLASS( GameTick_t );

	GameTick_t( int32 value = 0 ) : m_Value( value ) {}

	int32 GetTick() const { return m_Value; }
	void SetTick( int32 value ) { m_Value = value; }

	operator int32() const { return m_Value; }

private:
	int32 m_Value;
};

enum ActivateType_t
{
	ACTIVATE_TYPE_INITIAL_CREATION = 0,
	ACTIVATE_TYPE_DATAUPDATE_CREATION,
	ACTIVATE_TYPE_ONRESTORE,
};

enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
	DATA_UPDATE_POST_UPDATE,
};

schema enum EntityDormancyType_t
{
	ENTITY_NOT_DORMANT = 0,
	ENTITY_DORMANT,
	ENTITY_SUSPENDED,
};

enum NetworkStateChangeType_t
{
	NETWORK_STATE_CHANGE_FULL = 0, // The whole entity changed.
	NETWORK_STATE_CHANGE_FIELDS, // The fields at m_LocalOffsets changed.
};

enum NetworkStateChangeFlags_t
{
	NETWORK_STATE_CHANGED = 1 << 0, // Some fields changed.
	NETWORK_STATE_FULL_CHANGE = 1 << 1, // The whole entity changed (FL_FULL_EDICT_CHANGED).
};

enum EntityNetworkingMode_t : uint32
{
	ENTITY_NETWORKING_MODE_DEFAULT = 0,
	ENTITY_NETWORKING_MODE_NETWORKED,
	ENTITY_NETWORKING_MODE_NOT_NETWORKED,
};

enum EntityComponentHelperFlags_t : uint32
{
	ENTITY_COMPONENT_HELPER_ALLOCATE = 1 << 0, // Allocate is called when an entity of the class is created
	ENTITY_COMPONENT_HELPER_FREE = 1 << 1, // Free is called when the instance of an entity of the class is destroyed
};
DEFINE_ENUM_BITWISE_OPERATORS( EntityComponentHelperFlags_t );

// The owner entity is not owned: nothing is released when the pointer changes or goes away
class CEntityOwnerAccessor
{
public:
	template< class T >
	static void AddRef( T *pObj ) {}

	template< class T >
	static void Release( T *pObj ) {}
};

// Captured at construction from the thread-local pointer of the entity under construction, which the entity spawn helper sets.
class CEntityOwnerPtr : public CSmartPtr< CEntityInstance, CEntityOwnerAccessor >
{
public:
	// ...
};

#include "schemacompiler/schemaoff.h"

#endif // ENTITYTYPES_H
