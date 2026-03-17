#ifndef ENTITYNETWORK_H
#define ENTITYNETWORK_H

#if _WIN32
#pragma once
#endif

#include "tier1/utlmap.h"
#include "entityidentity.h"
#include "entitysystem.h"

class ServerClass;
class ClientClass;
class CNetworkTransmitComponent;
class CChangeInfoAccessor;
class CEntityInstancePolymorphicMetadataHelper;

struct Entity2Networkable_t
{
	union
	{
		ServerClass *m_pServerClass;
		ClientClass *m_pClientClass;
	} u;

	CEntityIdentity *m_pEntity;
	CEntityInstance *m_pEntityInstance;
	CEntityInstance *m_pParentEntity;
	const char *m_pszDesignerName;
	const char* m_pszCPPClassname;
	CNetworkTransmitComponent *m_pTransmitComponent;
	CChangeInfoAccessor *m_pChangeAccessor;
	uint32 *m_pnChangeFlags;
	CEntityInstancePolymorphicMetadataHelper *m_pMetadataHelper;

	int m_nSerialNumber;
	SpawnGroupHandle_t m_SpawnGroupHandle;
	CEntityHandle m_hEntity;
};
static_assert( sizeof( Entity2Networkable_t ) == 96 );

class IEntity2Networkables
{
public:
	virtual ~IEntity2Networkables() = 0;

	virtual bool GetEntity2Networkable( CEntityIndex nEntryIndex, Entity2Networkable_t *info ) = 0;
	virtual CUtlMap< int, Entity2Networkable_t > &GetEntity2Networkables( void ) const = 0;
	virtual void Unk01() = 0;
	virtual void Unk02() = 0;
	virtual ServerClass **GetClassList() = 0;

	virtual void OnEntityCreated( CEntityInstance *pEntity ) = 0; // Entity2Networkable_t creation into UtlRBTreeNode_t.
	virtual void OnEntityDeleted( CEntityInstance *pEntity ) = 0;
	virtual void OnEntityParentChanged( CEntityInstance *pEntity, CEntityInstance *pNewParent ) = 0;
};

class CEntity2NetworkClasses : public IEntity2Networkables, public IEntityListener
{
public:
	ServerClass *m_pClassList;
	void *m_pPad[2];
	CUtlMap< int, Entity2Networkable_t > m_NetworkedEntities2;
};
static_assert( sizeof( CEntity2NetworkClasses ) == 72 );

#endif // ENTITYNETWORK_H
