//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The server's implementation of the tool game simulation interface.
//
//=============================================================================//

#ifndef TOOLGAMESIMULATIONAPI_H
#define TOOLGAMESIMULATIONAPI_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "engine2/itoolgamesimulationapi.h"
#include "entity2/ientitylistener.h"

#include <tier0/utlhashtable.h>
#include <tier0/utlvector.h>

//-----------------------------------------------------------------------------
// Registered as "CToolGameSimulationAPI" (static factory). 96 bytes on both platforms. No source has
// the class; the names are named by what the code does. libserver.so copies the 44 interface methods
// and the two IEntityListener overrides to the end of the main table (slots 66..111).
//-----------------------------------------------------------------------------
class CToolGameSimulationAPI : public CAutoGameSystem, public IToolGameSimulationAPI, public IEntityListener
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CToolGameSimulationAPI();

	GS_EVENT( GameInit );
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// IToolGameSimulationAPI
	virtual void PushPhysicsObjectAlongRay( const Vector &vecStart, const Vector &vecEnd ) override;
	virtual bool GiveListenServerHostItem( const Vector &vecOrigin, const QAngle &angAngles, const char *pszItemDefinitionName ) override;
	virtual void TraceHull( const Vector &vecStart, const Vector &vecEnd, const Vector &vecMins, const Vector &vecMaxs, ToolTraceRayResult_t *pResult ) override;
	virtual void TraceRay( const Vector &vecStart, const Vector &vecEnd, ToolTraceRayResult_t *pResult ) override;
	virtual bool IsListenServerHostInGodMode() override;
	virtual void SetListenServerHostGodMode( bool bEnable ) override;
	virtual bool IsListenServerHostNoTarget() override;
	virtual void SetListenServerHostNoTarget( bool bEnable ) override;
	virtual CEntityHandle SpawnEntity( const char *pszClassName, const char *pszModelName, Vector vecOrigin, QAngle angAngles ) override;
	virtual bool LoadMap( const char *pszMapName, KeyValues *pSettings ) override;
	virtual bool LoadSavegame( const char *pszSaveName, KeyValues *pSettings ) override;
	virtual void SaveGame( const char *pszSaveName ) override;
	virtual KeyValues *GetSessionKeyValues() override;
	virtual ISceneWorld *GetSceneWorld() override;
	virtual void *GetDebugOverlay() override;
	virtual float GetCurTime() override;
	virtual void Disconnect() override;
	virtual IToolGameSimulationAPI_IPhysicsGrab *CreatePhysicsGrab( CEntityHandle hEntity, const Vector &vecGrabPoint, bool bLocalSpace, void *pPhysicsBody ) override;
	virtual void EmitSoundForListenServerHost( const Vector *pOrigin, const char *pszSoundEvent ) override;
	virtual void *UnimplementedQuery() override;
	virtual const char *GetEntityName( CEntityHandle hEntity ) override;
	virtual const char *GetEntityDesignerName( CEntityHandle hEntity ) override;
	virtual const char *GetEntityClassName( CEntityHandle hEntity ) override;
	virtual const Vector &GetEntityOrigin( CEntityHandle hEntity ) override;
	virtual void SetEntityName( CEntityHandle hEntity, const char *pszName ) override;
	virtual void FireEntityInput( CEntityHandle hEntity, const char *pszInput, const char *pszValue ) override;
	virtual void FindEntitiesByName( const char *pszName, CUtlVector< CEntityHandle > &entities ) override;
	virtual CEntityKeyValues *GetEntityKeyValues( CEntityHandle hEntity ) override;
	virtual void GetListenServerHostEyeTransform( CTransform *pTransform ) override;
	virtual void GetSceneEntityInfo( CEntityHandle hEntity, ToolSceneEntityInfo_t *pInfo ) override;
	virtual void TeleportListenServerHost( const Vector &vecOrigin, const QAngle *pAngles ) override;
	virtual CTransform GetListenServerHostTransform() override;
	virtual void GetEntityInfo( CEntityHandle hEntity, ToolEvent_GetEntityInfoResult_t *pResult ) override;
	virtual void GetNamedEntitiesInFrustum( const CFrustum &frustum, CUtlVector< CEntityHandle > &entities ) override;
	virtual void GetNamedEntities( CUtlVector< CEntityHandle > &entities ) override;
	virtual const char *GetMapName() override;
	virtual CEntityHandle GetEntityParent( CEntityHandle hEntity, bool bRoot ) override;
	virtual CEntityHandle GetEntityOwner( CEntityHandle hEntity, bool bTopmost ) override;
	virtual CEntityHandle FindNamedEntity( const char *pszName ) override;
	virtual void HideEntity( CEntityHandle hEntity ) override;
	virtual void UnhideAllEntities() override;
	virtual bool CanListenServerHostReceiveItems() override;
	virtual bool GiveListenServerHostWearable( CEconItemView *pItem ) override;
	virtual bool GiveListenServerHostNamedItem( const char *pszItemName ) override;

	// IEntityListener: track the entities spawned with a name / forget deleted ones
	virtual void OnEntitySpawned( CEntityInstance *pEntity ) override;
	virtual void OnEntityDeleted( CEntityInstance *pEntity ) override;

public:
	// Every entity spawned with a non-empty name (a set: 8-byte entries, the handle is the key)
	CUtlHashtable< CEntityHandle > m_NamedEntities;
	// The entities HideEntity gave EF_NODRAW
	CUtlVector< CEntityHandle > m_HiddenEntities;
};

#endif // TOOLGAMESIMULATIONAPI_H
