#ifndef CS_PLAYER_WEAPONGRAPHCONTROLLER_H
#define CS_PLAYER_WEAPONGRAPHCONTROLLER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/globalsymbol.h"
#include "tier0/utlvector.h"
#include "entitytypes.h"
#include "../animgraphcontroller.h"
#include "animlib/animevents.h"

#include "schemacompiler/schemaon.h"

class CCSWeaponBase;

// CCSWeaponBase::m_iWeaponGameplayAnimState
schema enum WeaponGameplayAnimState : uint16
{
	WPN_ANIMSTATE_UNINITIALIZED = 0,
	WPN_ANIMSTATE_DROPPED = 1,
	WPN_ANIMSTATE_HOLSTERED = 10,
	WPN_ANIMSTATE_DEPLOY = 11,
	WPN_ANIMSTATE_IDLE = 50,
	WPN_ANIMSTATE_SHOOT_PRIMARY = 100,
	WPN_ANIMSTATE_SHOOT_SECONDARY = 101,
	WPN_ANIMSTATE_SHOOT_DRYFIRE = 102,
	WPN_ANIMSTATE_CHARGE = 103,
	WPN_ANIMSTATE_GRENADE_PULL_PIN = 200,
	WPN_ANIMSTATE_GRENADE_READY = 201,
	WPN_ANIMSTATE_GRENADE_THROW = 202,
	WPN_ANIMSTATE_C4_PLANT = 300,
	WPN_ANIMSTATE_HEALTHSHOT_INJECT = 400,
	WPN_ANIMSTATE_KNIFE_PRIMARY_HIT = 500,
	WPN_ANIMSTATE_KNIFE_PRIMARY_MISS = 501,
	WPN_ANIMSTATE_KNIFE_SECONDARY_HIT = 502,
	WPN_ANIMSTATE_KNIFE_SECONDARY_MISS = 503,
	WPN_ANIMSTATE_KNIFE_PRIMARY_STAB = 504,
	WPN_ANIMSTATE_KNIFE_SECONDARY_STAB = 505,
	WPN_ANIMSTATE_SILENCER_APPLY = 600,
	WPN_ANIMSTATE_SILENCER_REMOVE = 601,
	WPN_ANIMSTATE_RELOAD = 800,
	WPN_ANIMSTATE_RELOAD_OUTRO = 801,
	WPN_ANIMSTATE_RELOAD_SILENT = 802,
	WPN_ANIMSTATE_RELOAD_OUTRO_SILENT = 803,
	WPN_ANIMSTATE_INSPECT = 1000,
	WPN_ANIMSTATE_INSPECT_OUTRO = 1001,
	WPN_ANIMSTATE_INVENTORY_UI_TUMBLE = 1500,
	WPN_ANIMSTATE_INVENTORY_UI_KEYCHAIN_APPLY = 1501,
	WPN_ANIMSTATE_END_VALID = 2000,
};

