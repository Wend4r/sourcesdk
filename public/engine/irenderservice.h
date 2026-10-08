//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service that renders the engine window
//
//=============================================================================//

#ifndef IRENDERSERVICE_H
#define IRENDERSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <IEngineService.h>
#include <splitscreenslot.h>
#include <spawngrouptypes.h>

class IRenderHandler;
class ISceneWorld;
struct EventClientPreOutput_t;

abstract_class IRenderService : public IEngineService
{
public:
	// Handlers are kept sorted by descending nPriority
	virtual void InstallRenderHandler( const char *pszName, int nPriority, IRenderHandler *pHandler ) = 0;
	virtual void RemoveRenderHandler( IRenderHandler *pHandler ) = 0;

	virtual void ChangeVideoMode( RenderDeviceInfo_t &info ) = 0;

	virtual CUtlStringToken GetRenderingPipeline() = 0;

	virtual bool unk027() = 0;

	virtual void GetSplitScreenViewport( const void *pViewport, int nSlot, void *pOutViewport ) = 0;

	virtual ISceneWorld *GetSceneWorld() = 0;
	virtual bool IsFullyLoadedForPlayer( WorldGroupId_t hWorldGroupId, CSplitScreenSlot nSlot ) = 0;

	virtual void *Unk_FindOrCreateWorldSession( void *p ) = 0;

	virtual void LaunchAsyncBeginRenderingViewsJob( const EventClientPreOutput_t &event, bool bAddViews ) = 0;
	virtual bool FinishAsyncBeginRenderingViewsJob( bool *pbAddedViews ) = 0;

	virtual bool ShouldRenderAllWindows() = 0;
	// Sets m_bRenderCoordination_RequestAllWindowsRenderNextFrame
	virtual void RequestAllWindowsRenderNextFrame() = 0;
	virtual void ForceRenderForNextFrames() = 0;
	// Outside tools mode always true
	virtual bool ShouldRender() = 0;

	// Flag set by the client output event handler
	virtual bool unk038() = 0;
};

#endif // IRENDERSERVICE_H
