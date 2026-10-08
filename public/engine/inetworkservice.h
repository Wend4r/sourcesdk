//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service that owns the network sockets
//
//=============================================================================//

#ifndef INETWORKSERVICE_H
#define INETWORKSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <IEngineService.h>

abstract_class INetworkService : public IEngineService
{
public:
	virtual void OpenSockets() = 0;
};

#endif // INETWORKSERVICE_H