//-----------------------------------------------------------------------------
// Drives the graph of a weapon model from the state of the weapon and reads back what the
// animation reports. The weapon controller lives in the controller manager of the pawn that
// holds the weapon; CCS2PawnGraphController derives from it.
//
// ReflectWeaponState (not virtual) runs for the weapon every update: it sets the weapon
// parameters and, when m_iWeaponGameplayAnimState or its timestamp changed, starts the action of
// the new state. PostGraphUpdate turns the sampled events (WPN_* IDs, sound events) into the
// members after the parameters, which CCSWeaponBase::ProcessAnimGraphFlags applies to the weapon.
//
// The game implements the virtual methods; the objects are only reached through pointers.
//-----------------------------------------------------------------------------
schema class CCS2WeaponGraphController : public CAnimGraphControllerBase
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	// For WPN_ANIMSTATE_INSPECT and WPN_ANIMSTATE_INSPECT_OUTRO, called by ReflectWeaponState
	// with the variation value of the new state: starts action_inspect, or restarts it with
	// action_reset when it already runs. CCS2PawnGraphController plays the deploy or the idle
	// action instead. Named by its caller.
	virtual void ReflectInspectState( CCSWeaponBase *pWeapon, float flVariation );

	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_action;
	CAnimGraph2ParamOptionalRef< bool > m_bActionReset;
	CAnimGraph2ParamOptionalRef< float32 > m_flWeaponActionSpeedScale;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_weaponCategory;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_weaponType;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_weaponExtraInfo;
	CAnimGraph2ParamOptionalRef< float32 > m_flWeaponAmmo;
	CAnimGraph2ParamOptionalRef< float32 > m_flWeaponAmmoMax;
	CAnimGraph2ParamOptionalRef< float32 > m_flWeaponAmmoReserve;
	CAnimGraph2ParamOptionalRef< bool > m_bWeaponIsSilenced;
	CAnimGraph2ParamOptionalRef< float32 > m_flWeaponIronsightAmount;
	CAnimGraph2ParamOptionalRef< bool > m_bIsUsingLegacyModel;
	CAnimGraph2ParamOptionalRef< float32 > m_idleVariation;
	CAnimGraph2ParamOptionalRef< float32 > m_deployVariation;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_attackType;
	CAnimGraph2ParamOptionalRef< float32 > m_attackThrowStrength;
	CAnimGraph2ParamOptionalRef< float32 > m_flAttackVariation;
	CAnimGraph2ParamOptionalRef< float32 > m_inspectVariation;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_inspectExtraInfo;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_reloadStage;

	// The state and its timestamp that ReflectWeaponState last started an action for
	noschema WeaponGameplayAnimState m_nLastWeaponGameplayAnimState;

	// m_szAnimClass of the weapon data, or m_szName without it, as the deploy action set it.
	// Named by what it holds.
	noschema CGlobalSymbol m_deployedWeaponAnimClass;
	noschema GameTime_t m_flLastWeaponGameplayAnimStateTimestamp;

	// Set by PostGraphUpdate from the events of the last update, each named after its event:
	// WPN_ACTION_COMPLETE, or WPN_PRIMARY_FIRE_COMPLETE / WPN_SECONDARY_FIRE_COMPLETE for the
	// attack in progress; WPN_RELOAD_ADD_AMMO; WPN_RELOAD_SILENT; WPN_HEALTHSHOT_INJECT;
	// WPN_SILENCER_ATTACH; WPN_SILENCER_DETACH; WPN_BLOCK_INSPECT
	noschema bool m_bActionComplete;
	noschema bool m_bReloadAddAmmo;
	noschema bool m_bReloadSilent;
	noschema bool m_bHealthshotInject;
	noschema bool m_bSilencerAttach;
	noschema bool m_bSilencerDetach;
	noschema bool m_bBlockInspect;

	// 1, 2 or 3 after WPN_INSPECT_INTRO, WPN_INSPECT_LOOP or WPN_INSPECT_OUTRO, 0 without them
	noschema int32 m_nInspectStage;

	// The number in the secondary ID of WPN_BOMB_STAGE, -1 without it
	noschema int32 m_nBombStage;

	// Index of the attack_type symbol of the attack action in progress, 1 to 13 by the gameplay
	// state; 2 and 3 are the primary and the secondary attack. Named by what it holds.
	noschema int32 m_nAttackType;

	// Seconds in action_idle; after 3 seconds with WPN_IDLE_COMPLETE a new idle variation starts.
	// Named by what it holds.
	noschema float32 m_flIdleTime;

	// WPN_DROP_MAG, and the token of its secondary ID
	noschema bool m_bDropMag;
	noschema uint32 m_nDropMagID;

	// The sound events sampled in the last update, which ProcessAnimGraphFlags plays. The server
	// class only: the client class ends before this member (728 bytes).
	noschema CUtlVectorFixedGrowable< CNmSoundEvent, 10 > m_soundEvents;
};

#include "schemacompiler/schemaoff.h"

#endif // CS_PLAYER_WEAPONGRAPHCONTROLLER_H
