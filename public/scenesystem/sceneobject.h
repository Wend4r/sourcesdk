//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Scene object
//
//===========================================================================//

#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H
#ifdef _WIN32
#pragma once
#endif

#include <tier0/platform.h>
#include <tier0/utlstringtoken.h>
#include <mathlib/vector.h>
#include <mathlib/mathlib.h>
#include <color.h>
#include <scenesystem/scenesystem_flags.h>

class CSceneObject;
class CMeshInstance;
class ISceneObjectDesc;
class ISceneWorld;
class IPVS;
class CPVSData;
struct CSceneObjectExtraData_t;
struct CSceneObjectLightingCache_t;

//-----------------------------------------------------------------------------
// Entry of a scene object in its world: the bounds and a copy of the
// renderable flags that the culling code reads without touching the object.
//-----------------------------------------------------------------------------
struct CSceneObjectReference_t
{
	VectorAligned m_vecAABBMins;
	VectorAligned m_vecAABBMaxes;
	ESceneObjectFlags m_nRenderableFlags;
	CSceneObject *m_pObject;
};

//-----------------------------------------------------------------------------
// Receives the scene objects that the scene system deletes.
//-----------------------------------------------------------------------------
abstract_class ISceneObjectListener
{
public:
	virtual void OnSceneObjectDeleted( CSceneObject *pSceneObject ) = 0;
};

//-----------------------------------------------------------------------------
// Base class of every object rendered by the scene system. Objects are built
// and destroyed through ISceneSystem, which also owns most of the state below.
//-----------------------------------------------------------------------------
class CSceneObject
{
public:
	virtual ~CSceneObject();

	// Recomputes the visibility of the object in its world after its bounds or world changed.
	virtual void RecomputePVS( IPVS *pPVS );

	// Called for each object scheduled through ISceneSystem::SceneObject_ScheduleDirtyUpdate.
	virtual void UpdateDirty();

	bool IsDeleted() const { return m_bIsDeleted; }

	ISceneObjectDesc *GetDesc() const { return m_pDesc; }
	ISceneWorld *GetWorld() const { return m_pWorld; }

	bool HasMeshInstanceData() const { return ( m_nObjectTypeFlags & SCENEOBJECTTYPEFLAG_HAS_MESH_INSTANCE_DATA ) != 0; }
	CMeshInstance *GetMeshInstanceData() const { return HasMeshInstanceData() ? m_pMeshInstanceData : nullptr; }

	const matrix3x4a_t &GetTransform() const { return m_transform; }

	ESceneObjectTypeFlags GetObjectTypeFlags() const { return m_nObjectTypeFlags; }
	ESceneObjectFlags GetFlags() const { return m_nRenderableFlags; }
	bool HasFlags( ESceneObjectFlags nFlags ) const { return ( m_nRenderableFlags & nFlags ) != 0; }
	ESceneObjectFlags GetOriginalFlags() const { return m_nOriginalRenderableFlags; }

	float GetAlphaFade() const { return m_fAlpha; }
	float GetFadeStartDistanceSquared() const { return m_flStartFadeDistanceSquared; }
	float GetFadeCullDistanceSquared() const { return m_flFarCullDistanceSquared; }

	uint8 GetObjectClass() const { return m_nObjectClass; }
	CUtlStringToken GetLayerMatchID() const { return m_nLayerMatchID; }

	CSceneObjectExtraData_t *GetExtraData() const { return m_pExtraData; }
	CSceneObjectLightingCache_t *GetLightingCache() const { return m_pLightingCache; }

public:
	// Only valid with SCENEOBJECTTYPEFLAG_HAS_MESH_INSTANCE_DATA.
	CMeshInstance *m_pMeshInstanceData;
	ISceneObjectDesc *m_pDesc;

	// Set while the object is in a world.
	CSceneObjectReference_t *m_pRefData;

	matrix3x4a_t m_transform;

	// Set to -1 by the constructor.
	int m_nUnk050;
	int m_nUnk054;
	int m_nUnk058;

	float m_flStartFadeDistanceSquared;
	float m_flFarCullDistanceSquared;
	float m_fAlpha;

	ESceneObjectTypeFlags m_nObjectTypeFlags;
	ESceneObjectFlags m_nRenderableFlags;
	ESceneObjectFlags m_nOriginalRenderableFlags;

	uint32 m_nUnk080;

	color24 m_tint;
	uint8 m_nMeshGroupMaskSmall;

	// Compared with ISceneSystem::GetFinishRenderingViewsCounter.
	uint16 m_nGameRenderCounter;

	uint8 m_nDebugLevel : 2;
	uint8 m_nSizeCullBloat : 2;
	uint8 m_nBoundsType : 1;
	uint8 m_bIsDeleted : 1;
	uint8 m_nUnk08A : 2;

	// Index into the object classes of ISceneSystem::FindOrCreateSceneObjectClass.
	uint8 m_nObjectClass;

	CPVSData *m_pPVSData;
	CSceneObjectExtraData_t *m_pExtraData;
	CSceneObjectLightingCache_t *m_pLightingCache;
	ISceneWorld *m_pWorld;

	// Set to -1 by ISceneSystem::ConstructSceneObject.
	uint32 m_nUnk0B0;

	CUtlStringToken m_nLayerMatchID;

	// Points into the game entity that owns the object, or null.
	void *m_pUnk0B8;
};

#endif // SCENEOBJECT_H
