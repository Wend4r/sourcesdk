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
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <resourcesystem/resourcehandle.h>

class CNavData;
class CSceneObject;
class InfoForResourceTypeCModel;
class INavListener;
class IToolGameSimulationAPI;
class CEntityClass;
class ISceneWorld;
class IScreenshotCallback;
class InfoForResourceTypeCTextureBase;
class IToolObject;
class Vector;
class Vector4D;
struct EconItemInfo_t;
struct CharacterInfo_t;
class CDmElement;
class CBufferString;
class CEntityHandle;
struct LightDesc_t;
struct matrix3x4_t;
struct ParticleProjectedTextureInfo_t;

FORWARD_DECLARE_HANDLE( HDeferredLight );
FORWARD_DECLARE_HANDLE( HProjectedTexture );

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

	// Forward to the game client
	virtual void UpdateProjectedTexture( const ParticleProjectedTextureInfo_t &info, HProjectedTexture &hTexture ) = 0;
	virtual void DeleteProjectedTexture( HProjectedTexture hTexture ) = 0;
	virtual void *UpdateDeferredLight( ISceneWorld *pSceneWorld, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, Vector &vecUnk1, float flUnk1, Vector4D &vecUnk2, float flUnk2, float flUnk3, float flUnk4, Vector vecUnk3, float flUnk5, float flUnk6, float flUnk7, float flUnk8, float flUnk9, HDeferredLight &hLight ) = 0;
	virtual void DeleteDeferredLight( HDeferredLight hLight ) = 0;
	virtual CSceneObject *UpdateStandardLight( ISceneWorld *pSceneWorld, const LightDesc_t &desc, HDeferredLight &hLight ) = 0;
	virtual void DeleteStandardLight( HDeferredLight hLight ) = 0;
	virtual CSceneObject *UpdateBarnLight( ISceneWorld *pSceneWorld, const void *pBarnLightDesc, const matrix3x4_t &transform, HDeferredLight &hLight ) = 0;
	virtual void DeleteBarnLight( HDeferredLight hLight ) = 0;
	// Forward to the game client
	virtual void GetEconItemNamesForModel( const char *pszModelName, bool bUnk1, bool bUnk2, CUtlVector< CUtlString > &names ) = 0;
	virtual void GetEconItemNamesForCharacter( const char *pszCharacterName, bool bUnk1, bool bUnk2, CUtlVector< CUtlString > &names ) = 0;
	virtual void GetEconItemsInfoForModel( const char *pszModelName, const char *pszUnk, bool bUnk1, bool bUnk2, bool bUnk3, CUtlVector< EconItemInfo_t > &items ) = 0;
	virtual void GetEconItemsInfoForCharacter( const CharacterInfo_t *pCharacter, const char *pszUnk, bool bUnk1, bool bUnk2, bool bUnk3, CUtlVector< EconItemInfo_t > &items ) = 0;
	// 0 without a game client
	virtual int GetEconSlotIndex( const char *pszEconSlotName ) = 0;
	// 1.0 without a game client
	virtual float GetDefaultScaleForModel( const char *pszModelName ) = 0;
	virtual float GetDefaultScaleForCharacter( const CharacterInfo_t *pCharacter ) = 0;
	virtual void GetCharacterForModel( const char *pszModelName, bool bUnk, CharacterInfo_t *pCharacter ) = 0;
	virtual bool GetCharacterModelName( const CharacterInfo_t *pCharacter, int nModelIndex, CBufferString &modelName ) = 0;
	virtual void GetCharacterList( CUtlVector< CharacterInfo_t > *pCharacters ) = 0;
	virtual void GetSceneDirectoryForModel( const char *pszModelName, CBufferString &sceneDirectory ) = 0;
	virtual void UpdateGlobalLightFromLightInfos( IToolObject *pUnk, const CUtlVector< IToolObject * > &lightInfos, const Vector &vecUnk, int nUnk, float flUnk1, float flUnk2, float flUnk3 ) = 0;
	virtual bool GetGlobalLightParameters( int *pnWeatherType, float *pflTimeOfDayAmount, float *pflSmoothedTimeOfDayAmount ) = 0;
	virtual void *GetEconItemSystem_Client() = 0;

	// Forwards to the game server
	virtual void *GetEconItemSystem_Server() = 0;

	virtual void GetModelCombineInfo( CWeakHandle< InfoForResourceTypeCModel > hCombinedModel, CStrongHandleCopyable< InfoForResourceTypeCModel > *phBaseModel, int *piBaseSkin, const CUtlVector< CStrongHandleCopyable< InfoForResourceTypeCModel > > **ppVecAdditionalModels, const CUtlVector< int > **ppVecSkins ) = 0;

	// Dispatch EventClientSceneSystemThreadStateChange_t to the active loop with m_bThreadsActive true and false
	virtual void NotifySceneSystemThreadsActive() = 0;
	virtual void NotifySceneSystemThreadsInactive() = 0;

	virtual void GetHitGroupEnumInfo( CUtlVector< int > &values, CUtlVector< CUtlString > &names ) = 0;

	// Forward to the game client
	virtual CDmElement *GetEntityModelElement( CEntityHandle hEntity ) = 0;
	// Null without a game client
	virtual CWeakHandle< InfoForResourceTypeCModel > GetModelFromElement( CDmElement *pElement ) = 0;
	// The game client reports a valid local player pawn handle
	virtual bool HasLocalPlayerPawn() = 0;

	virtual ISceneWorld *GetSceneWorld() = 0;
	virtual void *GetPlayerViewSetup( CSplitScreenSlot nSlot ) = 0;
	// Current exposure of the split screen player's tonemap system in the world session, -1.0 without a connected client or
	// world session
	virtual float GetTonemapExposure( CSplitScreenSlot nSlot ) = 0;

	// Forwards to ISource2Client::GetEntityMaterialAttributes
	virtual void GetEntityMaterialAttributes( CUtlVector< void * > *pAttributes ) = 0;

	virtual const char *GetVDataClassName( const char *pName, const char *pScopeFile ) = 0;
	virtual const char *GetVDataClassNameByDataType( const char *pName, const char *pGenericDataType ) = 0;
	virtual const CUtlVector< CUtlString > &GetSubclassNamesInScope( const char *pScopeFile ) = 0;
	virtual const CUtlVector< CUtlString > &GetSubclassNamesInScopeByDataType( const char *pGenericDataType ) = 0;
	virtual void GetAllSubclassNames( CUtlVector< CUtlString > &names ) = 0;
	virtual const char *GetSubclassDesignerName( const char *pSubclassName ) = 0;
	virtual const CUtlVector< CUtlString > &GetDesignerNamesForClass( const char *pClassName ) = 0;
	virtual void GetSubclassAutoCompleteList( const void *pUnk, CUtlVector< const char * > &completions ) = 0;

	// Forwards to the game client, NULL without one
	virtual void *GetCompositeMaterialManager() = 0;

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

	// Forward to the game client; the second one does nothing for 0
	virtual uint16 AllocateMouth( SoundEventGuid_t guid ) = 0;
	virtual void FreeMouth( uint16 nMouth ) = 0;
};

#endif // ITOOLSERVICE_H
