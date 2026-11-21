//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The decal game system: decal parameters of the decal materials
//          and the decal resource manifest.
//
//=============================================================================//

#ifndef DECALGAMESYSTEM_H
#define DECALGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <resourcefile/resourcetype.h>
#include <tier0/threadtools.h>
#include <tier0/utlhashtable.h>
#include <tier0/utlrbtree.h>
#include <tier0/utlsignalslot.h>

class CResourceManifestPrerequisite;

//-----------------------------------------------------------------------------
// The decal parameters of one decal material (or of one sequence of a decal sheet), 80 bytes.
// The members are named after the material attributes they are read from (the client reads
// them by token: "decalWorldWidth", "decalDepthOffset", ...). The server only frees these records.
//-----------------------------------------------------------------------------
class CDecalMaterialParams
{
public:
	// "decalWorldWidth" / "decalWorldHeight"; without them the texture size times
	// CDecalGameSystem::m_flDefaultTextureScale (64 x 64 by default)
	float m_flWorldWidth;
	float m_flWorldHeight;
	// "decalSizeVariance" / "decalHeightVariance"
	float m_flSizeVariance;
	float m_flHeightVariance;
	// "decalScale" (1.0), "decalScaleVariation" (0.25)
	float m_flDecalScale;
	float m_flDecalScaleVariation;
	// "decalDepth" (4.0), "decalDepthOffset", "decalDepthVariance"
	float m_flDepth;
	float m_flDepthOffset;
	float m_flDepthVariance;
	// "decalFadeStartTime" / "decalStartFadeTime" and "decalFadeDuration" (defaults from the
	// r_decals_default_start_fade / r_decals_default_fade_duration convars)
	float m_flDecalStartFadeTime;
	float m_flDecalFadeDuration;
	// "animationScale", "animationStartTime" (sheet decals)
	float m_flAnimationScale;
	float m_flAnimationStartTime;
	// "decalAlignWithGravityFactor"
	float m_flAlignWithGravityFactor;
	// Named by what the code does with them: an integer attribute (token 0x8033177F, 0 by default)
	// and a boolean attribute (token 0xED023051, false by default) of the material
	int m_nIntAttribute;
	bool m_bBoolAttribute;
	// "lightingOriginOffset"
	float m_flLightingOriginOffset;
	// Named by what the code does with it: a float attribute (token 0x2CA724E6) clamped to at least 1
	float m_flMinOneAttribute;
	// Named by what the code does with it: the sheet sequence these parameters were made for,
	// -1 for a material without a sheet
	int m_nSheetSequence;
	// "decalRtEncoding", or the value of the "decalSkinnedModelMode" enum string; -1 when neither is set
	uint8 m_nRtEncoding;
};

//-----------------------------------------------------------------------------
// Named by what it holds: the key of CDecalGameSystem::m_MaterialParams (16 bytes), the decal
// material's resource id and the sheet sequence. The game hashes it as Hash( id ) | nSequence.
//-----------------------------------------------------------------------------
struct DecalMaterialParamsKey_t
{
	ResourceId_t m_nMaterialId;
	int m_nSequence;

	bool operator==( const DecalMaterialParamsKey_t &other ) const { return m_nMaterialId.m_Value == other.m_nMaterialId.m_Value && m_nSequence == other.m_nSequence; }
};

struct DecalMaterialParamsKeyHashFunctor
{
	unsigned int operator()( const DecalMaterialParamsKey_t &key ) const { return Mix64HashFunctor()( key.m_nMaterialId.m_Value ) | key.m_nSequence; }
};

struct ResourceIdLessFunctor
{
	bool operator()( const ResourceId_t &lhs, const ResourceId_t &rhs ) const { return lhs.m_Value < rhs.m_Value; }
};

//-----------------------------------------------------------------------------
// Reallocating factory ("DecalGameSystem"), Linux 192 / Windows 144 bytes: the std::shared_mutex is
// a 56-byte pthread_rwlock_t on Linux and an 8-byte SRWLOCK on Windows, so everything after it moves
// by 48. On the server only GameInit / GameShutdown do something; the client (same source,
// decalgamesystem.cpp) fills m_MaterialParams and m_MissingParamsMaterials and adds members after
// m_flDefaultTextureScale. It adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CDecalGameSystem : public CAutoGameSystem, public CUtlSlot
{
public:
	DECLARE_GAME_SYSTEM();

	CDecalGameSystem();
	virtual ~CDecalGameSystem();

	// Creates m_pResourceManifestPrerequisite with the "DecalGameSystemManifest_server" manifest
	// ("CDecalGameSystem::LoopInit") and hands it to the event's prerequisite list
	GS_EVENT( GameInit );
	// Frees the m_MaterialParams records and releases m_pResourceManifestPrerequisite
	GS_EVENT( GameShutdown );
	// Empty on the server
	GS_EVENT( GameActivate );
	GS_EVENT( GameDeactivate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// Named by what it guards: m_MaterialParams and m_MissingParamsMaterials (shared lock to look up,
	// exclusive to add; client)
	CThreadSpinRWLock m_MaterialParamsLock;
	// Named by what it holds: the decal parameters of each decal material / sheet sequence
	// (records of 80 bytes, freed on GameShutdown)
	CUtlHashtable< DecalMaterialParamsKey_t, CDecalMaterialParams *, DecalMaterialParamsKeyHashFunctor > m_MaterialParams;
	CResourceManifestPrerequisite *m_pResourceManifestPrerequisite;
	// Named by what it holds: the materials that have no decal parameters (warned once; client)
	CUtlRBTree< ResourceId_t, ResourceIdLessFunctor > m_MissingParamsMaterials;
	// "Hammer/DefaultTextureScale" of the game config (0.25 by default): the world size of a decal
	// texel when the material sets no decalWorldWidth / decalWorldHeight
	float m_flDefaultTextureScale;
};

#endif // DECALGAMESYSTEM_H
