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
#include <tier0/smartptr.h>
#include <tier0/utlstringtoken.h>
#include <tier0/utlvector.h>

class CSceneObject;
class CRenderMesh;
class CModel;
class CModelBuilder;
class CMeshInstance;
class IMeshRayTrace;
class ISceneObjectDesc;
class ISceneWorld;
class InfoForResourceTypeCTextureBase;
class IMaterial2;

//-----------------------------------------------------------------------------
// A mesh of a model, by the index of the mesh in the model
//-----------------------------------------------------------------------------
template < class T >
class CModelMeshHandleBase
{
public:
	T m_hModel;
	int m_iMesh;
};

class CModelMeshHandle : public CModelMeshHandleBase< CWeakHandle< InfoForResourceTypeCModel > >
{
};

abstract_class IMeshSystem : public IAppSystem
{
public:
	virtual void PrepareRenderMesh( CRenderMesh *pMeshData ) = 0;

	virtual CWeakHandle< InfoForResourceTypeCModel > FindOrCreateProceduralModel( const char *pResourceName, bool bIsAnonymous, CModelBuilder *pModelBuilder ) = 0;

	virtual CMeshInstance *CreateMeshInstance( ISceneObjectDesc *pDesc, uint32 nSceneSystemFrameCounter ) = 0;

	virtual void DestroyMeshInstance( CMeshInstance *pMeshInstance ) = 0;

	virtual void *CreateProjectedDecal( const void *pDesc ) = 0;

	virtual void DestroyProjectedDecal( void *pDecal ) = 0;
	virtual CSceneObject *GetProjectedDecalSceneObject( void *pDecal ) = 0;

	virtual CSceneObject *CreateSceneObject( ResourceHandle_t hModel, const matrix3x4_t &modelToWorld, const char *pszDescName, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;

	virtual CSceneObject *CreateSceneObject( ResourceHandle_t hModel, const CTransform &modelToWorld, const char *pszDescName, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;


	// Creates the scene objects of every mesh of the model, unanimated ones through "BaseSceneObjectDesc"
	virtual bool CreateSceneObjects( ResourceHandle_t hModel, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld, CUtlVector< CSceneObject * > *pObjectsOut ) = 0;
	virtual CSceneObject *CreateSceneObjectForSceneObjectData( CModelMeshHandle hMesh, int nSceneObjectIndex, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld, ISceneObjectDesc *pDesc ) = 0;

	virtual void RemoveUnusedMeshGroups( CSceneObject *pSceneObject, uint64 nMeshGroupMask ) = 0;
	virtual void EnableTraceLines() = 0;
	virtual void GetModelBounds( ResourceHandle_t hModel, Vector *pMins, Vector *pMaxs ) = 0;

	// Adds the translucency bits of nTranslucencyType to nTypeFlags
	virtual uint64 ComputeSceneObjectTranslucencyFlags( uint32 nTranslucencyType, uint64 nFlags, uint64 nTypeFlags ) = 0;
	virtual void UpdateTranslucencyFlagsOnMaterialChange( CSceneObject *pSceneObject ) = 0;

	// UpdateTranslucencyFlagsOnMaterialChange() with hMaterial in place of the material of every draw
	virtual void unk028( CSceneObject *pSceneObject, ResourceHandle_t hMaterial ) = 0;
	virtual CMeshInstance *CreateMeshInstance( ISceneObjectDesc *pDesc, uint32 nSceneSystemFrameCounter, CModelMeshHandle hRenderMesh, int nSceneObjectIndex, uint64 nOriginalSceneObjectFlags, CSceneObject *pSceneObject, uint64 nMeshGroupMask, uint8 nLODGroupMask, const int16 *pLocalToMasterTable ) = 0;

	// NULL unless EnableTraceLines() was called
	virtual IMeshRayTrace *FindOrCreateMeshRayTrace( const CRenderMesh *pMesh ) = 0;
	virtual void GetToolsGeometryInfo( const CRenderMesh *pRenderMesh, const void **ppOutTraceData ) = 0;
	// Subtracts the vertex, index and "VertexPaintBlendParams" sizes of the buffer description p from the mesh
	// memory counters when EnableTraceLines() was called; the animation system calls it for every 112-byte
	// description of an array before freeing it
	virtual void unk031( const void *p ) = 0;
	virtual CSmartPtr< CRenderMesh > CreateRenderMesh( const char *pName ) = 0;
	virtual void InvalidateMeshRayTrace( CRenderMesh *pRenderMesh ) = 0;
	// Loads the model resource pModelName without its external references, compiling it through p first
	// when bCompile is set; NULL when the name is not a model or loading fails
	virtual CModel *LoadModelSyncNoExtrefs( const char *pModelName, bool bCompile, void *p ) = 0;

	// Moves the std::function at pFunction into CMeshSystem::m_LegacyModelLoadCallback
	virtual void SetLegacyModelLoadCallback( void *pFunction ) = 0;

	// Draw flags of a loaded material, computed from its attributes ("bShaderBlended", "NeedsDynamicShadows",
	// "AlphaTestZPrepass", "ForceZPrepass", "BindlessResources", "DisableLODBlending", ...) and then passed through
	// every callback added by unk037(); 0 when hMaterial is not loaded
	virtual uint32 unk036( ResourceHandle_t hMaterial ) = 0;

	// Adds pfnCallback to the callbacks of unk036() when it is not there yet and refreshes every loaded model
	virtual void unk037( uint32 ( *pfnCallback )( IMaterial2 *pMaterial, uint32 nFlags ) ) = 0;

	// Appends CMeshSystem::m_unk0D0 to pOut under its lock
	virtual void unk038( CUtlVector< CMeshInstance * > *pOut ) = 0;


	virtual int FindInputIndex( ResourceHandle_t hMaterial, int nIndex ) = 0;

	virtual int FindInputIndex( ResourceHandle_t hMaterial, const char *pszName ) = 0;
	virtual int FindInputIndex( ResourceHandle_t hMaterial, CUtlStringToken token ) = 0;

	// Texture of the material when it has the attribute that enables it, read by FindInputIndex()
	virtual CWeakHandle< InfoForResourceTypeCTextureBase > unk042( ResourceHandle_t hMaterial ) = 0;

	virtual ~IMeshSystem() {}
};

#endif // IMESHSYSTEM_H
