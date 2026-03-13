#ifndef ENTITYNETWORK_H
#define ENTITYNETWORK_H

#if _WIN32
#pragma once
#endif

#include "tier1/utlmap.h"
#include "entityidentity.h"
#include "entitysystem.h"

class ServerClass;

class IEntity2Networkables
{
public:
	virtual ~IEntity2Networkables() = 0;

	virtual bool GetEntity2Networkable( CEntityIndex nEntryIndex, Entity2Networkable_t *info ) = 0;
	virtual CUtlMap<int, Entity2Networkable_t> &GetEntity2Networkables( void ) const = 0;
	virtual void CreateEntity2Networkable() = 0;
	virtual void Unk01() = 0;
	virtual void Unk02() = 0;
	virtual ServerClass **GetClassList() = 0;
};

class CEntity2NetworkClasses : public IEntity2Networkables, public IEntityListener
{
public:
	ServerClass **m_ppClassList;
	void *m_pPad[2];
	CUtlMap<int,Entity2Networkable_t> m_NetworkedEntities2;
};


#endif // ENTITYNETWORK_H
