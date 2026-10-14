//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ILOOPMODE_H
#define ILOOPMODE_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <tier0/eventdispatcher.h>
#include <inputsystem/inputenums.h>
#include <keyvalues.h>
#include <splitscreenslot.h>
#include <netmessages.pb.h>
#include "rendersystem/rendersystemtypes.h"
#include "engine2/isource2serverconfig.h"

#include "schemacompiler/schemaon.h"

class ISource2WorldSession;
class ISceneView;
class IPrerequisite;

DECLARE_POINTER_HANDLE(SwapChainHandle_t);
DECLARE_POINTER_HANDLE(HSceneViewRenderTarget);
DECLARE_POINTER_HANDLE(ActiveLoop_t);

enum ClientServerMode_t : int32
{
	CLIENTSERVERMODE_NONE = 0,
	CLIENTSERVERMODE_SERVER,
	CLIENTSERVERMODE_CLIENT,
	CLIENTSERVERMODE_LISTENSERVER,
};

struct RHBackColorBuffer_t
{
	ISceneView *m_pView;
	HSceneViewRenderTarget m_hBackColorBuffer;
};


schema struct EngineLoopState_t
{
	DECLARE_SCHEMA_DATA_CLASS( EngineLoopState_t );

	noschema PlatWindow_t m_hWnd;
	noschema SwapChainHandle_t m_hSwapChain;
	noschema InputContextHandle_t m_hInputContext;
	int m_nPlatWindowWidth;
	int m_nPlatWindowHeight;
	int m_nRenderWidth;
	int m_nRenderHeight;
};

enum LoopModeType_t : int32
{
	LOOP_MODE_TYPE_INVALID = -1,
	LOOP_MODE_TYPE_SIMPLE = 0,
	LOOP_MODE_TYPE_CLIENT,
	LOOP_MODE_TYPE_SERVER,
	LOOP_MODE_TYPE_HOST,

	LOOP_MODE_TYPE_COUNT,
};

enum InputHandlerResult_t : int32
{
  	IHR_INPUT_IGNORED = 0,
  	IHR_INPUT_CONSUMED,
};

abstract_class ILoopType
{
public:
	virtual void AddEngineService(const char* pszName) = 0;
};

template <typename T>
struct CBaseCmdKeyValues : T
{
	KeyValues* m_pKeyValues;
};

class GameSessionConfiguration_t : public CBaseCmdKeyValues<CSVCMsg_GameSessionConfiguration>
{
};

abstract_class IPrerequisiteRegistry
{
public:
	virtual void RegisterPrerequisite( IPrerequisite * ) = 0;
};

abstract_class ILoopModePrerequisiteRegistry : public IPrerequisiteRegistry
{
public:
	virtual void UnregisterPrerequisite( IPrerequisite * ) = 0;
};

abstract_class ILoopMode
{
public:
	virtual bool LoopInit( KeyValues *pKeyValues, ILoopModePrerequisiteRegistry *pRegistry ) = 0;
	virtual void LoopShutdown( void ) = 0;
	virtual void OnLoopActivate( const EngineLoopState_t &state, CEventDispatcher<CEventIDManager_Default> *pEventDispatcher) = 0;
	virtual void OnLoopDeactivate( const EngineLoopState_t &state, CEventDispatcher<CEventIDManager_Default> *pEventDispatcher ) = 0;
	virtual void RegisterEventMap( CEventDispatcher<CEventIDManager_Default> *pEventDispatcher, EventMapRegistrationType_t nRegistrationType) = 0;
	virtual InputHandlerResult_t HandleInputEvent( const InputEvent_t &event, CSplitScreenSlot nSplitScreenPlayerSlot ) = 0;
	virtual RHBackColorBuffer_t AddViewsToSceneSystem( const EngineLoopState_t &state, double flRenderTime, double flRealTime,
		const RenderViewport_t &viewport, const RHBackColorBuffer_t &backColorBuffer ) = 0;
	// AMNOTE: True only while loading a level, unless -favor_consistent_framerate is set; the engine then skips frames while presents are outstanding
	virtual bool unk001( void ) = 0;
};

abstract_class ILoopModeFactory
{
public:
	virtual bool Init( ILoopType *pLoop ) =0;
	virtual void Shutdown( void ) =0;
	virtual ILoopMode *CreateLoopMode( void ) =0;
	virtual void DestroyLoopMode( ILoopMode *pLoopMode ) =0;
	virtual LoopModeType_t GetLoopModeType( void ) const =0;
};

#include "schemacompiler/schemaoff.h"

#endif // ILOOPMODE_H
