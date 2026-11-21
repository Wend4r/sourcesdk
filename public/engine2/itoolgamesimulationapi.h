//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game simulation interface the tools reach through IToolService::GetToolGameSimulationAPI.
//
//=============================================================================//

#ifndef ITOOLGAMESIMULATIONAPI_H
#define ITOOLGAMESIMULATIONAPI_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "entity2/entityidentity.h"
#include "tier0/utlvector.h"

class Vector;
class QAngle;
class CTransform;
class CFrustum;
class KeyValues;
class ISceneWorld;
class CEntityKeyValues;
class CEconItemView;
// The 40-byte trace result the tool events also use:
// the hit entity's handle, the end position and plane normal, the fraction
struct ToolTraceRayResult_t;
// The result of GetEntityInfo
struct ToolEvent_GetEntityInfoResult_t;
// Named by what it holds: the scene file, the actor name, the scene's duration and two state flags of a
// CSceneEntity (GetSceneEntityInfo)
struct ToolSceneEntityInfo_t;
// RTTI name of the 48-byte object CreatePhysicsGrab allocates; the tools delete it through its vtable
class IToolGameSimulationAPI_IPhysicsGrab;

//-----------------------------------------------------------------------------
// Implemented by the server's CToolGameSimulationAPI. No virtual destructor. No source names the
// methods; every method is named by what the server's implementation does.
// "Listen server host" is the player of slot 0 (UTIL_GetListenServerHost); those methods warn and do
// nothing on a dedicated server.
//-----------------------------------------------------------------------------
abstract_class IToolGameSimulationAPI
{
public:
	// Traces from vecStart to vecEnd and pushes the physics object hit along the ray
	virtual void PushPhysicsObjectAlongRay( const Vector &vecStart, const Vector &vecEnd ) = 0;
	// Creates the item of the named item definition for the listen server host (the origin is not read)
	virtual bool GiveListenServerHostItem( const Vector &vecOrigin, const QAngle &angAngles, const char *pszItemDefinitionName ) = 0;
	virtual void TraceHull( const Vector &vecStart, const Vector &vecEnd, const Vector &vecMins, const Vector &vecMaxs, ToolTraceRayResult_t *pResult ) = 0;
	virtual void TraceRay( const Vector &vecStart, const Vector &vecEnd, ToolTraceRayResult_t *pResult ) = 0;
	// FL_GODMODE / FL_NOTARGET of the listen server host
	virtual bool IsListenServerHostInGodMode() = 0;
	virtual void SetListenServerHostGodMode( bool bEnable ) = 0;
	virtual bool IsListenServerHostNoTarget() = 0;
	virtual void SetListenServerHostNoTarget( bool bEnable ) = 0;
	// Spawns an entity from key values ("classname", "model", "origin", "angles"; "targetname"
	// modeldoc_preview_ent)
	virtual CEntityHandle SpawnEntity( const char *pszClassName, const char *pszModelName, Vector vecOrigin, QAngle angAngles ) = 0;
	// Loads a map (maps/<name>) with the session settings (is_tool_controlled_session set)
	virtual bool LoadMap( const char *pszMapName, KeyValues *pSettings ) = 0;
	// "CToolGameSimulationAPI::LoadSavegame:  no such save file '%s'"
	virtual bool LoadSavegame( const char *pszSaveName, KeyValues *pSettings ) = 0;
	// Executes "save <name>"
	virtual void SaveGame( const char *pszSaveName ) = 0;
	// IGameConfiguration::GetSessionKeyValues
	virtual KeyValues *GetSessionKeyValues() = 0;
	// The render service's scene world once the world is fully loaded for player 0, else null
	virtual ISceneWorld *GetSceneWorld() = 0;
	// The debug overlay object of the debug overlay game system
	virtual void *GetDebugOverlay() = 0;
	// gpGlobals->curtime
	virtual float GetCurTime() = 0;
	// Executes "disconnect"
	virtual void Disconnect() = 0;
	// Creates a physics grab of the entity's physics body (pPhysicsBody, or the entity's own when null)
	virtual IToolGameSimulationAPI_IPhysicsGrab *CreatePhysicsGrab( CEntityHandle hEntity, const Vector &vecGrabPoint, bool bLocalSpace, void *pPhysicsBody ) = 0;
	// Emits a sound to the listen server host only
	virtual void EmitSoundForListenServerHost( const Vector *pOrigin, const char *pszSoundEvent ) = 0;
	// Returns null in the server
	virtual void *UnimplementedQuery() = 0;
	// The identity's name / designer name, the class name ("<no class info>"), "" for an invalid handle
	virtual const char *GetEntityName( CEntityHandle hEntity ) = 0;
	virtual const char *GetEntityDesignerName( CEntityHandle hEntity ) = 0;
	virtual const char *GetEntityClassName( CEntityHandle hEntity ) = 0;
	// vec3_origin for an invalid handle
	virtual const Vector &GetEntityOrigin( CEntityHandle hEntity ) = 0;
	virtual void SetEntityName( CEntityHandle hEntity, const char *pszName ) = 0;
	// Fires the input with a string value
	virtual void FireEntityInput( CEntityHandle hEntity, const char *pszInput, const char *pszValue ) = 0;
	virtual void FindEntitiesByName( const char *pszName, CUtlVector< CEntityHandle > &entities ) = 0;
	virtual CEntityKeyValues *GetEntityKeyValues( CEntityHandle hEntity ) = 0;
	// The eye position and angles of the listen server host
	virtual void GetListenServerHostEyeTransform( CTransform *pTransform ) = 0;
	virtual void GetSceneEntityInfo( CEntityHandle hEntity, ToolSceneEntityInfo_t *pInfo ) = 0;
	virtual void TeleportListenServerHost( const Vector &vecOrigin, const QAngle *pAngles ) = 0;
	// The origin and angles of the listen server host (identity when there is none)
	virtual CTransform GetListenServerHostTransform() = 0;
	virtual void GetEntityInfo( CEntityHandle hEntity, ToolEvent_GetEntityInfoResult_t *pResult ) = 0;
	// The named entities (CToolGameSimulationAPI tracks every entity spawned with a name) whose bounds
	// touch the frustum / all of them
	virtual void GetNamedEntitiesInFrustum( const CFrustum &frustum, CUtlVector< CEntityHandle > &entities ) = 0;
	virtual void GetNamedEntities( CUtlVector< CEntityHandle > &entities ) = 0;
	// The map name of the network game server, "" without one
	virtual const char *GetMapName() = 0;
	// The move parent (the root one when bRoot is set) / the owner entity (the topmost one when bTopmost)
	virtual CEntityHandle GetEntityParent( CEntityHandle hEntity, bool bRoot ) = 0;
	virtual CEntityHandle GetEntityOwner( CEntityHandle hEntity, bool bTopmost ) = 0;
	// The first tracked named entity with this name
	virtual CEntityHandle FindNamedEntity( const char *pszName ) = 0;
	// Adds EF_NODRAW and remembers the entity / takes EF_NODRAW off every remembered entity
	virtual void HideEntity( CEntityHandle hEntity ) = 0;
	virtual void UnhideAllEntities() = 0;
	// The listen server host is a controller with a pawn whose state is 0 (active)
	virtual bool CanListenServerHostReceiveItems() = 0;
	// Creates the wearable of the item view on the listen server host's pawn ("wearable_item", the
	// texture prefab and wear attributes)
	virtual bool GiveListenServerHostWearable( CEconItemView *pItem ) = 0;
	// Gives the named item to the listen server host's pawn (item services)
	virtual bool GiveListenServerHostNamedItem( const char *pszItemName ) = 0;
};

#endif // ITOOLGAMESIMULATIONAPI_H
