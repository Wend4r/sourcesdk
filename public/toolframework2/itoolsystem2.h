//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Interface of a Workshop Tools module, loaded and driven by the tool framework
//
//===========================================================================//

#ifndef ITOOLSYSTEM2_H
#define ITOOLSYSTEM2_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"
#include "appframework/iappsystem.h"

class KeyValues3;

// What the tool framework asks a tool to open ( IToolSystem2::OpenAssets ). The assets are asset system
// objects ( opaque here ). Named by what it holds.
struct ToolAssetRequest_t
{
	// 0: show the tool with no assets, 1: open m_Assets, 3: open the single asset of m_Assets at m_pSubasset
	int m_nRequestType;
	CUtlVector< const void * > m_Assets;

	// The subasset of the asset ( or NULL ) when m_nRequestType is 3
	const void *m_pSubasset;

	// Named by what it holds: strings the caller frees after the call; the tool framework leaves it empty
	CUtlVector< CUtlString > m_Arguments;
};

// A tool module exports this interface under its m_InterfaceName from bin/sdkenginetools.txt, or under
// TOOLSYSTEM2_INTERFACE_VERSION when the tool has none
abstract_class IToolSystem2 : public IAppSystem
{
public:
	// Called once after the module is loaded and added to the tool framework's app systems: factory is the
	// tool framework's interface factory, nToolIndex the tool's index in m_EngineTools. A false return unloads
	// the tool ( "Failed to initialize tool system" )
	virtual bool InitToolSystem( CreateInterfaceFn factory, uint8 nToolIndex ) = 0;

	virtual void OpenAssets( const ToolAssetRequest_t &request ) = 0;

	// Called every frame between the PreFrame and PostFrame tool frame listeners
	virtual void RunFrame() = 0;

	// Named by what it does: the answer to a quit request; the tool framework returns the first non-zero
	// answer of the loaded tools ( 0 quits, 1 cancels the quit, anything else asks again next frame )
	virtual int QueryQuit() = 0;

	// Named by what it does: the client calls it on every loaded tool with ( 0, true ) when its game loop
	// mode activates and ( 0, false ) when it deactivates
	virtual void OnGameLoopModeChanged( uint8 nLoopModeType, bool bActive ) = 0;

	// Named by what it does: a request sent to the tool by name ( IToolFramework2::SendRequestToTool )
	virtual void HandleRequest( KeyValues3 *pRequest ) = 0;
};

#endif // ITOOLSYSTEM2_H
