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
#include <resourcesystem/resourcehandle.h>
#include <tier0/utlstringtoken.h>

#include <functional>

class CSceneObject;
class CSceneLightObject;
class ISceneObjectDesc;
class ISceneWorld;
class ISceneView;
class IRenderDevice;
class IRayTraceSceneWorld;
class ISceneViewDebugOverlays;
class CViewId;
class CFrustum;
struct RenderViewport_t;
struct SceneSystemPerFrameStats_t;
struct RenderInputLayoutField_t;
struct PipelineRenderTargetInfo_t;
enum RenderMultisampleType_t : uint8;

FORWARD_DECLARE_HANDLE( SwapChainHandle_t );
FORWARD_DECLARE_HANDLE( VertexBufferHandle_t );
FORWARD_DECLARE_HANDLE( IndexBufferHandle_t );

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
	virtual ISceneView *AddView( const char *pszViewName, const CViewId &viewId, const CFrustum &frustum, SwapChainHandle_t hSwapChain, ISceneWorld *pWorld, const RenderViewport_t &viewport, const uint32 *pUnk, int nUnk ) = 0;
	virtual ISceneView *AddDynamicView( const char *pszViewName, const ISceneView *pParentView, const CFrustum &frustum, SwapChainHandle_t hSwapChain, ISceneWorld *pWorld, const RenderViewport_t &viewport, const uint32 *pUnk, int nUnk1, int nUnk2 ) = 0;
	virtual void Unk_BeginRenderingDynamicView( ISceneView *pView ) = 0;
	virtual void Unk_AddDependentView( void *p ) = 0;
	virtual void FinishRenderingViews() = 0;
	virtual bool WaitForRenderingToComplete() = 0;
	virtual bool IsRenderingBusy() = 0;
	virtual void SetSceneObjectBounds( CSceneObject *pObject, const Vector &vecMins, const Vector &vecMaxs ) = 0;
	virtual void SetObjectBoundsInfinite( CSceneObject *pObject ) = 0;

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
	virtual VertexBufferHandle_t GetWellKnownVertexBuffer( int nBuffer ) = 0;
	virtual IndexBufferHandle_t GetWellKnownIndexBuffer( int nBuffer ) = 0;
	virtual void *GetWellKnownInputLayout( int nLayout, uint8 nVariation ) = 0;
	virtual RenderInputLayoutField_t *GetWellKnownInputLayoutFields( int nLayout, int *pFieldCount ) = 0;
	virtual void unk050() = 0;
	virtual HRenderTexture GetWellKnownTexture( int nTexture ) = 0;
	virtual IMaterial2 *Unk_GetWellKnownMaterial( int nMaterial ) = 0;
	virtual HMaterialWeak GetWellKnownMaterialHandle( int nMaterial ) = 0;
	virtual void *GetWellKnownShape( int nShape ) = 0;
	virtual void unk055() = 0;
	virtual void unk056() = 0;
	virtual void GetRenderTargetSize( int nSize, int *pWidth, int *pHeight ) = 0;
	virtual int BestRenderTargetSize( int nWidth, int nHeight ) = 0;
	virtual void SetMinimumFrameBufferRenderTargetSize( int nWidth, int nHeight ) = 0;
	virtual void Unk_AddTextureToSet( void *p ) = 0;
	virtual void unk061() = 0;
	virtual void unk062() = 0;
	virtual void unk063() = 0;
	virtual void unk064() = 0;
	virtual void unk065() = 0;
	virtual void unk066() = 0;
	virtual void Unk_CreateSceneViewDebugOverlayList( void *p ) = 0;
	virtual void DestroySceneViewDebugOverlayList( ISceneViewDebugOverlays *pOverlays ) = 0;
	virtual void unk069() = 0;
	virtual void unk070() = 0;
	virtual void unk071() = 0;

	virtual void InitObject( CSceneObject *pObject, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;

	virtual void MoveObjectToWorld( CSceneObject *pObject, ISceneWorld *pWorld ) = 0;

	virtual void FrameUpdate( bool bUnk ) = 0;
	virtual void Unk_AllocateScratchRenderTargets( void *p ) = 0;

	virtual void ComputePipelineRenderTargetInfo( SwapChainHandle_t hSwapChain, bool bUnk, PipelineRenderTargetInfo_t *pInfo ) = 0;

	virtual void ComputePipelineRenderTargetInfo( int nWidth, int nHeight, RenderMultisampleType_t nMultisampleType, bool bUnk, PipelineRenderTargetInfo_t *pInfo ) = 0;
	virtual void SetMainSwapChain( SwapChainHandle_t hSwapChain ) = 0;
	virtual void SetCanAllocateScratchRenderTargetsOnDemand( bool bCanAllocate ) = 0;
	virtual void *GetSceneSystemStats() = 0;
	virtual void *GetSceneSystemStatsMutex() = 0;
	virtual void unk082() = 0;
	virtual void Unk_PrintPerFrameStats( void *p ) = 0;
	virtual SceneSystemPerFrameStats_t &GetPerFrameStats() = 0;
	virtual int GetFrameCounter() const = 0;
	virtual void unk086() = 0;
	virtual void unk087() = 0;
	virtual void unk088() = 0;

	virtual void Unk_AllocateTransforms( int nCount, int *pFirstIndex, void **ppTransforms ) = 0;

	virtual void Unk_AllocateTransforms( int nUnk, int nCount, int *pFirstIndex, void **ppTransforms ) = 0;
	virtual void unk091() = 0;
	virtual void unk092() = 0;
	virtual void unk093() = 0;
	virtual ISceneView *CreateTemplateView( const char *pszName, ISceneWorld *pWorld, const uint32 *pUnk ) = 0;
	virtual CViewId CreateViewId( int nPlayer ) = 0;
	virtual void DeleteSceneObjectAtFrameEnd( CSceneObject *pObject ) = 0;
	virtual void GetAllActiveSceneWorlds( CUtlVector< ISceneWorld * > &worlds ) = 0;
	virtual uint8 FindOrCreateSceneObjectClass( const char *pszName ) = 0;
	virtual void SetObjectClass( CSceneObject *pObject, uint8 nClass ) = 0;
	virtual void GetObjectClassName( uint8 nClass, char *pszBuffer, int nBufferSize ) = 0;
	virtual void ConstructSceneObject( CSceneObject *pObject ) = 0;

	// Called from the CSceneObject destructor.
	virtual void DestructSceneObject( CSceneObject *pObject ) = 0;

	virtual void unk103() = 0;

	virtual void Unk_AddViewFromTemplate( void *p ) = 0;

	virtual void Unk_InstallTemplateViewFactory( void *p ) = 0;
	virtual void RemoveTemplateViewFactory( const char *pszName ) = 0;
	virtual void QueueSceneObjectUpdateJob( std::function< void() > &&job ) = 0;
	virtual void ExecuteQueuedSceneObjectUpdateJobs() = 0;
	virtual void InstallMaterialDependentSceneObjectUpdater( const std::function< void( CSceneObject * ) > &updater ) = 0;
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
	virtual void SceneObject_ScheduleDirtyUpdate( CSceneObject *pObject ) = 0;
	virtual void SceneObject_UnscheduleDirtyUpdate( CSceneObject *pObject ) = 0;
	virtual void SceneObject_AttachExtraData( CSceneObject *pObject ) = 0;
	virtual void unk126() = 0;
	virtual void SceneObject_UpdateFlagsBasedOnMaterial( CSceneObject *pObject ) = 0;
	virtual void unk128() = 0;
	virtual void unk129() = 0;
	virtual void unk130() = 0;

	// nPlayerIndex -1 applies to every player.
	virtual void SceneObject_SetVisibilityToPlayerIndex( CSceneObject *pObject, int nPlayerIndex, bool bUnk, bool bVisible ) = 0;

	virtual void SceneObject_SetSplitScreenMask( CSceneObject *pObject, uint32 nMask ) = 0;
	virtual void SceneObject_AddChild( CSceneObject *pParent, CUtlStringToken token, CSceneObject *pChild, uint32 nFlags ) = 0;
	virtual void SceneObject_RemoveChild( CSceneObject *pParent, CSceneObject *pChild ) = 0;
	virtual CSceneObject *SceneObject_FindChild( const CSceneObject *pParent, CUtlStringToken token ) = 0;
	virtual void SceneObject_MirrorTransformToChildSceneObjects( const CSceneObject *pObject ) = 0;
	virtual void SceneObject_MirrorVisibilityToChildSceneObjects( const CSceneObject *pObject ) = 0;
	virtual void SceneObject_SetForceLayerID( CSceneObject *pObject, CUtlStringToken layerId ) = 0;
	virtual void SceneObject_PostFlagsUpdate( CSceneObject *pObject ) = 0;
	virtual int RegisterExtraDataAllocation( const char *pszName ) = 0;
	virtual float GetCurrentRenderTime() = 0;
	virtual void AdvanceNonRenderedFrame() = 0;
	virtual uint8 GetObjectSettings( const CSceneObject *pObject ) const = 0;
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
