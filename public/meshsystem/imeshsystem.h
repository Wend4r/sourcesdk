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
#include <tier0/refcount.h>
#include <tier0/utlstringtoken.h>
#include <tier0/utlvector.h>

class CMeshInstance;
class CRenderMesh;
class CSceneObject;
class CUtlString;
class ISceneObjectDesc;
class ISceneWorld;
class InfoForResourceTypeCModel;
struct CModelMeshHandle;
struct MeshProjectedDecalInfo_t;
struct TraceDataForDraw_t;
struct HProjectedDecal__;

typedef HProjectedDecal__ *HProjectedDecal;

//-----------------------------------------------------------------------------
// Mesh system: models, mesh instances, projected decals and mesh ray traces.
//
// Names from the s&box DWARF of IMeshSystem and the Dota 2 (2018) Mach-O symbols
// of CMeshSystem where the CS2 body matches; the rest are named by what they do.
// The overloads of CreateMeshInstance, CreateSceneObject and FindInputIndex are
// declared at their Linux slots: MSVC groups each set at its first declaration
// in reverse order, as the Windows binary has it (CreateMeshInstance 13-14,
// CreateSceneObject 19-20, FindInputIndex 39-41).
//-----------------------------------------------------------------------------
abstract_class IMeshSystem : public IAppSystem
{
public:
	// Creates the meshlet resources of the render mesh
	virtual void PrepareRenderMesh( CRenderMesh *pRenderMesh ) = 0;

	// pResourceName receives the procedural resource name of the model
	virtual CWeakHandle< InfoForResourceTypeCModel > FindOrCreateProceduralModel( const char *pszName, bool bCreate, CUtlString *pResourceName ) = 0;

	virtual CMeshInstance *CreateMeshInstance( ISceneObjectDesc *pDesc, uint32 nFlags ) = 0;
	virtual void DestroyMeshInstance( CMeshInstance *pMeshInstance ) = 0;

	virtual HProjectedDecal CreateProjectedDecal( const MeshProjectedDecalInfo_t &info ) = 0;
	virtual void DestroyProjectedDecal( HProjectedDecal hDecal ) = 0;
	virtual CSceneObject *GetDecalSceneObject( HProjectedDecal hDecal ) = 0;

	virtual CSceneObject *CreateSceneObject( ResourceHandle_t hModel, const matrix3x4_t &modelToWorld, const char *pszDescName, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;
	virtual CSceneObject *CreateSceneObject( ResourceHandle_t hModel, const CTransform &modelToWorld, const char *pszDescName, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;

	// Creates the "BaseSceneObjectDesc" scene objects of every mesh of the model
	virtual bool CreateSceneObjects( ResourceHandle_t hModel, uint64 nFlags, ISceneWorld *pWorld, CUtlVector< CSceneObject * > *pSceneObjects, const int *pMeshGroupMask ) = 0;
	virtual CSceneObject *CreateSceneObjectForSceneObjectData( const CModelMeshHandle &meshHandle, int nDrawCallIndex, uint64 nFlags, ISceneWorld *pWorld, uint32 nLayerMask, ISceneObjectDesc *pDesc ) = 0;
	virtual void RemoveUnusedMeshGroups( CSceneObject *pSceneObject, uint64 nMeshGroupMask ) = 0;
	virtual void EnableTraceLines() = 0;
	virtual void GetModelBounds( ResourceHandle_t hModel, Vector &vecMins, Vector &vecMaxs ) = 0;
	virtual uint64 ComputeSceneObjectTranslucencyFlags( uint16 nTranslucencyType, uint32 nMaterialFlags, uint64 nFlags ) = 0;
	virtual void UpdateTranslucencyFlagsOnMaterialChange( CSceneObject *pSceneObject ) = 0;
	virtual void ChangeModel( CSceneObject *pSceneObject, ResourceHandle_t hModel ) = 0;

	virtual CMeshInstance *CreateMeshInstance( ISceneObjectDesc *pDesc, uint32 nFlags, const CModelMeshHandle &meshHandle, int nDrawCallIndex, uint64 nObjectFlags, CSceneObject *pParent, uint64 nMeshGroupMask, uint8 nLOD, const int16 *pBodyGroups ) = 0;

	// Only while trace lines are enabled
	virtual void *FindOrCreateMeshRayTrace( const CRenderMesh *pRenderMesh ) = 0;
	virtual void GetToolsGeometryInfo( const CRenderMesh *pRenderMesh, const CUtlVector< TraceDataForDraw_t > **ppGeometry ) = 0;
	virtual void InvalidateMeshRayTrace( CRenderMesh *pRenderMesh ) = 0;

	virtual CRefPtr< CRenderMesh > CreateRenderMesh( const char *pszName ) = 0;

	// Named by what it does: the ray trace of the mesh from the ray trace tree, NULL if none
	virtual void *FindMeshRayTrace( const CRenderMesh *pRenderMesh ) = 0;

	// pContext is handed to the load job
	virtual ResourceHandle_t LoadModelSyncNoExtrefs( const char *pszName, bool bCompile, void *pContext ) = 0;

	// Named by what it does: moves a std::function (32 bytes) into the member at +240
	virtual void SetModelLoadCallback( void *pCallback ) = 0;

	virtual uint32 GetMeshTranslucencyType( ResourceHandle_t hRenderMesh ) = 0;

	// Named by what they do: registers a vmdl listener once, and copies the tracked object
	// list into the vector under its lock
	virtual void AddModelListener( void *pListener ) = 0;
	virtual void GetTrackedObjects( CUtlVector< void * > *pObjects ) = 0;

	// Value of the input remap table of the material's texture (see GetInputIndexTexture),
	// -1 when there is none
	virtual int FindInputIndex( ResourceHandle_t hMaterial, int nIndex ) = 0;
	virtual int FindInputIndex( ResourceHandle_t hMaterial, const char *pszName ) = 0;
	virtual int FindInputIndex( ResourceHandle_t hMaterial, CUtlStringToken token ) = 0;

	// Named by what it does: the texture attribute of the material when its bool attribute
	// (both looked up by token) is set
	virtual ResourceHandle_t GetInputIndexTexture( ResourceHandle_t hMaterial ) = 0;

	virtual ~IMeshSystem() {}
};

#endif // IMESHSYSTEM_H
