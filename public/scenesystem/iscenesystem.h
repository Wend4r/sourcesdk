//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Scene system interface
//
//===========================================================================//

#ifndef ISCENESYSTEM_H
#define ISCENESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <materialsystem2/imaterial2.h>
#include <mathlib/vector.h>
#include <resourcefile/resourcehandle.h>

class CSceneObject;
class CSceneLightObject;
class ISceneObjectDesc;
class ISceneWorld;
class ISceneView;
class IRenderDevice;
class IRayTraceSceneWorld;
struct SceneSystemPerFrameStats_t;

FORWARD_DECLARE_HANDLE( SwapChainHandle_t );

class CTextureBase;

class InfoForResourceTypeCTextureBase
{
public:
	using RuntimeClass_t = CTextureBase;
};

using HRenderTexture = CWeakHandle< InfoForResourceTypeCTextureBase >;

abstract_class ISceneSystem : public IAppSystem
{
public:
	virtual CSceneObject *CreateSceneObject( ISceneObjectDesc *pDesc, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;

	virtual CSceneObject *Unk_CreateMeshSceneObject( ISceneWorld *pWorld, void *pUnk1, void *pUnk2, ResourceHandle_t hMaterial, int nUnk1, int nUnk2, const Vector &vecMins, const Vector &vecMaxs, int nUnk3, const char *pszDescName, bool bUnk, uint64 nFlags ) = 0;

	virtual CSceneObject *Unk_CreateMeshSceneObject( uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld, void *pUnk1, void *pUnk2, ResourceHandle_t hMaterial, int nUnk1, int nUnk2, const Vector &vecMins, const Vector &vecMaxs, int nUnk3, int16 nUnk4, const char *pszDescName, bool bUnk ) = 0;

	// Creates a CMeshBuilderSceneObjectDesc object.
	virtual void Unk_CreateMeshBuilderSceneObject( void *p ) = 0;

	virtual void Unk_CreateAnimatableSceneObject( void *p ) = 0;

	virtual void DeleteSceneObject( CSceneObject *pObject ) = 0;

	virtual ISceneObjectDesc *FindRenderableType( const char *pszName ) = 0;

	virtual void AddRenderableType( const char *pszName, ISceneObjectDesc *pDesc ) = 0;
	virtual void RemoveRenderableType( const char *pszName ) = 0;
	virtual void BeginRenderingViews( IRenderDevice *pRenderDevice, bool bUpdateStats ) = 0;
	virtual void Unk_AddView( void *p ) = 0;
	virtual void Unk_AddDynamicView( void *p ) = 0;
	virtual void Unk_BeginRenderingDynamicView( ISceneView *pView ) = 0;
	virtual void Unk_AddDependentView( void *p ) = 0;
	virtual void FinishRenderingViews() = 0;
	virtual bool WaitForRenderingToComplete() = 0;
	virtual bool IsRenderingBusy() = 0;
	virtual void SetSceneObjectBounds( CSceneObject *pObject, const Vector &vecMins, const Vector &vecMaxs ) = 0;
	virtual void unk029() = 0;

	// Returns FLT_MAX extents when the object has no bounds yet.
	virtual void GetSceneObjectBounds( CSceneObject *pObject, Vector *pMins, Vector *pMaxs ) = 0;

	virtual void unk031() = 0;
	virtual void unk032() = 0;
	virtual void unk033() = 0;
	virtual void unk034() = 0;
	virtual void unk035() = 0;

	// Creates a CDebugTextObjectDesc object.
	virtual void Unk_CreateDebugTextSceneObject( void *p ) = 0;

	virtual void unk037() = 0;

	virtual CSceneLightObject *CreateLight( const void *pLightDesc, ISceneWorld *pWorld ) = 0;

	virtual CSceneObject *CreateSkyBox( void *pMaterial, ISceneWorld *pWorld ) = 0;
	virtual void unk040() = 0;
	virtual void unk041() = 0;
	virtual void unk042() = 0;
	virtual void unk043() = 0;
	virtual ISceneWorld *CreateWorld( const char *pszDebugName ) = 0;
	virtual void DestroyWorld( ISceneWorld *pWorld ) = 0;
	virtual void unk046() = 0;
	virtual void unk047() = 0;
	virtual void unk048() = 0;
	virtual void unk049() = 0;
	virtual void unk050() = 0;
	virtual HRenderTexture GetWellKnownTexture( int nTexture ) = 0;
	virtual IMaterial2 *Unk_GetWellKnownMaterial( int nMaterial ) = 0;
	virtual HMaterialWeak GetWellKnownMaterialHandle( int nMaterial ) = 0;
	virtual void unk054() = 0;
	virtual void unk055() = 0;
	virtual void unk056() = 0;
	virtual void unk057() = 0;
	virtual void unk058() = 0;
	virtual void unk059() = 0;
	virtual void Unk_AddTextureToSet( void *p ) = 0;
	virtual void unk061() = 0;
	virtual void unk062() = 0;
	virtual void unk063() = 0;
	virtual void unk064() = 0;
	virtual void unk065() = 0;
	virtual void unk066() = 0;
	virtual void Unk_CreateSceneViewDebugOverlays( void *p ) = 0;
	virtual void Unk_DestroySceneViewDebugOverlays( void *p ) = 0;
	virtual void unk069() = 0;
	virtual void unk070() = 0;
	virtual void unk071() = 0;

	virtual void Unk_InitSceneObject( void *p ) = 0;

	virtual void Unk_SetSceneObjectWorld( void *p ) = 0;

	virtual void FrameUpdate( bool bUnk ) = 0;
	virtual void Unk_CreateScratchRenderTargets( void *p ) = 0;

	virtual void Unk_GetScratchRenderTarget( void *pUnk1, bool bUnk, void *pUnk2 ) = 0;

	virtual void Unk_GetScratchRenderTarget( int nWidth, int nHeight, bool bUnk1, bool bUnk2, uint32 *pUnk ) = 0;
	virtual void SetMainSwapChain( SwapChainHandle_t hSwapChain ) = 0;
	virtual void unk079() = 0;
	virtual void unk080() = 0;
	virtual void unk081() = 0;
	virtual void unk082() = 0;
	virtual void Unk_PrintPerFrameStats( void *p ) = 0;
	virtual SceneSystemPerFrameStats_t &GetPerFrameStats() = 0;
	virtual void unk085() = 0;
	virtual void unk086() = 0;
	virtual void unk087() = 0;
	virtual void unk088() = 0;

	virtual void Unk_AllocateTransforms( int nCount, int *pFirstIndex, void **ppTransforms ) = 0;

	virtual void Unk_AllocateTransforms( int nUnk, int nCount, int *pFirstIndex, void **ppTransforms ) = 0;
	virtual void unk091() = 0;
	virtual void unk092() = 0;
	virtual void unk093() = 0;
	virtual void unk094() = 0;
	virtual void unk095() = 0;
	virtual void unk096() = 0;
	virtual void unk097() = 0;
	virtual void unk098() = 0;
	virtual void unk099() = 0;
	virtual void unk100() = 0;
	virtual void unk101() = 0;

	// Called from the CSceneObject destructor.
	virtual void Unk_DestructSceneObject( CSceneObject *pObject ) = 0;

	virtual void unk103() = 0;

	virtual void Unk_AddTemplateView( void *p ) = 0;

	virtual void Unk_RegisterTemplateViewFactory( void *p ) = 0;
	virtual void Unk_UnregisterTemplateViewFactory( void *p ) = 0;
	virtual void unk107() = 0;
	virtual void unk108() = 0;
	virtual void unk109() = 0;
	virtual void unk110() = 0;
	virtual void unk111() = 0;
	virtual void unk112() = 0;
	virtual void unk113() = 0;
	virtual void unk114() = 0;
	virtual void unk115() = 0;
	virtual void Unk_LoadSparseShadowTreeManifest( void *p ) = 0;
	virtual void Unk_UnloadSparseShadowTreeManifest() = 0;
	virtual void Unk_GetSparseShadowTree( void *p ) = 0;
	virtual void Unk_CreateSparseShadowTree( void *p ) = 0;
	virtual void Unk_DestroySparseShadowTree( void *p ) = 0;
	virtual void unk121() = 0;
	virtual void unk122() = 0;
	virtual void unk123() = 0;
	virtual void unk124() = 0;
	virtual void unk125() = 0;
	virtual void unk126() = 0;
	virtual void unk127() = 0;
	virtual void unk128() = 0;
	virtual void unk129() = 0;
	virtual void unk130() = 0;
	virtual void unk131() = 0;
	virtual void unk132() = 0;
	virtual void unk133() = 0;
	virtual void unk134() = 0;
	virtual void unk135() = 0;
	virtual void unk136() = 0;
	virtual void unk137() = 0;
	virtual void unk138() = 0;

	virtual void Unk_OnSceneObjectFlagsChanged( CSceneObject *pObject ) = 0;

	virtual void unk140() = 0;
	virtual float unk141() = 0;
	virtual void unk142() = 0;
	virtual void unk143() = 0;
	virtual void unk144() = 0;
	virtual void unk145() = 0;
	virtual void Unk_GenerateCompositeMorphTextureAtlas( void *p ) = 0;

	// Gates ISceneUtils::CreateVolumetricFog.
	virtual bool Unk_IsVolumetricFogEnabled() = 0;

	// Gates ISceneUtils::CreateWindController.
	virtual bool Unk_IsWindEnabled() = 0;

	virtual bool unk149() = 0;
	virtual bool unk150() = 0;

	// Gates ISceneUtils::CreateCharacterDecalRenderer.
	virtual bool Unk_IsCharacterDecalRenderingEnabled() = 0;

	virtual void unk152() = 0;
	virtual void unk153() = 0;
	virtual void unk154() = 0;
	virtual void unk155() = 0;
	virtual void unk156() = 0;
	virtual void Unk_CreateAOProxySceneObject( void *p ) = 0;

	// Reads ao_proxy_capsule_list and ao_proxy_box_list.
	virtual void Unk_LoadAOProxies( void *p ) = 0;

	virtual void Unk_UpdateAOProxySceneObject( void *p ) = 0;
	virtual void unk160() = 0;
	virtual void unk161() = 0;
	virtual void unk162() = 0;
	virtual void unk163() = 0;
	virtual void unk164() = 0;
	virtual void unk165() = 0;
	virtual void unk166() = 0;
	virtual void unk167() = 0;
	virtual void unk168() = 0;
	virtual void unk169() = 0;
	virtual void unk170() = 0;
	virtual void unk171() = 0;
	virtual void unk172() = 0;
	virtual IRayTraceSceneWorld *CreateRayTraceWorld( const char *pszDebugName, int nMaxRayTypes, bool bUnk ) = 0;
	virtual void DestroyRayTraceWorld( IRayTraceSceneWorld *pWorld ) = 0;
	virtual void unk175() = 0;
	virtual void unk176() = 0;
	virtual void unk177() = 0;
	virtual void unk178() = 0;
	virtual void unk179() = 0;
	virtual void unk180() = 0;
	virtual void unk181() = 0;
	virtual void unk182() = 0;
	virtual void unk183() = 0;
	virtual void unk184() = 0;
	virtual void unk185() = 0;
	virtual void Unk_CreateDistanceFieldInstanceSceneObject( void *p ) = 0;
	virtual void unk187() = 0;
	virtual void unk188() = 0;
	virtual void unk189() = 0;
	virtual void unk190() = 0;
	virtual void AcquireImGuiLock() = 0;
	virtual void Unk_ReleaseImGuiLock( bool bUnk ) = 0;
	virtual bool Unk_IsImGuiLocked() = 0;
	virtual bool Unk_IsImGuiLockedByCurrentThread() = 0;
	virtual void unk195() = 0;
	virtual void unk196() = 0;
	virtual void unk197() = 0;
};

#endif // ISCENESYSTEM_H
