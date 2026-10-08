//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Registry of callbacks that queue debug drawing into a call queue
//
//=============================================================================//

#ifndef IDEBUGDRAWQUEUEMANAGER_H
#define IDEBUGDRAWQUEUEMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <tier1/utldelegate.h>

class CCallQueue;

typedef CUtlDelegate< void ( CCallQueue * ) > DebugDrawQueueCallback_t;

abstract_class IDebugDrawQueueMgr : public IAppSystem
{
public:
	virtual void AddQueueCallback( const DebugDrawQueueCallback_t &callback ) = 0;

	virtual void RemoveQueueCallback( const DebugDrawQueueCallback_t &callback ) = 0;

	// Invokes every registered callback with pCallQueue
	virtual void QueueDebugDraws( CCallQueue *pCallQueue ) = 0;
};

#endif // IDEBUGDRAWQUEUEMANAGER_H
