#ifndef WEAPON_CSBASE_VDATA_H
#define WEAPON_CSBASE_VDATA_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/globalsymbol.h"
#include "tier0/utlmap.h"
#include "tier0/utlstring.h"
#include "tier0/utlsymbollarge.h"
#include "mathlib/vector.h"
#include "../baseplayerweapon_vdata.h"
#include "cstrike15_item_constants.h"
#include "animlib/graph/animgraph_valuetypes.h"

#include "schemacompiler/schemaon.h"

schema enum CSWeaponType : uint32
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL = 1,
	WEAPONTYPE_SUBMACHINEGUN = 2,
	WEAPONTYPE_RIFLE = 3,
	WEAPONTYPE_SHOTGUN = 4,
	WEAPONTYPE_SNIPER_RIFLE = 5,
	WEAPONTYPE_MACHINEGUN = 6,
	WEAPONTYPE_C4 = 7,
	WEAPONTYPE_TASER = 8,
	WEAPONTYPE_GRENADE = 9,
	WEAPONTYPE_EQUIPMENT = 10,
	WEAPONTYPE_STACKABLEITEM = 11,
	WEAPONTYPE_UNKNOWN = 12,
};

schema enum CSWeaponCategory : uint32
{
	WEAPONCATEGORY_OTHER = 0,
	WEAPONCATEGORY_MELEE = 1,
	WEAPONCATEGORY_SECONDARY = 2,
	WEAPONCATEGORY_SMG = 3,
	WEAPONCATEGORY_RIFLE = 4,
	WEAPONCATEGORY_HEAVY = 5,
	WEAPONCATEGORY_COUNT = 6,
};

schema enum CSWeaponSilencerType : uint32
{
	WEAPONSILENCER_NONE = 0,
	WEAPONSILENCER_DETACHABLE = 1,
	WEAPONSILENCER_INTEGRATED = 2,
};

schema enum gear_slot_t : int32
{
	GEAR_SLOT_INVALID = -1,
	GEAR_SLOT_RIFLE = 0,
	GEAR_SLOT_PISTOL = 1,
	GEAR_SLOT_KNIFE = 2,
	GEAR_SLOT_GRENADES = 3,
	GEAR_SLOT_C4 = 4,
	GEAR_SLOT_RESERVED_SLOT6 = 5,
	GEAR_SLOT_RESERVED_SLOT7 = 6,
	GEAR_SLOT_RESERVED_SLOT8 = 7,
	GEAR_SLOT_RESERVED_SLOT9 = 8,
	GEAR_SLOT_RESERVED_SLOT10 = 9,
	GEAR_SLOT_RESERVED_SLOT11 = 10,
	GEAR_SLOT_BOOSTS = 11,
	GEAR_SLOT_UTILITY = 12,
	GEAR_SLOT_COUNT = 13,
	GEAR_SLOT_FIRST = 0,
	GEAR_SLOT_LAST = 12,
};

// A value per firing mode (CSWeaponMode: primary, secondary)
schema class CFiringModeFloat
{
public:
	TYPEMETA( MPropertyCustomEditor = "multi_float(2)" );

	float32 m_flValues[ 2 ];
};

schema class CFiringModeInt
{
public:
	TYPEMETA( MPropertyCustomEditor = "multi_int(2)" );

	int32 m_nValues[ 2 ];
};

