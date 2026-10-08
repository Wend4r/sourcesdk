//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Manager of named memory blocks shared between the client and the server
//
//=============================================================================//

#ifndef ISINGLEPLAYERSHAREDMEMORY_H
#define ISINGLEPLAYERSHAREDMEMORY_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <ispsharedmemory.h>

abstract_class ISinglePlayerSharedMemory : public IAppSystem
{
public:
	virtual ISPSharedMemory *GetSinglePlayerSharedMemorySpace( const char *szName, int nEntNum ) = 0;
};

#endif // ISINGLEPLAYERSHAREDMEMORY_H
