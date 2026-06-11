#ifndef ENTITYIO_H
#define ENTITYIO_H

#pragma once

#include "datamap.h"
#include "string_t.h"
#include "variant.h"
#include "tier0/bufferstring.h"
#include "schemasystem/schematypes.h"
#include "entityhandle.h"
#include "entitysystem.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

struct EntityIOConnectionDesc_t
{
	string_t m_targetDesc;
	string_t m_targetInput;
	string_t m_valueOverride;
	CEntityHandle m_hTarget;
	EntityIOTargetType_t m_nTargetType;
	int32 m_nTimesToFire;
	float m_flDelay;
};

COMPILE_TIME_ASSERT( sizeof( EntityIOConnectionDesc_t ) == 40 );

struct EntityIOConnection_t : EntityIOConnectionDesc_t
{
	CBufferString m_paramMap;
	bool m_bForwardAllArgs;

private:
	byte m_unk[7];

public:
	bool m_bMarkedForRemoval;
	EntityIOConnection_t *m_pNext;
};

COMPILE_TIME_ASSERT( sizeof( EntityIOConnection_t ) == 80 );

struct EntityIOOutputDesc_t
{
	const char* m_pName;
	uint32 m_nFlags;
	uint32 m_nOutputOffset;
};

class CEntityIOOutput
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void Save( CKV3TransferSaveContext *pContext ) = 0;
	virtual void Load( CKV3TransferLoadContext *pContext ) = 0;

public:
	const EntityIOConnection_t *GetConnections() const { return m_pConnections; }
	const EntityIOOutputDesc_t *GetDescription() const { return m_pDesc; }

private:
	EntityIOConnection_t *m_pConnections;
	EntityIOOutputDesc_t *m_pDesc;
};

COMPILE_TIME_ASSERT( sizeof( CEntityIOOutput ) == 24 );

template < typename T >
class CEntityOutputTemplate : public CEntityIOOutput
{
public:
	fieldtype_t ValueFieldType() const { return VariantDeduceType( T ); }
	T &Get() { return m_Value; }
	const T &Get() const { return m_Value; }

protected:
	T m_Value;
};

COMPILE_TIME_ASSERT( sizeof( CEntityOutputTemplate< int32 > ) == 32 );

struct InputData_t
{
	CBaseEntity *pActivator;
	CBaseEntity *pCaller;
	variant_t value;
	int nOutputID;
};

#endif // ENTITYIO_H