//-----------------------------------------------------------------------------
// Data of a Counter-Strike weapon subclass. Adds no virtual method to CBasePlayerWeaponVData;
// GetSlot and GetPosition return m_GearSlot and m_GearSlotPosition.
//-----------------------------------------------------------------------------
schema class CCSWeaponBaseVData : public CBasePlayerWeaponVData
{
public:
	TYPEMETA( MGetKV3ClassDefaults; MPropertySuppressBaseClassField = "m_iSlot"; MPropertySuppressBaseClassField = "m_iPosition" );

	~CCSWeaponBaseVData() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	void KV3TransferSave( CKV3TransferSaveContext *pContext ) const override;

	// Also derives m_WeaponCategory from m_WeaponType and reads the "taxonomy" table into m_Taxonomy
	void KV3TransferLoad( CKV3TransferLoadContext *pContext ) override;
	const char *GetKV3TransferClassName() const override;

	// Adds the skeleton and the tracer particle to what CBasePlayerWeaponVData precaches
	void Precache( const CEntityPrecacheContext *pContext ) override;

	int GetSlot() const override;
	int GetPosition() const override;

	CSWeaponType m_WeaponType;
	CSWeaponCategory m_WeaponCategory;
	CResourceNameTyped< CWeakHandle< InfoForResourceTypeCNmSkeleton > > m_szAnimSkeleton; META( MPropertyStartGroup = "Visuals" );
	Vector m_vecMuzzlePos0;
	Vector m_vecMuzzlePos1;
	CResourceNameTyped< CWeakHandle< InfoForResourceTypeIParticleSystemDefinition > > m_szTracerParticle; META( MPropertyDescription = "Effect to actually fire into the world from this weapon" );
	gear_slot_t m_GearSlot; META( MPropertyStartGroup = "HUD Positions"; MPropertyFriendlyName = "HUD Bucket"; MPropertyDescription = "Which 'column' to display this weapon in the HUD" );
	int32 m_GearSlotPosition;
	loadout_slot_t m_DefaultLoadoutSlot; META( MPropertyFriendlyName = "HUD Bucket Position"; MPropertyDescription = "Default team (non Terrorist or Counter-Terrorist) 'row' to display this weapon in the HUD." );
	int32 m_nPrice; META( MPropertyStartGroup = "In-Game Data" );
	int32 m_nKillAward;
	int32 m_nPrimaryReserveAmmoMax;
	int32 m_nSecondaryReserveAmmoMax;
	bool m_bMeleeWeapon;
	bool m_bHasBurstMode;
	bool m_bIsRevolver;
	bool m_bCannotShootUnderwater;
	CGlobalSymbol m_szName; META( MPropertyFriendlyName = "In-Code weapon name" );
	CSWeaponSilencerType m_eSilencerType;
	bool m_bShowCrosshair;
	bool m_bIsFullAuto;
	int32 m_nNumBullets;
	bool m_bReloadsSingleShells;
	CFiringModeFloat m_flCycleTime; META( MPropertyStartGroup = "Firing Mode Data" );
	float32 m_flCycleTimeWhenInBurstMode;
	float32 m_flTimeBetweenBurstShots;
	CFiringModeFloat m_flMaxSpeed;
	CFiringModeFloat m_flSpread;
	CFiringModeFloat m_flInaccuracyCrouch;
	CFiringModeFloat m_flInaccuracyStand;
	CFiringModeFloat m_flInaccuracyJump;
	CFiringModeFloat m_flInaccuracyLand;
	CFiringModeFloat m_flInaccuracyLadder;
	CFiringModeFloat m_flInaccuracyFire;
	CFiringModeFloat m_flInaccuracyMove;
	CFiringModeFloat m_flRecoilAngle;
	CFiringModeFloat m_flRecoilAngleVariance;
	CFiringModeFloat m_flRecoilMagnitude;
	CFiringModeFloat m_flRecoilMagnitudeVariance;
	CFiringModeInt m_nTracerFrequency;
	float32 m_flInaccuracyJumpInitial;
	float32 m_flInaccuracyJumpApex;
	float32 m_flInaccuracyReload;
	float32 m_flDeployDuration;
	float32 m_flDisallowAttackAfterReloadStartDuration;
	int32 m_nBurstShotCount;
	bool m_bAllowBurstHolster;
	int32 m_nRecoilSeed; META( MPropertyStartGroup = "Firing" );
	int32 m_nSpreadSeed;
	float32 m_flAttackMovespeedFactor;
	float32 m_flInaccuracyPitchShift;
	float32 m_flInaccuracyAltSoundThreshold;
	CUtlString m_szUseRadioSubtitle;
	bool m_bUnzoomsAfterShot; META( MPropertyStartGroup = "Zooming" );
	bool m_bHideViewModelWhenZoomed;
	int32 m_nZoomLevels;
	int32 m_nZoomFOV1;
	int32 m_nZoomFOV2;
	float32 m_flZoomTime0;
	float32 m_flZoomTime1;
	float32 m_flZoomTime2;
	float32 m_flIronSightPullUpSpeed; META( MPropertyStartGroup = "Iron Sights" );
	float32 m_flIronSightPutDownSpeed;
	float32 m_flIronSightFOV;
	float32 m_flIronSightPivotForward;
	float32 m_flIronSightLooseness;
	int32 m_nDamage; META( MPropertyStartGroup = "Damage" );
	float32 m_flHeadshotMultiplier;
	float32 m_flArmorRatio;
	float32 m_flPenetration;
	float32 m_flRange;
	float32 m_flRangeModifier;
	float32 m_flFlinchVelocityModifierLarge;
	float32 m_flFlinchVelocityModifierSmall;
	float32 m_flRecoveryTimeCrouch; META( MPropertyStartGroup = "Recovery" );
	float32 m_flRecoveryTimeStand;
	float32 m_flRecoveryTimeCrouchFinal;
	float32 m_flRecoveryTimeStandFinal;
	int32 m_nRecoveryTransitionStartBullet;
	int32 m_nRecoveryTransitionEndBullet;
	float32 m_flThrowVelocity; META( MPropertyStartGroup = "Grenade Data" );
	Vector m_vSmokeColor;
	CGlobalSymbol m_szAnimClass;

	// The keys of the "taxonomy" table of the subclass plus m_szName, each mapped to true: the
	// weapon names and kinds a weapon counts as. CCSWeaponBase::GetBoneTransformOverride asks it
	// for weapon_elite. Named after the key it is loaded from.
	noschema CUtlOrderedMap< CUtlSymbolLarge, bool > m_Taxonomy;

	// A handle in the opaque handle table of the pulse system (PVAL_OPAQUE_HANDLE values), -1 when
	// there is none; the destructor releases it. Nothing in the server module sets it. Named by
	// what it holds.
	noschema uint32 m_hPulseOpaqueHandle;
};

#include "schemacompiler/schemaoff.h"

#endif // WEAPON_CSBASE_VDATA_H
