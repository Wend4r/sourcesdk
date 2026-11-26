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
#include <resourcesystem/resourcehandletypes.h>
#include <scenesystem/isceneview.h>

#include <functional>

class CSceneObject;
class CSceneLightObject;
class ISceneObjectDesc;
class ISceneWorld;
class ISceneView;
class ISceneLayer;
class ISceneViewDebugOverlays;
class ISceneSystemListener;
class ISceneSystemLightListener;
class ISceneObjectDeletionListener;
class CFrustum;
class IRenderDevice;
class IRenderContext;
class IRayTraceSceneWorld;
class CRenderAttributes;
class CBufferString;
class CTextLayoutInfo;
class CPVSBits;
class CSceneSystemStencilState;
class CDistanceFieldScene;
class CSparseShadowTree;
class FourVectors;
class Vector2D;
class Vector4D;
class QAngle;
struct AABB_t;
struct matrix3x4_t;
struct LightDesc_t;
struct MeshBuilderBuffers_t;
struct SceneObjectBuffers_t;
struct SceneScratchRenderTargetInfo_t;
struct PipelineRenderTargetInfo_t;
struct SceneSystemPerFrameStats_t;
struct SceneSystemTransformEntry_t;
struct SceneSystemFrameStatsEntry_t;
struct RenderInputLayoutField_t;
struct CSimpleStaticShapeDesc;
struct PerViewConstantBuffer_t;
struct RenderTargetDesc_t;
struct SceneObjectTrackEntry_t;
class VMatrix;

FORWARD_DECLARE_HANDLE( SwapChainHandle_t );
FORWARD_DECLARE_HANDLE( RenderBufferHandle_t );
FORWARD_DECLARE_HANDLE( RenderInputLayout_t );

// Builds a template view; registered by name with InstallTemplateViewFactory
typedef ISceneView *SceneTemplateViewFactoryFunc_t( const char *pszName, const ISceneView *pParentView, void *pContext );

