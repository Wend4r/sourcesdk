#ifndef BASEPLAYERWEAPON_VDATA_H
#define BASEPLAYERWEAPON_VDATA_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/bufferstring.h"
#include "tier0/utlmap.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "shareddefs.h"
#include "entitysubclassvdata.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcesystem/resourcehandletypes.h"
#include "resourcesystem/resourcename.h"
#include "particles/iparticlesystemmgr.h"

#include "schemacompiler/schemaon.h"

// Index into the ammo types of scripts/ammo.vdata, -1 for none
schema struct AmmoIndex_t
{
	TYPEMETA( MIsBoxedIntegerType );

	int8 m_Value;
};

schema enum WeaponSound_t : uint32
{
	WEAPON_SOUND_EMPTY = 0,
	WEAPON_SOUND_SECONDARY_EMPTY = 1,
	WEAPON_SOUND_SINGLE = 2,
	WEAPON_SOUND_SECONDARY_ATTACK = 3,
	WEAPON_SOUND_MELEE_MISS = 4,
	WEAPON_SOUND_MELEE_HIT = 5,
	WEAPON_SOUND_MELEE_HIT_WORLD = 6,
	WEAPON_SOUND_MELEE_HIT_PLAYER = 7,
	WEAPON_SOUND_MELEE_HIT_NPC = 8,
	WEAPON_SOUND_SPECIAL1 = 9,
	WEAPON_SOUND_SPECIAL2 = 10,
	WEAPON_SOUND_SPECIAL3 = 11,
	WEAPON_SOUND_NEARLYEMPTY = 12,
	WEAPON_SOUND_IMPACT = 13,
	WEAPON_SOUND_REFLECT = 14,
	WEAPON_SOUND_SECONDARY_IMPACT = 15,
	WEAPON_SOUND_SECONDARY_REFLECT = 16,
	WEAPON_SOUND_RELOAD = 17,
	WEAPON_SOUND_SINGLE_ACCURATE = 18,
	WEAPON_SOUND_ZOOM_IN = 19,
	WEAPON_SOUND_ZOOM_OUT = 20,
	WEAPON_SOUND_MOUSE_PRESSED = 21,
	WEAPON_SOUND_DROP = 22,
	WEAPON_SOUND_RADIO_USE = 23,
	WEAPON_SOUND_NUM_TYPES = 24,
};

schema enum RumbleEffect_t : int32
{
	RUMBLE_INVALID = -1,
	RUMBLE_STOP_ALL = 0,
	RUMBLE_PISTOL = 1,
	RUMBLE_357 = 2,
	RUMBLE_SMG1 = 3,
	RUMBLE_AR2 = 4,
	RUMBLE_SHOTGUN_SINGLE = 5,
	RUMBLE_SHOTGUN_DOUBLE = 6,
	RUMBLE_AR2_ALT_FIRE = 7,
	RUMBLE_RPG_MISSILE = 8,
	RUMBLE_CROWBAR_SWING = 9,
	RUMBLE_AIRBOAT_GUN = 10,
	RUMBLE_JEEP_ENGINE_LOOP = 11,
	RUMBLE_FLAT_LEFT = 12,
	RUMBLE_FLAT_RIGHT = 13,
	RUMBLE_FLAT_BOTH = 14,
	RUMBLE_DMG_LOW = 15,
	RUMBLE_DMG_MED = 16,
	RUMBLE_DMG_HIGH = 17,
	RUMBLE_FALL_LONG = 18,
	RUMBLE_FALL_SHORT = 19,
	RUMBLE_PHYSCANNON_OPEN = 20,
	RUMBLE_PHYSCANNON_PUNT = 21,
	RUMBLE_PHYSCANNON_LOW = 22,
	RUMBLE_PHYSCANNON_MEDIUM = 23,
	RUMBLE_PHYSCANNON_HIGH = 24,
	NUM_RUMBLE_EFFECTS = 25,
};

// Name of a sound event, owning its string. The destructor of a weapon's data purges it as a
// buffer string, which is all the game shows of the type. Named by what it holds.
class CSoundEventName
{
public:
	const char *Get() const { return m_Name.Get(); }

	CBufferString m_Name;
};

// Name of a model attachment with its token. The game loads it with the KV3 transfer of
// CUtlStringAndTokenWithStorage, the class it derives from.
class CAttachmentNameSymbolWithStorage : public CUtlStringAndTokenWithStorage
{
};

