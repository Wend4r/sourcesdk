//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service that exposes game client and game server functionality to the tools
//
//=============================================================================//

#ifndef ITOOLSERVICE_H
#define ITOOLSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <IEngineService.h>
#include <splitscreenslot.h>
#include <tier0/utlstring.h>
#include <tier1/utlvector.h>

class CNavData;
class INavListener;
class IToolGameSimulationAPI;
class CEntityClass;
class ISceneWorld;
class IScreenshotCallback;

abstract_class IToolService : public IEngineService
{
public:
	virtual void ExecuteCommand( const char *pszCommand ) = 0;

	virtual const char *GetMapName() = 0;
	// Client signon state is SIGNONSTATE_FULL
	virtual bool IsInGame() = 0;

	virtual bool IsPaused() = 0;
	virtual bool GetNavMeshData( CNavData *pNavMeshData ) = 0;
	virtual void SetNavMeshData( const CNavData *pNavMeshData ) = 0;
	virtual void RegisterNavListener( INavListener *pNavListener ) = 0;
	virtual void UnregisterNavListener( INavListener *pNavListener ) = 0;
	virtual void GetAnimationActivityList( CUtlVector< CUtlString > &activityList ) = 0;
	virtual void GetAnimationEventList( CUtlVector< CUtlString > &eventList ) = 0;
	virtual IToolGameSimulationAPI *GetToolGameSimulationAPI() = 0;

	virtual void *GetToolClientSimulationAPI() = 0;

	// Forward to game client methods that are not identified yet
	virtual void unk035() = 0;
	virtual void unk036() = 0;
	virtual void *unk037() = 0;
	virtual void unk038() = 0;
	virtual void unk039() = 0;
	virtual void unk040() = 0;
	virtual void unk041() = 0;
	virtual void unk042() = 0;
	virtual void unk043() = 0;
	virtual void unk044() = 0;
	virtual void unk045() = 0;
	virtual void unk046() = 0;
	virtual void *unk047() = 0;
	// 1.0 without a game client
	virtual float unk048() = 0;
	virtual float unk049() = 0;
	virtual void unk050() = 0;
	virtual bool unk051() = 0;
	virtual void unk052() = 0;
	virtual void unk053() = 0;
	virtual void unk054() = 0;
	virtual bool unk055() = 0;
	virtual void unk056() = 0;

	virtual void *GetEconItemSystem() = 0;

	virtual void *unk058() = 0;

	virtual void unk059() = 0;
	virtual void unk060() = 0;

	virtual void GetHitGroupEnumInfo( CUtlVector< int > &values, CUtlVector< CUtlString > &names ) = 0;

	// Forward to the game client
	virtual void *unk062() = 0;
	// Returns an 8-byte value by value
	virtual void unk063() = 0;
	// The game client reports a valid local player entity handle
	virtual bool unk064() = 0;

	virtual ISceneWorld *GetSceneWorld() = 0;
	virtual void *unk066( CSplitScreenSlot nSlot ) = 0;
	// -1.0 without a connected client
	virtual float unk067( CSplitScreenSlot nSlot ) = 0;

	// Forwards to the game client
	virtual void unk068() = 0;

	virtual const char *GetVDataClassName( const char *pName, const char *pScopeFile ) = 0;
	virtual const char *GetVDataClassNameByDataType( const char *pName, const char *pGenericDataType ) = 0;
	virtual const CUtlVector< CUtlString > &GetSubclassNamesInScope( const char *pScopeFile ) = 0;
	virtual const CUtlVector< CUtlString > &GetSubclassNamesInScopeByDataType( const char *pGenericDataType ) = 0;
	virtual void GetAllSubclassNames( CUtlVector< CUtlString > &names ) = 0;
	virtual const char *GetSubclassDesignerName( const char *pSubclassName ) = 0;
	virtual const CUtlVector< CUtlString > &GetDesignerNamesForClass( const char *pClassName ) = 0;
	virtual void GetSubclassAutoCompleteList( const void *pUnk, CUtlVector< const char * > &completions ) = 0;

	// Forwards to the game client
	virtual void unk077() = 0;

	// "" without the game server
	virtual const char *GetNativeClassForScriptClass( const char *pScriptClassName ) = 0;
	virtual CEntityClass *GetScriptClassForDesignerName( const char *pDesignerName ) = 0;
	virtual bool IsScriptClassDerivedFrom( const char *pDesignerName, const char *pBaseName ) = 0;

	// Forwards to the game client
	virtual void unk081() = 0;

	virtual void RequestScreenshot( IScreenshotCallback *pCallback, void *pContext ) = 0;
	virtual void RequestJpegScreenshot( IScreenshotCallback *pCallback, int nQuality, void *pContext ) = 0;
	virtual void GenerateScreenshotFileName( const char *pszFileName, const char *pszExtension, CUtlString &sFileName ) = 0;

	// Forward to game server methods that are not identified yet
	virtual void unk085() = 0;
	virtual void unk086() = 0;
	virtual void unk087() = 0;
	virtual void unk088() = 0;

	// Forward to the game client; unk090 does nothing for 0
	virtual void *unk089() = 0;
	virtual void unk090( uint16 nUnk ) = 0;
};

#endif // ITOOLSERVICE_H
