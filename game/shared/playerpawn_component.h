#ifndef PLAYERPAWN_COMPONENT_H
#define PLAYERPAWN_COMPONENT_H

#pragma once

#include "tier0/platform.h"
#include "entitytypes.h"
#include "schemasystem/schematypes.h"

class CBasePlayerController;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct CEntityPrecacheContext;
struct CTakeDamageResult;
struct datamap_t;

//-----------------------------------------------------------------------------
// Virtual tables of CPlayerPawnComponent and the classes derived from it (the
// "services" of a player pawn), as laid out by the server module.
//
// The classes carry a VTable suffix and declare virtual methods only: the game
// classes of the same names are schema classes whose members are looked up at
// run time, so a module declares its own class with the members it needs and
// derives it from the matching VTable class.
//
// Every VTable class below the root is a template over its base class and adds only the
// virtual methods its own level introduces. A module with its own class at each
// level of the hierarchy puts them between the layers:
//
//   class CPlayerPawnComponent : public CPlayerPawnComponentVTable {};
//   class CPlayer_WeaponServices : public CPlayer_WeaponServicesVTable< CPlayerPawnComponent > {};
//   class CCSPlayer_WeaponServices : public CCSPlayer_WeaponServicesVTable< CPlayer_WeaponServices > {};
//
// The default template arguments chain the VTable classes directly, for code that
// only needs to call the virtual methods: CCSPlayer_WeaponServicesVTable<>.
//
// Slot numbers in the comments and in the Unk_ names are Windows indexes. The
// order is the same on Linux, where the destructor takes two slots, so every
// slot after it is one further.
//
// A pawn keeps its components in a list and runs the methods from Reset to
// Event_Killed for each of them from its own method named in the comment.
//
// The game has no symbols for these classes. A name comes from a profiler or
// assert string of the method, from the pawn method that calls it, or from the
// Source 1 CBasePlayer method with the same behaviour. The Unk_ slots are empty
// in every class of the server and of the client and no caller was found, so
// there is nothing to name them after.
//-----------------------------------------------------------------------------
abstract_class CPlayerPawnComponentVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual datamap_t *GetDataDescMap() = 0;

	// Save and restore m_pComponentGraphController, no derived class overrides them
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;

	// "CPlayerPawnComponent" for every class
	virtual const char *GetKV3TransferClassName() const = 0;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() = 0;
	virtual ~CPlayerPawnComponentVTable() = 0;

	// The second call after the component is linked to its pawn, after PreInit
	virtual void Init() = 0;

	// CBasePlayerPawn::SetOrResetInitialState
	virtual void Reset() = 0;

	// CBasePlayerPawn::Activate
	virtual void Activate( ActivateType_t activateType ) = 0;

	// The subclass data of the pawn was set (1) or reloaded (2). No component overrides it.
	virtual void OnSubclassVDataChanged( uint32 nChangeType ) = 0;

	// CBasePlayerPawn::UpdateOnRemove
	virtual void UpdateOnRemove() = 0;

	// CBasePlayerPawn::PreThink
	virtual void PreThink() = 0;

	// CBasePlayerPawn::PostThink. A pawn that is neither alive nor a spectator runs it
	// only for the components that return true from ShouldAlwaysPostThink.
	virtual void PostThink() = 0;
	virtual bool ShouldAlwaysPostThink() = 0;

	// CBasePlayerPawn::OnRestore
	virtual void OnRestore() = 0;

	// CBasePlayerPawn::Precache, no component overrides it
	virtual void Precache( const CEntityPrecacheContext *pContext ) = 0;

	// CModelState::SetupModel gave the pawn a model. No server component overrides it;
	// the second argument points at the model handle.
	virtual void OnNewModel( void *pUnknown, void *pModelHandle ) = 0;

	// Called for every component by the controller setter of the pawn (from
	// CBasePlayerController::SetPawn) after m_hController changed, before the pawn's own
	// OnControllerChanged; no server component overrides it. Named by its caller
	virtual void OnControllerChanged( CBasePlayerController *pOldController, CBasePlayerController *pNewController ) = 0;

	// CBasePlayerPawn::Event_Killed, after the pawn has stored its death time
	virtual void Event_Killed( const CTakeDamageResult &result ) = 0;

	// CBasePlayerPawn::Connect. Stores the result of CreateComponentGraphController
	// in m_pComponentGraphController.
	virtual void InitComponentGraphController( void *pContext ) = 0;

	// Returns null for every server class
	virtual void *CreateComponentGraphController( void *pContext ) = 0;

	// The first call after the component is linked to its pawn, every class chains to its base
	virtual void PreInit() = 0;
};

#endif // PLAYERPAWN_COMPONENT_H
