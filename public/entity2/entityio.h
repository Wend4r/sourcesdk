#ifndef ENTITYIO_H
#define ENTITYIO_H

#pragma once

#include "datamap.h"
#include "string_t.h"
#include "variant.h"
#include "tier0/bufferstring.h"
#include "tier1/keyvalues3.h"
#include "schemasystem/schematypes.h"
#include "entitypulsecallcontext.h"
#include "entityhandle.h"
#include "entitytypes.h"
#include "spawngrouptypes.h"
#include "tier0/threadtools.h"
#include "tier1/utlsymbollarge.h"

#define ENTITY_INVLAID_OUTPUT_OFFSET ( ~0 )

class CEntityInstance;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CEntityIOOutput;

enum EntityIOTargetType_t
{
	ENTITY_IO_TARGET_INVALID = -1,
	ENTITY_IO_TARGET_CLASSNAME = 0,
	ENTITY_IO_TARGET_CLASSNAME_DERIVES_FROM = 1,
	ENTITY_IO_TARGET_ENTITYNAME = 2,
	ENTITY_IO_TARGET_CONTAINS_COMPONENT = 3,
	ENTITY_IO_TARGET_SPECIAL_ACTIVATOR = 4,
	ENTITY_IO_TARGET_SPECIAL_CALLER = 5,
	ENTITY_IO_TARGET_EHANDLE = 6,
	ENTITY_IO_TARGET_ENTITYNAME_OR_CLASSNAME = 7,
};

struct EntityIOQueuePrioritizedEvent_t
{
	EntityIOQueuePrioritizedEvent_t() = default;
	EntityIOQueuePrioritizedEvent_t( WorldGroupId_t worldGroupId, GameTime_t flFireTime, CEntityInstance *pActivator, CEntityInstance *pCaller );

	void Init( const CVariant &value, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap );

	WorldGroupId_t m_WorldGroupId;
	GameTime_t m_flFireTime;
	EntityIOTargetType_t m_targetType;
	CUtlSymbolLarge m_pTarget;
	CUtlSymbolLarge m_pTargetInput;
	CEntityHandle m_hActivator;
	CEntityHandle m_hCaller;
	CEntityHandle m_hEntTarget; // a pointer to the entity to target; overrides m_pTarget

	CVariant m_variantValue; // variable-type parameter

	CPulseArgumentPack m_PulseArguments;
	CPulseInputParamMap m_paramMap;

	EntityIOQueuePrioritizedEvent_t *m_pNext;
	EntityIOQueuePrioritizedEvent_t *m_pPrev;
};

class CEventQueue
{
public:
	void AddEvent( EntityIOQueuePrioritizedEvent_t *pEvent );
	void RemoveEvent( EntityIOQueuePrioritizedEvent_t *pEvent );

public:
	CAtomicMutex m_Mutex;
	EntityIOQueuePrioritizedEvent_t m_Events;
};

// Output registered by an entity class
struct EntOutput_t
{
private:
	uint8 m_pad0000[ 16 ];

public:
	// Object that holds the output, such as a component; null when the entity holds it
	void *( *m_pfnGetOutputOwner )( CEntityInstance *pEntity );

	uint32 : 1, m_nOutputOffset : 31;

	CEntityIOOutput *GetOutput( CEntityInstance *pEntity ) const;
};

struct CEntityIOInputFunction
{
	typedef void (*InputAdapterFunc_t)(const CUtlAbstractDelegate *, CEntityInstance *, CEntityInstance *, CEntityInstance *, void *, const CVariant *);

	const char *m_pName;
	uint32 m_nFlags;
	void *m_pContext;
	CUtlAbstractDelegate m_delegate;
	InputAdapterFunc_t m_adapterFunc;
};

struct EntInput_t : CEntityIOInputFunction
{
};

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
	// TODO(@Wend4r): Implement kv3lib stuff
	CPulseInputParamMap m_paramMap;

	// The connection is removed the next time its output fires
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

// Observers of entity I/O
class IEntityIONotify
{
public:
	virtual void OnConnectionFired( CEntityInstance *pActivator, CEntityInstance *pCaller, const EntityIOConnection_t *pConnection, const CPulseArgumentPack *pArgs ) = 0;
	virtual void Unk_01() = 0;
	virtual void OnConnectionRemoved( CEntityInstance *pActivator, CEntityInstance *pCaller, const EntityIOConnection_t *pConnection ) = 0;
	virtual void OnOutputFired( CEntityInstance *pActivator, CEntityInstance *pCaller, const EntityIOOutputDesc_t *pDesc, const CPulseArgumentPack *pArgs, float flDelay ) = 0;
};

class CEntityIOOutput
{
public:
	// TODO(@Wend4r): Implement schemacompiler2 & kv3lib stuff
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;

public:
	~CEntityIOOutput()
	{
		for ( EntityIOConnection_t *pConnection = m_pConnections; pConnection; )
		{
			EntityIOConnection_t *pNext = pConnection->m_pNext;

			Release( pConnection );

			pConnection = pNext;
		}

		m_pConnections = nullptr;
	}

	// The output takes ownership of pConnection
	void AddConnection( EntityIOConnection_t *pConnection )
	{
		pConnection->m_pNext = m_pConnections;
		m_pConnections = pConnection;
	}

	// Returns the first connection that targets hTarget
	EntityIOConnection_t *FindConnection( CEntityHandle hTarget )
	{
		EntityIOConnection_t *pConnection = m_pConnections;

		while ( pConnection && pConnection->m_hTarget != hTarget )
			pConnection = pConnection->m_pNext;

		return pConnection;
	}

	void MarkConnectionForRemoval( EntityIOConnection_t *pConnection )
	{
		if ( pConnection )
			pConnection->m_bMarkedForRemoval = true;
	}

	EntityIOConnection_t *GetConnections() { return m_pConnections; }
	const EntityIOConnection_t *GetConnections() const { return m_pConnections; }

	EntityIOOutputDesc_t *GetDescription() { return m_pDesc; }
	const EntityIOOutputDesc_t *GetDescription() const { return m_pDesc; }

	void FireOutput( CEntityInstance *pActivator, CEntityInstance *pCaller, const CVariant &value, float flDelay );

	// Queues an event per connection and removes connections that fired their last time
	void FireOutputInternal( CEntityInstance *pActivator, CEntityInstance *pCaller, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap, const CVariant *pValue, float flDelay );

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

class CBaseDynamicIOSignature;

// Dynamic outputs of a pulse graph instance, one per output of the signature
class CDynamicIOInstance
{
public:
	// A connection added for an output the signature does not have
	struct PendingConnection_t
	{
		CUtlString m_sOutputName;
		EntityIOConnectionDesc_t m_desc;
		CPulseInputParamMap m_paramMap;
	};

	CEntityIOOutput *FindOutput( const char *pszName ); // Case-insensitive
	void FindOutputs( const char *pszName, CUtlVector< CEntityIOOutput * > &outputs );
public:
	CBaseDynamicIOSignature *m_pSignature;
	CUtlVector< CEntityIOOutput > m_outputs;
	CUtlVector< PendingConnection_t > m_pendingConnections;

	// Links in the instance list of the signature
	CDynamicIOInstance *m_pPrev;
	CDynamicIOInstance *m_pNext;
};

struct InputData_t
{
	CEntityInstance *pActivator;
	CEntityInstance *pCaller;
	variant_t value;
	int nOutputID;
};

#endif // ENTITYIO_H
