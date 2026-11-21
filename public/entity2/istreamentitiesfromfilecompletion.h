#ifndef ISTREAMENTITIESFROMFILECOMPLETION_H
#define ISTREAMENTITIESFROMFILECOMPLETION_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "entityhandle.h"

//-----------------------------------------------------------------------------
// Told by the entity save/restore code when entities streamed in from a save
// file have been restored (CEntity2SaveRestore::EndRestoreEntities)
//-----------------------------------------------------------------------------
abstract_class IStreamEntitiesFromFileCompletion
{
public:
	virtual void OnStreamEntitiesFromFileCompleted( CUtlVector< CEntityHandle > &entities ) = 0;
};

#endif // ISTREAMENTITIESFROMFILECOMPLETION_H
