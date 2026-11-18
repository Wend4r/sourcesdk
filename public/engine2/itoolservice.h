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

#include <engine/IEngineService.h>
#include <splitscreenslot.h>
#include <entityhandle.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>

class CNavData;
class INavListener;
class IToolGameSimulationAPI;
class CEntityClass;
class ISceneWorld;
class IScreenshotCallback;
class CEconItemSystem;
class Vector;

// AMNOTE: Names come from the CToolService vtable of Dota 2 2018 (libengine2.dylib symbols) and from the
// ISource2Client / ISource2Server methods the CS2 implementation forwards to.
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

	// Forward to the methods of ISource2Client with the same names
	virtual void UpdateProjectedTexture( const void *pInfo, void **phTexture ) = 0;
	virtual void DeleteProjectedTexture( void *hTexture ) = 0;
	virtual void *UpdateDeferredLight( ISceneWorld *pWorld, const void *pDesc, void **phLight ) = 0;
	virtual void DeleteDeferredLight( void *hLight ) = 0;
	virtual void *UpdateStandardLight( ISceneWorld *pWorld, const void *pDesc, void **phLight ) = 0;
	virtual void DeleteStandardLight( void *hLight ) = 0;
	virtual void *UpdateTransformedLight( ISceneWorld *pWorld, const void *pDesc, const void *pTransform, void **phLight ) = 0;
	virtual void DeleteTransformedLight( void *hLight ) = 0;
	virtual void GetEconItemNamesForModel( const char *pszModelName, bool bExcludeItemSets, bool bExcludeLockedItems, CUtlVector< CUtlString > &names ) = 0;
	virtual void GetEconItemNamesForCharacter( const char *pszCharacterName, bool bExcludeItemSets, bool bExcludeLockedItems, CUtlVector< CUtlString > &names ) = 0;
	virtual void GetEconItemsInfoForModel( const char *pszModelName, const char *pszSlot, bool bExcludeItemSets, bool bExcludeLockedItems, bool bExcludeDefaultItems, void *pItems ) = 0;
	virtual void GetEconItemsInfoForCharacter( const char *pszCharacterName, const char *pszSlot, bool bExcludeItemSets, bool bExcludeLockedItems, bool bExcludeDefaultItems, void *pItems ) = 0;
	virtual bool GetDefaultControlPointAutoUpdates( const char *pszParticleName, void *pAutoUpdates ) = 0;
	// 1.0 without a game client
	virtual float GetDefaultScaleForModel( const char *pszModelName ) = 0;
	virtual float GetDefaultScaleForCharacter( const char *pszCharacterName ) = 0;
	virtual void GetCharacterNameForModel( const char *pszModelName, int nUnused, void *pCharacterName ) = 0;
	virtual bool GetModelNameForCharacter( const char *pszCharacterName, int nIndex, CUtlString *pModelName ) = 0;
	virtual void GetCharacterList( CUtlVector< CUtlString > &characters ) = 0;
	virtual void GetDefaultChoreoDirForModel( const char *pszModelName, CUtlString *pDirectory ) = 0;
	virtual void UpdateGlobalLightFromLightInfos( void *pGlobalLight, const void *pLightInfos, const Vector &vecDirection, int nLightType, float flBrightness, float flRange, float flAngle ) = 0;
	virtual bool GetGlobalLightParameters( int *pLightType, float *pBrightness, float *pRange ) = 0;
	// ISource2Client::GetEconItemSystem
	virtual CEconItemSystem *GetEconItemSystem_Client() = 0;

	virtual CEconItemSystem *GetEconItemSystem_Server() = 0;

	// ISource2Client::GetModelCombineInfo
	virtual void GetModelCombineInfo( const void *hModel, void *pCombinedModel, int *pCombineIndex ) = 0;

	// Named by what they do: dispatch EventClientSceneSystemThreadStateChange_t with true and with false
	virtual void OnSceneSystemThreadsStarted() = 0;
	virtual void OnSceneSystemThreadsStopped() = 0;

	virtual void GetHitGroupEnumInfo( CUtlVector< int > &values, CUtlVector< CUtlString > &names ) = 0;

	// Forward to ISource2Client
	virtual void *GetToolClientEntityInterface() = 0;
	virtual CEntityHandle GetToolClientSelectedEntity() = 0;
	// Named by what it does: the game client reports a valid local player pawn handle
	virtual bool HasLocalPlayerPawn() = 0;

	virtual ISceneWorld *GetSceneWorld() = 0;
	// ISource2Client::GetPlayerViewSetup; -1 selects the active split screen slot
	virtual void *GetPlayerViewSetup( CSplitScreenSlot nSlot ) = 0;
	// Named by what it does: a float the view object of the network game client reports for the split screen slot,
	// -1.0 without a connected client
	virtual float QuerySplitScreenPlayerFloat( CSplitScreenSlot nSlot ) = 0;

	// ISource2Client::UpdateClientEntityList
	virtual void UpdateClientEntityList() = 0;

	virtual const char *GetVDataClassName( const char *pName, const char *pScopeFile ) = 0;
	virtual const char *GetVDataClassNameByDataType( const char *pName, const char *pGenericDataType ) = 0;
	virtual const CUtlVector< CUtlString > &GetSubclassNamesInScope( const char *pScopeFile ) = 0;
	virtual const CUtlVector< CUtlString > &GetSubclassNamesInScopeByDataType( const char *pGenericDataType ) = 0;
	virtual void GetAllSubclassNames( CUtlVector< CUtlString > &names ) = 0;
	virtual const char *GetSubclassDesignerName( const char *pSubclassName ) = 0;
	virtual const CUtlVector< CUtlString > &GetDesignerNamesForClass( const char *pClassName ) = 0;
	virtual void GetSubclassAutoCompleteList( const void *pPartialName, CUtlVector< const char * > &completions ) = 0;

	// ISource2Client::GetClientToolInterface
	virtual void *GetClientToolInterface() = 0;

	// "" without the game server
	virtual const char *GetNativeClassForScriptClass( const char *pScriptClassName ) = 0;
	virtual CEntityClass *GetScriptClassForDesignerName( const char *pDesignerName ) = 0;
	virtual bool IsScriptClassDerivedFrom( const char *pDesignerName, const char *pBaseName ) = 0;

	// ISource2Client::EmptyToolCallback
	virtual void EmptyToolCallback() = 0;

	virtual void RequestScreenshot( IScreenshotCallback *pCallback, void *pContext ) = 0;
	virtual void RequestJpegScreenshot( IScreenshotCallback *pCallback, int nQuality, void *pContext ) = 0;
	virtual void GenerateScreenshotFileName( const char *pszFileName, const char *pszExtension, CUtlString &sFileName ) = 0;

	// AMNOTE: Forward to the four empty ISource2Server slots after LogForHTTPListeners (ToolServerCallback1-4); no
	// source names them, and neither side passes or reads an argument
	virtual void ToolServerCallback1() = 0;
	virtual void ToolServerCallback2() = 0;
	virtual void ToolServerCallback3() = 0;
	virtual void ToolServerCallback4() = 0;

	// ISource2Client::AllocateMouth / FreeMouth; FreeMouth does nothing for 0
	virtual uint16 AllocateMouth( int nEntityIndex ) = 0;
	virtual void FreeMouth( uint16 nMouth ) = 0;
};

#endif // ITOOLSERVICE_H
