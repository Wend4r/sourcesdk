//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Mesh system interface
//
//===========================================================================//

#ifndef IMESHSYSTEM_H
#define IMESHSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <mathlib/mathlib.h>
#include <mathlib/transform.h>
#include <resourcesystem/resourcehandle.h>
#include <resourcesystem/resourcehandletypes.h>
#include <tier0/utlstringtoken.h>
#include <tier3/tier3.h>

class CSceneObject;
class CRenderMesh;
class CModelBuilder;
class CMeshInstance;
class IMeshRayTrace;
class ISceneObjectDesc;
class ISceneWorld;

abstract_class IMeshSystem : public IAppSystem
{
public:
	virtual void PrepareRenderMesh( CRenderMesh *pMeshData ) = 0;

	virtual CWeakHandle< InfoForResourceTypeCModel > FindOrCreateProceduralModel( const char *pResourceName, bool bIsAnonymous, CModelBuilder *pModelBuilder ) = 0;

	virtual CMeshInstance *CreateMeshInstance( ISceneObjectDesc *pDesc, uint32 nSceneSystemFrameCounter ) = 0;

	virtual void DestroyMeshInstance( CMeshInstance *pMeshInstance ) = 0;

	virtual void Unk_CreateProjectedDecal( void *p ) = 0;

	virtual void Unk_DestroyProjectedDecal( void *p ) = 0;
	virtual CSceneObject *Unk_GetProjectedDecalSceneObject( void *pDecal ) = 0;

	virtual CSceneObject *CreateSceneObject( ResourceHandle_t hModel, const matrix3x4_t &modelToWorld, const char *pszDescName, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;

	virtual CSceneObject *CreateSceneObject( ResourceHandle_t hModel, const CTransform &modelToWorld, const char *pszDescName, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;

	virtual void unk021() = 0;
	virtual void unk022() = 0;
	virtual void RemoveUnusedMeshGroups( CSceneObject *pSceneObject, uint64 nMeshGroupMask ) = 0;
	virtual void EnableTraceLines() = 0;
	virtual void GetModelBounds( ResourceHandle_t hModel, Vector *pMins, Vector *pMaxs ) = 0;
	virtual void unk026() = 0;
	virtual void unk027() = 0;
	virtual void unk028() = 0;
	virtual CMeshInstance *CreateMeshInstance( ISceneObjectDesc *pDesc, uint32 nSceneSystemFrameCounter, void *pRenderMesh, int nSceneObjectIndex, uint64 nOriginalSceneObjectFlags, CSceneObject *pSceneObject, uint64 nMeshGroupMask, uint8 nUnk, const int16 *pUnk ) = 0;

	// NULL unless EnableTraceLines() was called
	virtual IMeshRayTrace *FindOrCreateMeshRayTrace( const CRenderMesh *pMesh ) = 0;
	virtual void GetToolsGeometryInfo( const CRenderMesh *pRenderMesh, const void **ppOutTraceData ) = 0;
	virtual void unk031() = 0;
	virtual void Unk_CreateRenderMesh( void *p ) = 0;
	virtual void InvalidateMeshRayTrace( CRenderMesh *pRenderMesh ) = 0;
	virtual void Unk_LoadModelSyncNoExtrefs( void *p ) = 0;
	virtual void unk035() = 0;
	virtual void unk036() = 0;
	virtual void unk037() = 0;
	virtual void unk038() = 0;

	virtual int Unk_FindInputIndex( ResourceHandle_t hMaterial, int nIndex ) = 0;

	virtual int Unk_FindInputIndex( ResourceHandle_t hMaterial, const char *pszName ) = 0;
	virtual int Unk_FindInputIndex( ResourceHandle_t hMaterial, CUtlStringToken token ) = 0;
	virtual void unk042() = 0;
};

//-----------------------------------------------------------------------------
// Implementation of IMeshSystem. Its members are kept as padding: the render
// mesh manager (polymorphic, g_pRenderMeshManager points at it) at 0x28, the
// model resource type manager (three vtables) from 0x38, two CUtlVectors and a
// lock from 0xA8, and a std::function at 0xF0.
//-----------------------------------------------------------------------------
class CMeshSystem : public CTier3AppSystem< IMeshSystem >
{
public:
	uint8 m_unk028[ 0x108 ];
};

#endif // IMESHSYSTEM_H
