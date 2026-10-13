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
#include <bitmap/imageformat_declarations.h>

#include <functional>

class CSceneObject;
class CSceneLightObject;
class CSceneBarnLightObject;
class CSceneLightProbeVolumeObject;
class CEnvMapSceneObject;
class CSceneDebugTextObject;
class CSceneAnimatableObject;
class CTextLayoutInfo;
class Vector2D;
class ISceneObjectDesc;
class ISceneWorld;
class ISceneView;
class IRenderDevice;
class IRenderContext;
class ISceneLayer;
class CBufferString;
class IRayTraceSceneWorld;
class ISceneViewDebugOverlays;
class ISparseShadowTree;
class CViewId;
class CFrustum;
struct AABB_t;
struct OBB_t;
struct RenderViewport_t;
struct LightDesc_t;
struct MeshBuilderBuffers_t;
struct SceneObjectBuffers_t;
class CRenderAttributes;
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

	virtual CSceneObject *CreateSceneObjectFromBuffers( ISceneWorld *pWorld, VertexBufferHandle_t hVertexBuffer, IndexBufferHandle_t hIndexBuffer, HMaterialWeak hMaterial, int nVertices, int nIndices, const Vector &vecBoundsMin, const Vector &vecBoundsMax, int nPrimitiveType, const char *pszDescName, bool bOwnVBAndIB, uint64 nFlags ) = 0;

	virtual CSceneObject *CreateSceneObjectFromBuffers( uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld, VertexBufferHandle_t hVertexBuffer, IndexBufferHandle_t hIndexBuffer, HMaterialWeak hMaterial, int nVertices, int nIndices, const Vector &vecBoundsMin, const Vector &vecBoundsMax, int nPrimitiveType, int16 nDrawPrimitiveFlags, const char *pszDescName, bool bOwnVBAndIB ) = 0;

	// Creates a CMeshBuilderSceneObjectDesc object.
	virtual CSceneObject *CreateSceneObjectFromStaticBuffers( uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld, const MeshBuilderBuffers_t &buffers, HMaterialWeak hMaterial, bool bDeleteAtFrameEnd ) = 0;

	virtual CSceneAnimatableObject *CreateAnimatableSceneObjectFromBuffers( ISceneWorld *pWorld, HMaterialWeak hMaterial, SceneObjectBuffers_t &data, uint64 nFlags, uint64 nTypeFlags, bool bOwnVBAndIB ) = 0;

	virtual void DeleteSceneObject( CSceneObject *pObject ) = 0;

	virtual ISceneObjectDesc *FindRenderableType( const char *pszName ) = 0;

	virtual void AddRenderableType( const char *pszName, ISceneObjectDesc *pDesc ) = 0;
	virtual void RemoveRenderableType( const char *pszName ) = 0;
	virtual void BeginRenderingViews( IRenderDevice *pRenderDevice, bool bUpdateStats ) = 0;
	virtual ISceneView *AddView( const char *pszViewName, const CViewId &viewId, const CFrustum &frustum, SwapChainHandle_t hSwapChain, ISceneWorld *pWorld, const RenderViewport_t &viewport, const uint32 *pVis, int nPriority ) = 0;
	virtual ISceneView *AddDynamicView( const char *pszViewName, const ISceneView *pParentView, const CFrustum &frustum, SwapChainHandle_t hSwapChain, ISceneWorld *pWorld, const RenderViewport_t &viewport, const uint32 *pVis, int nPriority, int nViewProducerIndex ) = 0;
	virtual void BeginRenderingDynamicView( ISceneView *pView ) = 0;
	virtual IRenderContext *RenderEmbeddedView( ISceneView *pView, ISceneLayer *pLayer ) = 0;
	virtual void FinishRenderingViews() = 0;
	virtual bool WaitForRenderingToComplete() = 0;
	virtual bool IsRenderingBusy() = 0;
	virtual void SetSceneObjectBounds( CSceneObject *pObject, const Vector &vecMins, const Vector &vecMaxs ) = 0;
	virtual void SetObjectBoundsInfinite( CSceneObject *pObject ) = 0;

	// Returns FLT_MAX extents when the object has no bounds yet.
	virtual void GetSceneObjectBounds( CSceneObject *pObject, Vector *pMins, Vector *pMaxs ) = 0;

	// Returns the shadow LOD index for the view (convar override, then the view, then the object) and writes 1 << index to pLODMask.
	virtual int unk031( CSceneObject *pObject, ISceneView *pView, uint8 *pLODMask ) = 0;

	virtual void SetPVSFromClusterMembership( CSceneObject *pObject, const uint32 *pClusters, int nClusterCount ) = 0;

	// Recompute the PVS data of the object from bounds or an oriented box in its world, unless PVS_NO_RECOMPUTE is set on it.
	// Overloads: MSVC places the oriented box one first.
	virtual void unk033( CSceneObject *pObject, const AABB_t &bounds, bool bUnk ) = 0;
	virtual void unk033( CSceneObject *pObject, const OBB_t &bounds, bool bUnk ) = 0;

	// Makes the object visible from every cluster of its world.
	virtual void unk035( CSceneObject *pObject ) = 0;

	virtual CSceneDebugTextObject *CreateTextObject( Vector2D *pEndPos, ISceneWorld *pWorld, const CTextLayoutInfo *pTextInfo, bool bPositionIn3DWorldSpace, const Vector &vLocal3DPosition ) = 0;

	// Overloads: MSVC places the barn light one first.
	virtual CSceneLightObject *CreateLight( const LightDesc_t &desc, ISceneWorld *pWorld ) = 0;
	virtual CSceneBarnLightObject *CreateLight( const void *pBarnLightDesc, ISceneWorld *pWorld ) = 0;

	virtual CSceneObject *CreateSkyBox( HMaterialWeak hMaterial, ISceneWorld *pWorld ) = 0;
	virtual CSceneLightProbeVolumeObject *CreateLightProbeVolume( ISceneWorld *pWorld ) = 0;
	// Tests the lights and barn lights that belong to one of the worlds of the view.
	virtual bool unk041( ISceneView *pView ) = 0;
	virtual void MarkLightProbeVolumeObjectUpdated( CSceneLightProbeVolumeObject *pLightProbe ) = 0;
	virtual void MarkEnvironmentMapObjectUpdated( CEnvMapSceneObject *pEnvMap ) = 0;
	virtual ISceneWorld *CreateWorld( const char *pszDebugName ) = 0;
	virtual void DestroyWorld( ISceneWorld *pWorld ) = 0;
	virtual VertexBufferHandle_t GetWellKnownVertexBuffer( int nBuffer ) = 0;
	virtual IndexBufferHandle_t GetWellKnownIndexBuffer( int nBuffer ) = 0;
	virtual void *GetWellKnownInputLayout( int nLayout, uint8 nVariation ) = 0;
	virtual RenderInputLayoutField_t *GetWellKnownInputLayoutFields( int nLayout, int *pFieldCount ) = 0;
	virtual void *unk050( int nIndex ) = 0;
	virtual HRenderTexture GetWellKnownTexture( int nTexture ) = 0;
	virtual IMaterial2 *GetWellKnownMaterial( int nMaterial ) = 0;
	virtual HMaterialWeak GetWellKnownMaterialHandle( int nMaterial ) = 0;
	virtual void *GetWellKnownShape( int nShape ) = 0;
	virtual HRenderTexture GetWellKnownRenderTarget( int nRenderTarget, int nSize ) = 0;
	virtual HRenderTexture GetWellKnownRenderTarget( int nRenderTarget, int nWidth, int nHeight ) = 0;
	virtual void GetRenderTargetSize( int nSize, int *pWidth, int *pHeight ) = 0;
	virtual int BestRenderTargetSize( int nWidth, int nHeight ) = 0;
	virtual void SetMinimumFrameBufferRenderTargetSize( int nWidth, int nHeight ) = 0;
	// Texture sets, guarded by one global mutex.
	virtual bool AddTextureToSet( HRenderTexture hTexture, int nSet ) = 0;
	virtual void unk061( HRenderTexture hTexture, int nSet ) = 0;
	virtual HRenderTexture unk062( int nIndex, int nSet ) = 0;
	virtual int unk063( HRenderTexture hTexture, int nSet ) = 0;
	virtual int unk064( int nValue, int nSet ) = 0;
	virtual void unk065() = 0;
	virtual uint16 unk066( int nSet ) = 0;

	virtual ISceneViewDebugOverlays *CreateSceneViewDebugOverlayList( int nTimingType, const char *pszName ) = 0;
	virtual void DestroySceneViewDebugOverlayList( ISceneViewDebugOverlays *pOverlays ) = 0;

	// Creates and destroys debug overlay lists kept in a second list.
	virtual ISceneViewDebugOverlays *unk069() = 0;
	virtual void unk070( ISceneViewDebugOverlays *pOverlays ) = 0;

	virtual void unk071( bool bUnk ) = 0;

	virtual void InitObject( CSceneObject *pObject, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;

	virtual void MoveObjectToWorld( CSceneObject *pObject, ISceneWorld *pWorld ) = 0;

	virtual void FrameUpdate( bool bUnk ) = 0;
	virtual void AllocateScratchRenderTargets( void *p ) = 0;

	virtual void ComputePipelineRenderTargetInfo( SwapChainHandle_t hSwapChain, bool bUnk, PipelineRenderTargetInfo_t *pInfo ) = 0;

	virtual void ComputePipelineRenderTargetInfo( int nWidth, int nHeight, RenderMultisampleType_t nMultisampleType, bool bUnk, PipelineRenderTargetInfo_t *pInfo ) = 0;
	virtual void SetMainSwapChain( SwapChainHandle_t hSwapChain ) = 0;
	virtual void SetCanAllocateScratchRenderTargetsOnDemand( bool bCanAllocate ) = 0;
	virtual void *GetSceneSystemStats() = 0;
	virtual void *GetSceneSystemStatsMutex() = 0;
	virtual void GetStatsSummaryString( CBufferString *pOut ) = 0;
	virtual void PrintPerFrameStats( CBufferString *pOut, const char *pszHeader ) = 0;
	virtual SceneSystemPerFrameStats_t &GetPerFrameStats() = 0;
	virtual uint32 GetFinishRenderingViewsCounter() const = 0;
	virtual uint32 GetFrameUpdateCounter() const = 0;
	virtual void BindTransformTextureAndInstanceIDBuffer( IRenderContext *pRenderContext, int nVBSlot, int nTransformSlotIndex, bool bUnk ) = 0;
	// Binds the transform buffer at offset 0.
	virtual void unk088( IRenderContext *pRenderContext, int nVBSlot ) = 0;

	virtual void AllocateFrameScopedTransforms( int nCount, int *pFirstIndex, void **ppTransforms ) = 0;

	virtual void AllocateFrameScopedTransforms( int nUnk, int nCount, int *pFirstIndex, void **ppTransforms ) = 0;
	// Allocate frame scoped 8 and 4 byte entries.
	virtual void *unk091( int nCount, int *pFirstIndex ) = 0;
	virtual void *unk092( int nCount, int *pFirstIndex ) = 0;
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

	virtual ISceneView *AddViewFromTemplate( const char *pszViewTemplateName, const char *pszViewName, const ISceneView *pParentView, const CFrustum &frustum, SwapChainHandle_t hSwapChain, ISceneWorld *pWorld, const RenderViewport_t &viewport, const uint32 *pVis, const CRenderAttributes *pAttributes ) = 0;

	virtual void InstallTemplateViewFactory( void *p ) = 0;
	virtual void RemoveTemplateViewFactory( const char *pszName ) = 0;
	virtual void QueueSceneObjectUpdateJob( std::function< void() > &&job ) = 0;
	virtual void ExecuteQueuedSceneObjectUpdateJobs() = 0;
	virtual void InstallMaterialDependentSceneObjectUpdater( const std::function< void( CSceneObject * ) > &updater ) = 0;
	virtual void unk110() = 0;
	virtual void unk111() = 0;
	virtual void SetIgnoreAlphaFade( bool bIgnoreAlphaFade ) = 0;
	virtual void unk113() = 0;

	// Destroys every sparse shadow tree, then loads them again when nUnk is positive.
	virtual void unk114( int nUnk ) = 0;

	// Destroys every sparse shadow tree.
	virtual void unk115() = 0;

	virtual void LoadSparseShadowTreeManifest() = 0;
	virtual void UnloadSparseShadowTreeManifest() = 0;
	virtual ISparseShadowTree *GetSparseShadowTree( int nIndex, bool bOnlyIfEnabled ) = 0;
	virtual void CreateSparseShadowTree( void *pUnk, int nIndex ) = 0;
	virtual void DestroySparseShadowTree( int nIndex ) = 0;

	virtual void unk121() = 0;

	// Sets the per class settings byte that GetObjectSettings returns.
	virtual void unk122( uint8 nClass, uint8 nSettings ) = 0;

	virtual void SceneObject_ScheduleDirtyUpdate( CSceneObject *pObject ) = 0;
	virtual void SceneObject_UnscheduleDirtyUpdate( CSceneObject *pObject ) = 0;
	virtual void SceneObject_AttachExtraData( CSceneObject *pObject ) = 0;
	virtual void SceneObject_AttachLightingCache( CSceneObject *pObject ) = 0;
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
	virtual void GenerateCompositeMorphTextureAtlas( void *p ) = 0;

	// Gates ISceneUtils::CreateVolumetricFog.
	virtual bool VolumetricFogEnabled() = 0;

	// Gates ISceneUtils::CreateWindController.
	virtual bool IsWindEnabled() = 0;

	virtual bool NonTexturedGradientFogEnabled() = 0;
	virtual bool CubemapFogEnabled() = 0;

	// Gates ISceneUtils::CreateCharacterDecalRenderer.
	virtual bool IsCharacterDecalRenderingEnabled() = 0;

	// Add and remove a pair of pointers.
	virtual void unk152( void *pUnk1, void *pUnk2 ) = 0;
	virtual void unk153( void *pUnk1, void *pUnk2 ) = 0;
	virtual ISceneView *FindView( const char *pszName ) = 0;
	virtual ImageFormat GetPrimaryRenderTargetFormat() = 0;
	virtual ImageFormat GetBackBufferFormat() = 0;
	virtual CSceneObject *CreateAOProxySceneObject( uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;

	// Reads ao_proxy_capsule_list and ao_proxy_box_list.
	virtual void LoadAOProxies( void *p ) = 0;

	virtual void UpdateAOProxySceneObject( void *p ) = 0;
	virtual void unk160() = 0;
	virtual void unk161() = 0;
	virtual void unk162() = 0;
	virtual void unk163() = 0;
	virtual void unk164() = 0;
	virtual void unk165() = 0;
	virtual void unk166() = 0;
	virtual void unk167() = 0;
	virtual int unk168() = 0;
	virtual void unk169() = 0;
	virtual bool unk170() = 0;
	virtual void unk171() = 0;
	// ORs the state flags of the overlay lists created by CreateSceneViewDebugOverlayList.
	virtual int unk172( int *pOut ) = 0;
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
	virtual CSceneObject *CreateDistanceFieldInstanceSceneObject( uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;
	virtual void unk187() = 0;
	virtual void unk188() = 0;
	virtual void unk189( void *pUnk ) = 0;
	virtual void *unk190( int nIndex ) = 0;
	virtual void AcquireImGuiLock() = 0;
	virtual void ReleaseImGuiLock( bool bUnk ) = 0;
	virtual bool IsImGuiLocked() = 0;
	virtual bool IsImGuiLockedByCurrentThread() = 0;
	virtual void unk195() = 0;
	virtual int unk196() = 0;
	virtual void unk197() = 0;
};

#endif // ISCENESYSTEM_H