abstract_class ISceneSystem : public IAppSystem
{
public:
	virtual CSceneObject *CreateSceneObject( ISceneObjectDesc *pDesc, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;

	// nVertexCount and nIndexCount, the bounds, nPrimitiveType and bMeshSetupFlag go to the mesh setup of the object unchanged.
	// pszDescName picks a registered object type, NULL a plain mesh. The first overload derives the flags from the material
	virtual CSceneObject *CreateSceneObjectFromBuffers( ISceneWorld *pWorld, RenderBufferHandle_t hVertexBuffer, RenderBufferHandle_t hIndexBuffer, ResourceHandle_t hMaterial, int nVertexCount, int nIndexCount, const Vector &vecMins, const Vector &vecMaxs, int nPrimitiveType, const char *pszDescName, bool bMeshSetupFlag, uint64 nTypeFlags ) = 0;
	virtual CSceneObject *CreateSceneObjectFromBuffers( uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld, RenderBufferHandle_t hVertexBuffer, RenderBufferHandle_t hIndexBuffer, ResourceHandle_t hMaterial, int nVertexCount, int nIndexCount, const Vector &vecMins, const Vector &vecMaxs, int nPrimitiveType, int16 nDrawPrimitiveFlags, const char *pszDescName, bool bMeshSetupFlag ) = 0;

	// Creates the "Meshbuilder2" vertex and index buffers from the builder output
	virtual CSceneObject *CreateSceneObjectFromStaticBuffers( uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld, const MeshBuilderBuffers_t *pBuffers, ResourceHandle_t hMaterial, bool bMeshSetupFlag ) = 0;

	// An "AnimatableSceneObjectDesc" object over the given buffers
	virtual CSceneObject *CreateAnimatableSceneObjectFromBuffers( ISceneWorld *pWorld, ResourceHandle_t hMaterial, const SceneObjectBuffers_t &buffers, uint64 nFlags, uint64 nTypeFlags, bool bMeshSetupFlag ) = 0;

	virtual void DeleteSceneObject( CSceneObject *pObject ) = 0;

	// Object types ("renderable types") by name
	virtual ISceneObjectDesc *GetSceneObjectDesc( const char *pszName ) = 0;
	virtual void AddSceneObjectType( const char *pszName, ISceneObjectDesc *pDesc ) = 0;
	virtual void RemoveSceneObjectType( const char *pszName ) = 0;

	virtual void BeginRenderingViews( IRenderDevice *pRenderDevice, bool bUpdateStats ) = 0;
	// Between BeginRenderingViews and FinishRenderingViews; the view renders into hSwapChain.
	// A non-null pWorld is added with ISceneView::AddWorldToRenderList( pWorld, pVisBits, true )
	virtual ISceneView *AddView( const char *pszName, const CViewId &viewId, const CFrustum &frustum, SwapChainHandle_t hSwapChain, ISceneWorld *pWorld, const RenderViewport_t &viewport, const uint32 *pVisBits, int nPriority ) = 0;
	virtual ISceneView *AddDynamicView( const char *pszName, const ISceneView *pParentView, const CFrustum &frustum, SwapChainHandle_t hSwapChain, ISceneWorld *pWorld, const RenderViewport_t &viewport, const uint32 *pVisBits, int nPriority, int nViewIndex ) = 0;
	virtual void BeginRenderingDynamicView( ISceneView *pView ) = 0;
	// Renders pView into the display list of pLayer; returns the render context of that display list
	virtual IRenderContext *RenderEmbeddedView( ISceneView *pView, ISceneLayer *pLayer ) = 0;
	virtual void FinishRenderingViews( float flFrameTime = 0.0f ) = 0;
	virtual bool WaitForRenderingToComplete() = 0;
	virtual bool IsWaitingOnRenderingJobs() = 0;

	virtual void SetObjectBounds( CSceneObject *pObject, const Vector &vecMins, const Vector &vecMaxs ) = 0;
	virtual void SetObjectBoundsInfinite( CSceneObject *pObject ) = 0;
	// Returns FLT_MAX extents when the object has no bounds yet.
	virtual void GetObjectBounds( const CSceneObject *pObject, Vector *pMins, Vector *pMaxs ) = 0;

	// Named by what it does: the LOD the object renders with in pView (a convar override, then the view's
	// override, then the object's forced LOD, then its model's LOD for the bounds); pLODMask gets 1 << LOD
	virtual int ComputeObjectLOD( const CSceneObject *pObject, ISceneView *pView, uint8 *pLODMask ) = 0;

	// The PVS bits of objects created with flag 8. Named by what they do, after SetPVSFromClusterMembership:
	// from the clusters at a point, in a box, or every cluster of the world
	virtual void SetPVSFromClusterMembership( CSceneObject *pObject, const int *pClusters, int nClusterCount ) = 0;
	virtual void SetPVSFromPoint( CSceneObject *pObject, const Vector &vecPoint, bool bIncludeNeighbours ) = 0;
	virtual void SetPVSFromBox( CSceneObject *pObject, const AABB_t &box, bool bIncludeNeighbours ) = 0;
	virtual void SetPVSToAllClusters( CSceneObject *pObject ) = 0;

	virtual CSceneObject *CreateTextObject( Vector2D *pPositions, ISceneWorld *pWorld, const CTextLayoutInfo *pLayout, bool bScreenSpace, const Vector &vecOrigin ) = 0;

	// Named by what it does: a light object of the light manager's eighth type (flags 0x1008) that copies desc
	virtual CSceneLightObject *CreateLightObject( const LightDesc_t &desc, ISceneWorld *pWorld ) = 0;
	// Omni, spot, directional or ortho light, by the type in the description
	virtual CSceneLightObject *CreateLight( const LightDesc_t &desc, ISceneWorld *pWorld ) = 0;
	virtual CSceneObject *CreateSkyBox( ResourceHandle_t hMaterial, ISceneWorld *pWorld ) = 0;
	virtual CSceneObject *CreateLightProbeVolume( ISceneWorld *pWorld ) = 0;

	// Named by what it does: refreshes the light objects that use the light token in the worlds pView renders
	virtual int UpdateLightsForViewWorlds( const ISceneView *pView ) = 0;

	// Both notify the listeners of the light manager
	virtual void MarkLightProbeVolumeObjectUpdated( CSceneObject *pLightProbeVolume ) = 0;
	virtual void MarkEnvironmentMapObjectUpdated( CSceneObject *pEnvMap ) = 0;

	virtual ISceneWorld *CreateWorld( const char *pszDebugName ) = 0;
	virtual void DestroyWorld( ISceneWorld *pWorld ) = 0;

	virtual RenderBufferHandle_t GetWellKnownVertexBuffer( int nBuffer ) = 0;
	virtual RenderBufferHandle_t GetWellKnownIndexBuffer( int nBuffer ) = 0;
	virtual RenderInputLayout_t GetWellKnownInputLayout( int nLayout, uint8 nVariation ) = 0;
	virtual const RenderInputLayoutField_t *GetWellKnownInputLayoutFields( int nLayout, int *pFieldCount ) = 0;
	// Named by what it does: a third table of well known render buffers, after the index buffers
	virtual RenderBufferHandle_t GetWellKnownBuffer( int nBuffer ) = 0;
	virtual HRenderTexture GetWellKnownTexture( int nTexture ) = 0;
	virtual IMaterial2 *GetWellKnownMaterial( int nMaterial ) = 0;
	virtual HMaterialWeak GetWellKnownMaterialHandle( int nMaterial ) = 0;
	virtual const CSimpleStaticShapeDesc &GetWellKnownShape( int nShape ) = 0;

	// Named by what they do: the scratch texture of nType (5 picks the current type) for a render target size class
	// (4 is none), or for the smallest size class that holds nWidth x nHeight. MSVC orders the two in reverse
	virtual HRenderTexture GetScratchRenderTargetTexture( int nType, int nSizeClass ) = 0;
	virtual HRenderTexture GetScratchRenderTargetTexture( int nType, int nWidth, int nHeight ) = 0;
	virtual void GetRenderTargetSize( int nSizeClass, int *pWidth, int *pHeight ) = 0;
	// Named by what it does: the smallest size class that holds nWidth x nHeight, 4 if none does
	virtual int GetRenderTargetSizeClass( int nWidth, int nHeight ) = 0;
	// Sets the full size and releases the scratch textures when it changes
	virtual void SetRenderTargetSize( int nWidth, int nHeight ) = 0;

	// Texture sets: reference counted lists of textures shared by every scene system user.
	// Named by what they do, after AddTextureToSet (its error message)
	virtual bool AddTextureToSet( HRenderTexture hTexture, int nSet ) = 0;
	virtual void RemoveTextureFromSet( HRenderTexture hTexture, int nSet ) = 0;
	virtual HRenderTexture GetTextureInSet( int nIndex, int nSet ) = 0;
	// -1 for a NULL texture or one not in the set
	virtual int FindTextureInSet( HRenderTexture hTexture, int nSet ) = 0;
	// The index of the entry registered with nId, -1 if none
	virtual int FindTextureInSetById( int nId, int nSet ) = 0;
	virtual HRenderTexture GetTextureSetTexture( int nSet ) = 0;
	// A counter of the set that changes when its textures do
	virtual int GetTextureSetVersion( int nSet ) = 0;

	virtual ISceneViewDebugOverlays *CreateSceneViewDebugOverlayList( int nTimingType, const char *pszName ) = 0;
	virtual void DestroySceneViewDebugOverlayList( ISceneViewDebugOverlays *pList ) = 0;
	// Named by what they do: a persistent list kept in a second set of lists
	virtual ISceneViewDebugOverlays *CreatePersistentSceneViewDebugOverlayList() = 0;
	virtual void DestroyPersistentSceneViewDebugOverlayList( ISceneViewDebugOverlays *pList ) = 0;
	// Named by what it does: a global flag of the debug overlays code
	virtual void SetDebugOverlaysEnabled( bool bEnabled ) = 0;

	virtual void InitObject( CSceneObject *pObject, uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;
	virtual void MoveObjectToWorld( CSceneObject *pObject, ISceneWorld *pWorld ) = 0;

	virtual void FrameUpdate( bool bRendering ) = 0;
	virtual void AllocateScratchRenderTargets( CUtlStringToken pipeline, int nWidth, int nHeight, const SceneScratchRenderTargetInfo_t *pInfo ) = 0;
	// MSVC orders the two in reverse
	virtual void ComputePipelineRenderTargetInfo( SwapChainHandle_t hSwapChain, bool bForceMSAAOff, PipelineRenderTargetInfo_t *pInfo ) = 0;
	virtual void ComputePipelineRenderTargetInfo( int nWidth, int nHeight, int nMultisampleType, bool bForceMSAAOff, PipelineRenderTargetInfo_t *pInfo ) = 0;
	virtual void SetMainSwapChain( SwapChainHandle_t hSwapChain ) = 0;

	// Named by what it does: a flag FrameUpdate reads while it sizes the scratch targets
	virtual void SetScratchTargetsForceRealloc( bool bForce ) = 0;
	// Named by what they do: the stats entry of the frame being built and the per-frame counters block
	virtual SceneSystemFrameStatsEntry_t *GetCurrentFrameStatsEntry() = 0;
	virtual int *GetFrameCounters() = 0;
	virtual void GetStatsSummaryString( CBufferString *pOut ) = 0;
	// Named by what it does: GetStatsSummaryString with a prefix in front of every line
	virtual void GetStatsSummaryStringWithPrefix( CBufferString *pOut, const char *pszPrefix ) = 0;
	virtual SceneSystemPerFrameStats_t &GetPerFrameStats() = 0;
	virtual int GetFinishRenderingViewsCounter() = 0;
	virtual int GetFrameUpdateCounter() = 0;

	// Binds the frame's transform buffer to nSlot from nFirstTransform on
	virtual void BindTransformSlot( IRenderContext *pRenderContext, int nSlot, int nFirstTransform, bool bUnused ) = 0;
	// Named by what it does: binds the whole transform buffer with a stride of 4
	virtual void BindTransformBuffer( IRenderContext *pRenderContext, int nSlot ) = 0;
	// MSVC orders the two in reverse
	virtual SceneSystemTransformEntry_t *AllocateFrameScopedTransforms( int nCount, int *pFirstIndex, SceneSystemTransformEntry_t **ppTransforms ) = 0;
	virtual SceneSystemTransformEntry_t *AllocateFrameScopedTransforms( int nAlignment, int nCount, int *pFirstIndex, SceneSystemTransformEntry_t **ppTransforms ) = 0;
	// Named by what they do: 8-byte and 4-byte entries from two more frame scoped stacks; pFirstIndex is -1 when full
	virtual void **AllocateFrameScopedPointers( int nCount, int *pFirstIndex ) = 0;
	virtual uint32 *AllocateFrameScopedIndices( int nCount, int *pFirstIndex ) = 0;
	// The index of the value in the extra shader data table, added when missing
	virtual uint32 GetExtraShaderData( const Vector4D &vValue ) = 0;
	// Named by what it does: a view outside the frame's list of views ("SceneView" when pszName is NULL)
	virtual ISceneView *CreateView( const char *pszName, ISceneWorld *pWorld, const uint32 *pVisBits ) = 0;
	// The view id of a player, -1 for views of no player: m_nViewId is the inverted CRC64 of nPlayerId
	virtual CViewId CreateViewId( int nPlayerId ) = 0;
	virtual void DeleteSceneObjectAtFrameEnd( CSceneObject *pObject ) = 0;
	virtual void GetAllActiveSceneWorlds( CUtlVector< ISceneWorld * > &outWorlds ) = 0;

	virtual int FindOrCreateSceneObjectClass( const char *pszName ) = 0;
	virtual uint8 SetObjectClass( CSceneObject *pObject, uint8 nClass ) = 0;
	// "<UNKNOWN ID: %d>" for an unregistered class
	virtual void GetObjectClassName( uint8 nClass, char *pszOut, int nOutSize ) = 0;

	virtual void ConstructSceneObject( CSceneObject *pObject ) = 0;
	// Called from the CSceneObject destructor.
	virtual void DestructSceneObject( CSceneObject *pObject ) = 0;
	virtual void SetSceneObjectTransformsFast( CSceneObject **ppObjects, int nCount, const FourVectors *pOrigins, const float *pScales, const FourVectors *pForward, const float *pUnused1, const float *pUnused2, const float *pUnused3, float flDefaultScale, const float *pRight, const float *pUp, const float *pTint ) = 0;

	virtual ISceneView *AddViewFromTemplate( const char *pszName, const char *pszTemplateName, const ISceneView *pParentView, const CFrustum &frustum, SwapChainHandle_t hSwapChain, ISceneWorld *pWorld, const RenderViewport_t &viewport, const uint32 *pVisBits, const CRenderAttributes *pAttributes ) = 0;
	virtual void InstallTemplateViewFactory( const char *pszName, SceneTemplateViewFactoryFunc_t *pFactory ) = 0;
	virtual void RemoveTemplateViewFactory( const char *pszName ) = 0;

	virtual void QueueSceneObjectUpdateJob( std::function< void() > &job ) = 0;
	virtual void ExecuteQueuedSceneObjectUpdateJobs() = 0;
	virtual void InstallMaterialDependentSceneObjectUpdater( const std::function< void( CSceneObject * ) > &updater ) = 0;

	// A listener added after the scene system is initialized is notified right away
	virtual void AddListener( ISceneSystemListener *pListener ) = 0;
	virtual void RemoveListener( ISceneSystemListener *pListener ) = 0;

	virtual void SetIgnoreAlphaFade( bool bIgnore ) = 0;
	virtual void SetupPerObjectLighting( CRenderAttributes &attributes, CSceneObject *pObject, const ISceneLayer *pLayer, bool bForceGPULightBinning ) = 0;

	// Sparse shadow trees. Named by what they do: destroy the four trees and their texture, then set them up for nMode; or only destroy them
	virtual void ReinitSparseShadowTrees( int nMode ) = 0;
	virtual void ShutdownSparseShadowTrees() = 0;
	virtual void LoadSparseShadowTreeManifest( const char *pszMap, const char *pszManifest, bool bSync ) = 0;
	virtual void UnloadSparseShadowTreeManifest() = 0;
	// NULL for an index over 3, or a tree that bRequireEnabled finds disabled
	virtual CSparseShadowTree *GetSparseShadowTree( uint32 nIndex, bool bRequireEnabled ) = 0;
	virtual CSparseShadowTree *CreateSparseShadowTree( const void *pCreateInfo, int nIndex ) = 0;
	virtual void DestroySparseShadowTree( int nIndex ) = 0;

	// Named by what it does: the material names of the object's extra data block
	virtual void GetSceneObjectMaterialNames( CUtlVector< CUtlString > &outNames, const CSceneObject *pObject ) = 0;
	// Named by what it does: sets a per-class byte flag
	virtual uint8 SetObjectClassFlags( uint8 nClass, bool bValue ) = 0;

	virtual void SceneObject_ScheduleDirtyUpdate( CSceneObject *pObject ) = 0;
	virtual void SceneObject_UnscheduleDirtyUpdate( CSceneObject *pObject ) = 0;
	// Allocates the extra per-object data block when the object has none
	virtual void SceneObject_AttachExtraData( CSceneObject *pObject ) = 0;
	virtual void SceneObject_AttachLightingCache( CSceneObject *pObject ) = 0;
	virtual void SceneObject_UpdateFlagsBasedOnMaterial( CSceneObject *pObject ) = 0;
	// Named by what they do: a stencil state kept in the scene system's table, referenced from the extra data
	virtual void SceneObject_SetStencilState( CSceneObject *pObject, const CSceneSystemStencilState *pState ) = 0;
	virtual const CSceneSystemStencilState *SceneObject_GetStencilState( const CSceneObject *pObject, CSceneSystemStencilState *pOut ) = 0;
	virtual void SceneObject_SetMaterialOverride( CSceneObject *pObject, IMaterial2 *pMaterial ) = 0;
	// Named by what it does: the material override of one layer, -1 for all of them
	virtual void SceneObject_SetLayerMaterialOverride( CSceneObject *pObject, int nLayer, IMaterial2 *pMaterial, bool bEnable ) = 0;
	// Named by what it does: bit N of nMask shows the object to player viewing index N (four of them)
	virtual void SceneObject_SetPlayerVisibilityMask( CSceneObject *pObject, int nMask ) = 0;
	virtual void SceneObject_AddChild( CSceneObject *pObject, CUtlStringToken token, CSceneObject *pChild, uint32 nFlags ) = 0;
	virtual void SceneObject_RemoveChild( CSceneObject *pObject, CSceneObject *pChild ) = 0;
	virtual CSceneObject *SceneObject_FindChild( const CSceneObject *pObject, CUtlStringToken token ) = 0;
	virtual void SceneObject_MirrorTransformToChildSceneObjects( const CSceneObject *pObject ) = 0;
	virtual void SceneObject_MirrorVisibilityToChildSceneObjects( const CSceneObject *pObject ) = 0;
	virtual void SceneObject_SetForceLayerID( CSceneObject *pObject, CUtlStringToken layerID ) = 0;
	virtual void SceneObject_PostFlagsUpdate( CSceneObject *pObject ) = 0;

	virtual void RegisterExtraDataAllocation( const char *pszName ) = 0;
	virtual float GetCurrentRenderTime() = 0;
	virtual void AdvanceNonRenderedFrame() = 0;
	// The settings byte of the object's class
	virtual uint8 GetObjectSettings( const CSceneObject *pObject ) = 0;

	// Named by what they do: the projection part of the view constants (jittered or not, by a scene system flag)
	// and the matching world to projection matrix
	virtual void FillOutViewProjectionConstants( PerViewConstantBuffer_t *pConstants, const CFrustum *pFrustum, const RenderTargetDesc_t &desc ) = 0;
	virtual VMatrix GetViewProjectionMatrix( const CFrustum *pFrustum ) = 0;
	virtual void FillOutViewConstants( PerViewConstantBuffer_t *pConstants, const CFrustum *pFrustum, const RenderViewport_t &viewport, const RenderTargetDesc_t &desc, float flTime, float flFrameTime, const Vector4D &vClipPlane ) = 0;

	// Gates ISceneUtils::CreateVolumetricFog.
	virtual bool VolumetricFogEnabled() = 0;
	// Gates ISceneUtils::CreateWindController.
	virtual bool WindEnabled() = 0;
	virtual bool NonTexturedGradientFogEnabled() = 0;
	virtual bool CubemapFogEnabled() = 0;
	// Gates ISceneUtils::CreateCharacterDecalRenderer.
	virtual bool CharacterDecalRenderingEnabled() = 0;

	// Named by what they do: a list of (world, view) pairs; add returns the existing entry when there is one
	virtual void *AddWorldViewPair( ISceneWorld *pWorld, ISceneView *pView ) = 0;
	virtual void RemoveWorldViewPair( ISceneWorld *pWorld, ISceneView *pView ) = 0;
	virtual ISceneView *FindView( const char *pszName ) = 0;
	virtual int GetPrimaryRenderTargetFormat() = 0;
	virtual int GetBackBufferFormat() = 0;

	virtual CSceneObject *CreateAOProxySceneObject( uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;
	// Reads ao_proxy_capsule_list and ao_proxy_box_list.
	virtual bool LoadAOProxies( const void *pModelKeyValues, int *pCapsuleCount, int *pBoxCount, uint32 *pBoneIndices ) = 0;
	virtual void UpdateAOProxySceneObject( CSceneObject *pObject, const matrix3x4_t *pBoneTransforms, const int *pCapsules, const int *pBoxes, int nBoneCount ) = 0;

	// A box of the world that culls (or keeps) objects; returns its id
	virtual int AddCullingBox( ISceneWorld *pWorld, int nMode, const Vector &vecOrigin, const QAngle &angAngles, const Vector &vecExtents ) = 0;
	virtual void RemoveCullingBox( ISceneWorld *pWorld, int nBox ) = 0;
	virtual bool SceneObject_GetMaterialsForObject( const CSceneObject *pObject, CUtlVector< HMaterialWeak > &outMaterials, CUtlVector< int > &outMaterialLayers ) = 0;

	// Named by what they do: two lists of listeners, one called back for light objects, one for every object the
	// scene system really deletes
	virtual void AddLightListener( ISceneSystemLightListener *pListener ) = 0;
	virtual void RemoveLightListener( ISceneSystemLightListener *pListener ) = 0;
	virtual void AddObjectDeletionListener( ISceneObjectDeletionListener *pListener ) = 0;
	virtual void RemoveObjectDeletionListener( ISceneObjectDeletionListener *pListener ) = 0;

	// Global render states keyed by token. Named by what the others do: the count of states
	// changed since they were last applied, a token to bool table, a pending flag and the apply step
	virtual void SetIntState( CUtlStringToken token, int nValue ) = 0;
	virtual int GetPendingStateCount() = 0;
	virtual void SetBoolState( CUtlStringToken token, bool bValue ) = 0;
	virtual bool IsStateUpdatePending() = 0;
	virtual void ApplyPendingStates() = 0;
	// Named by what it does: the highest value the persistent debug overlay lists report
	virtual void GetPersistentDebugOverlayTiming( int *pOut ) = 0;

	virtual IRayTraceSceneWorld *CreateRayTraceWorld( const char *pszDebugName, int nMaxRayTypes, bool bAllowCompaction ) = 0;
	virtual void DestroyRayTraceWorld( IRayTraceSceneWorld *pWorld ) = 0;

	// Named by what they do: adds the object of an entry (when its flag at +16 is set) to a hashed set; runs the
	// deferred object updates through their object types until one is not done; adds an object to a locked list
	virtual void AddTrackedObject( const SceneObjectTrackEntry_t &entry ) = 0;
	virtual void ProcessDeferredObjectUpdates( IRenderContext *pRenderContext ) = 0;
	virtual void AddObjectToLockedList( CSceneObject *pObject ) = 0;

	// Distance field scene. Named by what they do: register an object, the instance count, unregister, and
	// queries of an instance by index
	virtual int AddDistanceFieldObject( CSceneObject **ppObject ) = 0;
	virtual int GetDistanceFieldInstanceCount() = 0;
	virtual void RemoveDistanceFieldObject( CSceneObject **ppObject ) = 0;
	virtual CSceneObject *GetDistanceFieldInstanceObject( int nIndex ) = 0;
	virtual void GetDistanceFieldInstanceAtlasRange( int nIndex, int *pFirst, int *pCount ) = 0;
	virtual bool IsDistanceFieldInstanceResident( int nIndex ) = 0;
	virtual bool IsDistanceFieldInstanceDynamic( int nIndex ) = 0;
	virtual int UpdateDistanceFieldObject( CSceneObject *pObject ) = 0;
	virtual CSceneObject *CreateDistanceFieldInstanceSceneObject( uint64 nFlags, uint64 nTypeFlags, ISceneWorld *pWorld ) = 0;
	virtual CDistanceFieldScene *GetDistanceFieldScene() = 0;

	// Named by what they do: the hair renderer's update, a texture both render paths share (set only while it is
	// enabled) and a table of per-index renderers
	virtual void UpdateHairRendering() = 0;
	virtual void SetSharedSceneTexture( HRenderTexture hTexture ) = 0;
	virtual void *GetPerIndexRenderer( int nIndex ) = 0;

	virtual void AcquireImGuiLock() = 0;
	// bRelease frees the lock when the outermost holder releases it
	virtual void ReleaseImGuiLock( bool bRelease ) = 0;
	virtual bool IsImGuiLocked() = 0;
	virtual bool IsImGuiLockedByCurrentThread() = 0;

	// Named by what they do: a list of std::function updaters (like InstallMaterialDependentSceneObjectUpdater),
	// the view issue counter, and the views of the frame waiting for a late submit
	virtual void InstallSceneObjectUpdater( const std::function< void( CSceneObject * ) > &updater ) = 0;
	virtual int GetViewIssueCounter() = 0;
	virtual void AddLateSubmitView( ISceneView *pView ) = 0;
};

#endif // ISCENESYSTEM_H
