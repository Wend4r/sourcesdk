#ifndef GAME_SAVERESTORE_H
#define GAME_SAVERESTORE_H

#ifdef _WIN32
#pragma once
#endif

#include "entityhandle.h"
#include "tier0/utlvector.h"

// Notified by the entity save/restore system when it finishes streaming entities from a file
abstract_class IStreamEntitiesFromFileCompletion
{
public:
	virtual void OnStreamEntitiesFromFileCompleted( CUtlVector< CEntityHandle > &entities ) = 0;
};

#endif // GAME_SAVERESTORE_H
