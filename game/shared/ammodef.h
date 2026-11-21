//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Holds the definitions of the game's ammo types
//
//=============================================================================//

#ifndef AMMODEF_H
#define AMMODEF_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"
#include "tier0/convar.h"
#include "mathlib/range.h"
#include "schemasystem/schematypes.h"
#include "resourcesystem/ivdataresourcelistener.h"

#include "schemacompiler/schemaon.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

// Declared in baseplayerweapon_vdata.h: the index of an ammo type, -1 for none
struct AmmoIndex_t;

schema enum AmmoFlags_t : uint32
{
	AMMO_FORCE_DROP_IF_CARRIED = 0x1,
	AMMO_RESERVE_STAYS_WITH_WEAPON = 0x2,
	AMMO_FLAG_MAX = 0x2,
};

//-----------------------------------------------------------------------------
// One ammo type. CCSAmmoDef::AddAmmoType fills it from its arguments.
//-----------------------------------------------------------------------------
schema struct AmmoTypeInfo_t
{
public:
	TYPEMETA( MGetKV3ClassDefaults; MHasKV3TransferPolymorphicClassname );

	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext );

	// "AmmoTypeInfo_t" here
	virtual const char *GetKV3TransferClassName() const;

public:
	// The name of the ammo type (BULLET_PLAYER_9MM, AMMO_TYPE_FLASHBANG, ...); Index and
	// FindOrAddAmmoType compare it without case.
	noschema CUtlString m_Name;
	int32 m_nMaxCarry;
	// Built from the value of the ammo_<type>_impulse convar, stored at both ends.
	noschema CRangeFloat m_flPhysicsForceImpulse;
	CRangeInt m_nSplashSize;
	AmmoFlags_t m_nFlags;
	float32 m_flMass;
	CRangeFloat m_flSpeed;
};

//-----------------------------------------------------------------------------
// The ammo type of the game: adds the buy rules and the convar of the carry limit.
//-----------------------------------------------------------------------------
schema struct GameAmmoTypeInfo_t : public AmmoTypeInfo_t
{
public:
	TYPEMETA( MGetKV3ClassDefaults; MHasKV3TransferPolymorphicClassname );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	void KV3TransferSave( CKV3TransferSaveContext *pContext ) const override;
	void KV3TransferLoad( CKV3TransferLoadContext *pContext ) override;

	// "GameAmmoTypeInfo_t" here
	const char *GetKV3TransferClassName() const override;

public:
	int32 m_nBuySize;
	int32 m_nCost;
	// The ammo_<type>_max convar; when it is set, CCSAmmoDef::MaxCarry returns its value
	// instead of m_nMaxCarry.
	noschema ConVarRefAbstract m_MaxCarryCVar;
};

//-----------------------------------------------------------------------------
// The list of ammo types. The constructor makes the object the one GetAmmoDef()
// returns; the destructor clears that pointer.
//
// The vdata listener methods are empty in every ammo definition of the game.
//-----------------------------------------------------------------------------
class CAmmoDef : public IVDataResourceListener
{
public:
	void OnVDataLoaded( ResourceHandle_t hResource ) override {}
	void OnVDataReloaded( ResourceHandle_t hResource ) override {}
	void OnVDataUnloaded( ResourceHandle_t hResource ) override {}

	virtual ~CAmmoDef();

	// Called from CCSWeaponSystem::Init; CCSAmmoDef adds its ammo types here.
	// Named by what calls it.
	virtual void Init() {}

	// Called before the entity subclass vdata is first loaded (the lazy accessor of
	// CEntitySubclassGameSystem), so the weapon vdata finds the ammo types by name.
	// Named by what calls it.
	virtual void OnEntitySubclassVDataLoading() {}

	// Called from CCSWeaponSystem::Shutdown; CAmmoDefTyped removes every ammo type.
	// Named by what calls it.
	virtual void Shutdown() {}

	virtual int NumAmmoTypes() const = 0;
	virtual AmmoTypeInfo_t *GetAmmoOfIndex( AmmoIndex_t nAmmoIndex ) = 0;

	// GetAmmoOfIndex( nAmmoIndex )->m_nMaxCarry, 0 for an unknown index
	virtual int MaxCarry( AmmoIndex_t nAmmoIndex );

	// Returns the ammo type of this name, adds an empty one when there is none
	// (CCSAmmoDef::AddAmmoType then fills it). Named by what it does.
	virtual AmmoTypeInfo_t *FindOrAddAmmoType( const char *pszName ) = 0;

	// The index of the ammo type of this name, -1 for none. The game does not read
	// bSuppressWarnings: FindOrAddAmmoType passes true, the GetAmmoDef()->Index helper false.
	// Named by what the callers pass.
	virtual AmmoIndex_t Index( const char *pszName, bool bSuppressWarnings ) = 0;
};

//-----------------------------------------------------------------------------
// Ammo definition that stores its ammo types in a vector
//-----------------------------------------------------------------------------
template < class T >
class CAmmoDefTyped : public CAmmoDef
{
public:
	~CAmmoDefTyped() override;

	void Shutdown() override;
	int NumAmmoTypes() const override;
	AmmoTypeInfo_t *GetAmmoOfIndex( AmmoIndex_t nAmmoIndex ) override;
	AmmoTypeInfo_t *FindOrAddAmmoType( const char *pszName ) override;
	AmmoIndex_t Index( const char *pszName, bool bSuppressWarnings ) override;

public:
	// The game reserves 64 elements in the constructor.
	CUtlVector< T > m_AmmoTypes;
};

// Get the global ammodef object. This is usually implemented in each mod's game rules file somewhere,
// so the mod can setup custom ammo types.
CAmmoDef *GetAmmoDef();

#include "schemacompiler/schemaoff.h"

#endif // AMMODEF_H
