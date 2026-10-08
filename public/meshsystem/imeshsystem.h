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
#include <resourcefile/resourcehandle.h>
#include <tier0/utlstringtoken.h>

class CSceneObject;
class ISceneObjectDesc;
class ISceneWorld;

abstract_class IMeshSystem : public IAppSystem
{
public:
	virtual void Unk_CreateMeshletResources( void *p ) = 0;

	virtual void Unk_CreateModel( void *p ) = 0;

	virtual void *Unk_CreateMeshInstance( ISceneObjectDesc *pDesc, int nUnk ) = 0;

	virtual void Unk_DestroyMeshInstance( void *pMeshInstance ) = 0;

	virtual void Unk_CreateProjectedDecal( void *p ) = 0;

	virtual void Unk_DestroyProjectedDecal( void *p ) = 0;
	virtual CSceneObject *Unk_GetProjectedDecalSceneObject( void *pDecal ) = 0;

	virtual CSceneObject *CreateSceneObject( ResourceHandle_t hModel, const matrix3x4_t &modelToWorld, const char *pszDescName, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;

	virtual CSceneObject *CreateSceneObject( ResourceHandle_t hModel, const CTransform &modelToWorld, const char *pszDescName, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;

	virtual void unk021() = 0;
	virtual void unk022() = 0;
	virtual void unk023() = 0;
	virtual void unk024() = 0;
	virtual void unk025() = 0;
	virtual void unk026() = 0;
	virtual void unk027() = 0;
	virtual void unk028() = 0;
	virtual void *Unk_CreateMeshInstance( ISceneObjectDesc *pDesc, int nUnk1, void *pRenderMesh, int nUnk2, void *pUnk1, void *pUnk2, void *pUnk3, bool bUnk, void *pUnk4 ) = 0;
	virtual void unk029() = 0;
	virtual void unk030() = 0;
	virtual void unk031() = 0;
	virtual void unk032() = 0;
	virtual void unk033() = 0;
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

#endif // IMESHSYSTEM_H