//-----------------------------------------------------------------------------
// Data of a weapon subclass. Adds two virtual methods to CEntitySubclassVDataBase.
//-----------------------------------------------------------------------------
schema class CBasePlayerWeaponVData : public CEntitySubclassVDataBase
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	~CBasePlayerWeaponVData() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	void KV3TransferSave( CKV3TransferSaveContext *pContext ) const override;
	void KV3TransferLoad( CKV3TransferLoadContext *pContext ) override;
	const char *GetKV3TransferClassName() const override;

	// Precaches the three models and the two particles
	void Precache( const CEntityPrecacheContext *pContext ) override;

	// m_iSlot here, m_GearSlot in CCSWeaponBaseVData; CBasePlayerWeapon::GetSlot asks it.
	// Named after the member it returns.
	virtual int GetSlot() const;

	// m_iPosition here, m_GearSlotPosition in CCSWeaponBaseVData; CBasePlayerWeapon::GetPosition
	// asks it. Named after the member it returns.
	virtual int GetPosition() const;

	CResourceNameTyped< CWeakHandle< InfoForResourceTypeCModel > > m_szWorldModel; META( MPropertyStartGroup = "Visuals"; MPropertyDescription = "Model used on the ground or held by an entity"; MPropertyProvidesEditContextString = "ToolEditContext_ID_VMDL" );
	CResourceNameTyped< CWeakHandle< InfoForResourceTypeCModel > > m_szWorldModelAg2Override; META( MPropertyDescription = "Model used on the ground or held by an entity"; MPropertyProvidesEditContextString = "ToolEditContext_ID_VMDL" );
	CResourceNameTyped< CWeakHandle< InfoForResourceTypeCModel > > m_sToolsOnlyOwnerModelName; META( MPropertyDescription = "Model used by the tools only to populate comboboxes for things like animgraph parameter pickers" );
	bool m_bBuiltRightHanded; META( MPropertyDescription = "Was the weapon was built right-handed?" );
	bool m_bAllowFlipping; META( MPropertyDescription = "Allows flipping the model, regardless of whether it is built left or right handed" );
	CAttachmentNameSymbolWithStorage m_sMuzzleAttachment; META( MPropertyDescription = "Attachment to fire bullets from" );
	CResourceNameTyped< CWeakHandle< InfoForResourceTypeIParticleSystemDefinition > > m_szMuzzleFlashParticle; META( MPropertyDescription = "Effect when firing this weapon" );
	CUtlString m_szMuzzleFlashParticleConfig; META( MPropertyDescription = "Effect Config for Muzzle Flash - if set, will use this config specified in the particle effect, using whatever CP configuration is specified there, vdata muzzleflash attachment will be ignored"; MPropertyAttributeEditor = "ParticleConfigName()"; MPropertyEditContextOverrideKey );
	CResourceNameTyped< CWeakHandle< InfoForResourceTypeIParticleSystemDefinition > > m_szBarrelSmokeParticle; META( MPropertyDescription = "Barrel smoke after firing this weapon" );
	uint8 m_nMuzzleSmokeShotThreshold; META( MPropertyDescription = "Barrel smoke shot threshold to create smoke" );
	float32 m_flMuzzleSmokeTimeout; META( MPropertyDescription = "Barrel smoke shot timeout" );
	float32 m_flMuzzleSmokeDecrementRate; META( MPropertyDescription = "Barrel smoke decrement rate when not firing" );
	bool m_bGenerateMuzzleLight;
	bool m_bShouldAnimateInWorld; META( MPropertyDescription = "Was the weapon was built right-handed?" );
	bool m_bLinkedCooldowns; META( MPropertyStartGroup = "Behavior"; MPropertyDescription = "Should both primary and secondary attacks be cooled down together (so cooling down primary attack would cooldown both primary + secondary attacks)?" );
	ItemFlagTypes_t m_iFlags;
	int32 m_iWeight; META( MPropertyDescription = "This value used to determine this weapon's importance in autoselection" );
	bool m_bAutoSwitchTo; META( MPropertyFriendlyName = "Safe To Auto-Switch To"; MPropertyDescription = "Whether this weapon is safe to automatically switch to (should be false for eg. explosives that can the player may accidentally hurt themselves with)" );
	bool m_bAutoSwitchFrom; META( MPropertyFriendlyName = "Safe To Auto-Switch Away From" );
	AmmoIndex_t m_nPrimaryAmmoType; META( MPropertyStartGroup = "Ammo"; MPropertyAttributeEditor = "VDataChoice( scripts/ammo.vdata )"; MPropertyCustomFGDType = "string" );
	AmmoIndex_t m_nSecondaryAmmoType; META( MPropertyAttributeEditor = "VDataChoice( scripts/ammo.vdata )"; MPropertyCustomFGDType = "string" );
	int32 m_iMaxClip1; META( MPropertyFriendlyName = "Primary Clip Size"; MPropertyDescription = "How many bullets this gun can fire before it reloads (0 if no clip)"; MPropertyAttributeRange = "0 255" );
	int32 m_iMaxClip2; META( MPropertyFriendlyName = "Secondary Clip Size"; MPropertyDescription = "How many secondary bullets this gun can fire before it reloads (0 if no clip)"; MPropertyAttributeRange = "0 255" );
	int32 m_iDefaultClip1; META( MPropertyDescription = "Primary Initial Clip (-1 means use clip size)"; MPropertyAttributeRange = "-1 255" );
	int32 m_iDefaultClip2; META( MPropertyDescription = "Secondary Initial Clip (-1 means use clip size)"; MPropertyAttributeRange = "-1 255" );
	bool m_bReserveAmmoAsClips; META( MPropertyDescription = "Indicates whether to treat reserve ammo as clips (reloads) instead of raw bullets" );
	bool m_bTreatAsSingleClip; META( MPropertyDescription = "Regardless of ammo position, we'll always use clip1 as where our bullets come from" );
	bool m_bKeepLoadedAmmo; META( MPropertyDescription = "Indicates whether to keep any loaded ammo in the weapon on reload" );
	RumbleEffect_t m_iRumbleEffect; META( MPropertyStartGroup = "UI" );
	float32 m_flDropSpeed;
	int32 m_iSlot; META( MPropertyFriendlyName = "HUD Bucket"; MPropertyDescription = "Which 'column' to display this weapon in the HUD" );
	int32 m_iPosition; META( MPropertyFriendlyName = "HUD Bucket Position"; MPropertyDescription = "Which 'row' to display this weapon in the HUD" );
	CUtlOrderedMap< WeaponSound_t, CSoundEventName > m_aShootSounds; META( MPropertyStartGroup = "Sounds" );
};

#include "schemacompiler/schemaoff.h"

#endif // BASEPLAYERWEAPON_VDATA_H
