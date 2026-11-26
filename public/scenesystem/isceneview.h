//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Scene views and the layers they render
//
//===========================================================================//

#ifndef ISCENEVIEW_H
#define ISCENEVIEW_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector4d.h"
#include "rendersystem/rendersystemtypes.h"
#include "scenesystem/iscenelayer.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcesystem/resourcehandletypes.h"
#include "tier0/utlvector.h"

#include <memory>

class ISceneView;
class ISceneLayer;
class ISceneWorld;
class ISceneViewDebugOverlays;
class IRenderContext;
class IMaterial2;
class CSceneObject;
class CRenderAttributes;
class CSecondaryViewRenderInformation;
class CUtlStringToken;
class VPlane;
struct SceneViewRenderTargetInfo_t;
struct TonemapState_t;
struct SceneFogParams_t;
struct RsBlendStateDesc_t;
struct RenderTargetDesc_t;
struct SceneVolumetricFogVolume_t;
class CVolumeCuller;
class CFrustum;


FORWARD_DECLARE_HANDLE( SwapChainHandle_t );

// Returned through a hidden pointer on Windows because of the constructor, in a register on Linux
struct SceneViewUserValue_t
{
	SceneViewUserValue_t() : m_nValue( 0 ) {}

	int m_nValue;
};

// Identifies a view across frames
struct CViewId
{
	uint64 m_nViewId;
	int m_nPlayerId;
};

abstract_class ISceneView
{
public:
	// The name the view was added with
	virtual const char *GetDebugName() = 0;

	// pLayerMode is a string token followed by its CBufferString (the mode name, "Default" for most layers)
	virtual ISceneLayer *AddRenderLayer( const char *pszName, const RenderViewport_t &viewport, const void *pLayerMode ) = 0;

	// A layer drawn by pRenderer instead of from scene objects
	virtual ISceneLayer *AddProceduralLayer( const char *pszName, const RenderViewport_t &viewport, IProceduralLayerRenderer *pRenderer, bool bDeleteRenderProcedureWhenFinished ) = 0;

	virtual ISceneLayer *AddClearLayer( const char *pszName, const RenderViewport_t &viewport, int nClearFlags ) = 0;

	// Named by what it does. A null or empty pszName defaults to "GpuCull", "GpuCull-PrepareReculling" or "GpuRecull"
	// for nCullPass 0, 1 or 2
	virtual ISceneLayer *AddGpuCullLayer( const char *pszName, const RenderViewport_t &viewport, uint8 nCullPass ) = 0;

	// An empty layer other layers are ordered against; named "label: <pszLabel>"
	virtual ISceneLayer *AddLabelLayer( const char *pszLabel ) = 0;
	virtual bool HasLabelLayer( CUtlStringToken label ) = 0;

	virtual void MoveLayerToBeAfterLayer( ISceneLayer *pLayer, ISceneLayer *pAfterLayer ) = 0;

	// pLayer renders only after the label layer (or pDependency) has
	virtual void AddSemaphoreDependencyForLabelLayer( CUtlStringToken label, ISceneLayer *pLayer ) = 0;
	virtual void AddSemaphoreDependency( ISceneLayer *pLayer, ISceneLayer *pDependency ) = 0;

	// Named by what it does: appends nPass to the pass list of pLayer when the global per-pass table enables it
	virtual void AddLayerToPass( uint8 nPass, ISceneLayer *pLayer ) = 0;

	// Draws pMaterial full screen into the target desc or the outputs; bRendererFlag is stored in the layer renderer.
	// MSVC orders the two overloads in reverse
	virtual ISceneLayer *AddSimplePostProcessingLayer( const char *pszName, const RenderViewport_t &viewport, const IMaterial2 *pMaterial, const RenderTargetDesc_t &desc, bool bRendererFlag ) = 0;
	virtual ISceneLayer *AddSimplePostProcessingLayer( const char *pszName, const RenderViewport_t &viewport, const IMaterial2 *pMaterial, const SceneLayerOutputs_t &outputs, bool bRendererFlag ) = 0;

	// Draws the srcRect texels of a nTextureWidth x nTextureHeight texture into dstRect with shader mode shaderMode
	virtual ISceneLayer *AddSimpleQuadRendererLayer( const char *pszName, const RenderViewport_t &viewport, const CUtlStringToken &shaderMode, ResourceHandle_t hTexture, int nTextureWidth, int nTextureHeight, const RenderViewport_t &srcRect, const RenderViewport_t &dstRect ) = 0;

	// Resolves color target nColorTargetIndex of the previous layer into hTarget; fails as the first layer.
	// The game does not read nUnusedX and nUnusedY
	virtual ISceneLayer *AddResolveRenderTargetLayer( const char *pszName, const RenderViewport_t &viewport, int nUnusedX, int nUnusedY, int nColorTargetIndex, HSceneViewRenderTarget hTarget ) = 0;

	// Resolves the depth target of the previous layer into hTarget; fails as the first layer
	virtual ISceneLayer *AddResolveDepthStencilLayer( const char *pszName, const RenderViewport_t &viewport, int nUnusedX, int nUnusedY, HSceneViewRenderTarget hTarget ) = 0;

	// bCopyWorldVolumes copies the culling boxes and fog volumes of the world into the view (RemoveCullBoxes clears them)
	virtual void AddWorldToRenderList( ISceneWorld *pWorld, const uint32 *pVisBits, bool bCopyWorldVolumes ) = 0;
	virtual void AddSceneObjectToRenderList( CSceneObject *pObject ) = 0;

	virtual SwapChainHandle_t GetSwapChain() = 0;

	virtual ~ISceneView() {}

	virtual ISceneLayer *AddDebugInfoLayer( const char *pszName, const RenderViewport_t &viewport, uint32 nDebugInfoType ) = 0;

	// Render target names are looked up inside the innermost scope (up to 4 deep)
	virtual void PushNamedRenderTargetScope( const char *pszScope ) = 0;
	virtual void PopNamedRenderTargetScope() = 0;

	// SCENE_VIEW_RENDER_TARGET_INVALID if no target of that name exists
	virtual HSceneViewRenderTarget FindRenderTarget( const char *pszName ) = 0;

	virtual void AddFlags( uint32 nFlags ) = 0;
	virtual uint32 GetFlags() = 0;

	virtual const TonemapState_t &GetTonemapState() = 0;
	virtual void SetTonemapState( const TonemapState_t &state ) = 0;
	virtual void SetFogstate( const SceneFogParams_t &params ) = 0;
	virtual const SceneFogParams_t &GetFogstate() = 0;

	// Forwards to SetPlayerViewingIndex
	virtual void SetSplitscreenView( int nSlot ) = 0;

	// Copied into every layer added afterwards
	virtual void SetDefaultLayerObjectRequiredFlags( uint64 nFlags ) = 0;
	virtual void SetDefaultLayerObjectExcludedFlags( uint64 nFlags ) = 0;
	virtual uint64 GetDefaultLayerObjectRequiredFlags() = 0;
	virtual uint64 GetDefaultLayerObjectExcludedFlags() = 0;

	// Named by what they do: the type flags half of the defaults (CS2 objects have flags and type flags), copied
	// into every layer added afterwards
	virtual void SetDefaultLayerObjectRequiredTypeFlags( uint64 nFlags ) = 0;
	virtual void SetDefaultLayerObjectExcludedTypeFlags( uint64 nFlags ) = 0;
	virtual uint64 GetDefaultLayerObjectRequiredTypeFlags() = 0;
	virtual uint64 GetDefaultLayerObjectExcludedTypeFlags() = 0;

	// Named by what they do: a 128-bit mask copied into every layer added afterwards, next to its flag masks
	virtual SceneObjectClassMask_t GetDefaultLayerObjectClassMask() = 0;
	virtual void SetDefaultLayerObjectClassMask( SceneObjectClassMask_t mask ) = 0;

	virtual ISceneLayer *GetFinalOutputLayer() = 0;
	virtual void GetRenderTargetInfo( HSceneViewRenderTarget hTarget, SceneViewRenderTargetInfo_t *pInfo ) = 0;

	// Copies srcViewport of hSource into dstViewport of outputs
	virtual ISceneLayer *AddBlitTextureToTargetLayer( const char *pszName, HSceneViewRenderTarget hSource, const RenderViewport_t &srcViewport, const RenderViewport_t &dstViewport, const SceneLayerOutputs_t &outputs, Vector4D *pColorScale, bool bPointSample, RsBlendStateDesc_t *pBlendState, Vector4D *pColorBias ) = 0;

	virtual void MarkRenderTargetRead( HSceneViewRenderTarget hTarget ) = 0;
	virtual HRenderTexture GetRenderTargetTexture( HSceneViewRenderTarget hTarget ) = 0;

	// Only plane 0 is stored
	virtual void SetUserClipPlane( int nIndex, const VPlane &plane ) = 0;

	// The scene system only stores the flag
	virtual bool GetShadowsEnabled() = 0;
	virtual void SetShadowsEnabled( bool bEnabled ) = 0;

	virtual void *GetUserDataPtr() = 0;
	virtual void SetUserDataPtr( void *pData ) = 0;

	// Created on first use
	virtual ISceneViewDebugOverlays *GetDebugOverlayList() = 0;

	// Shares the debug overlay list of pOtherView; true if there is one
	virtual bool ShareDebugOverlayList( ISceneView *pOtherView ) = 0;

	virtual const CRenderAttributes &GetRenderAttributes() const = 0;
	virtual CRenderAttributes &GetRenderAttributes() = 0;

	virtual void SetDynamicViewTemplate( CUtlStringToken name, ISceneView *pTemplate ) = 0;

	// Walks up the parent views until one has the template
	virtual ISceneView *GetDynamicViewTemplate( CUtlStringToken name ) = 0;

	virtual bool IsTemplate() = 0;

	// Clones the view and its layers as a child of pParentView, taking its view id
	virtual ISceneView *Duplicate( ISceneView *pParentView ) = 0;

	virtual CViewId GetViewId() = 0;

	// Named by what they do: the index of the view among the views of the frame (-1 for dynamic views), and a second
	// 16-bit index that the light and view constants setup reads
	virtual int GetFrameViewIndex() = 0;
	virtual int GetViewConstantsIndex() = 0;

	virtual void ComputeRenderTargetAllocations() = 0;

	// Both values are percentages; the second one is clamped to [ 0, 100 ]
	virtual void SetBoundingVolumeSizeCullThresholdInPercent( float flThreshold, float flMaxThreshold ) = 0;
	// Named by what it does: two thresholds the GPU culling and the instanced mesh culling read
	virtual void SetGpuCullSizeThresholds( float flThreshold, float flMaxThreshold ) = 0;

	virtual void SetDistanceScaleForLODAndCulling( float flScale ) = 0;
	virtual float GetSquaredDistanceScaleForLODAndCulling() = 0;

	// Named by what it does: a distance scale the instanced mesh culling reads
	virtual void SetInstancedMeshDistanceScale( float flScale ) = 0;

	// -1 selects index 3
	virtual void SetPlayerViewingIndex( int nIndex ) = 0;

	virtual int GetPriority() = 0;
	virtual void SetPriority( int nPriority ) = 0;

	virtual void SetParent( ISceneView *pParent ) = 0;
	virtual ISceneView *GetParent() = 0;

	virtual void CopyWorldsAndVisFrom( ISceneView *pOtherView, const Vector &vecOrigin, int nVisType ) = 0;
	virtual void CopyWorldsFrom( ISceneView *pOtherView ) = 0;

	virtual void AddDependentView( ISceneView *pView, int nType ) = 0;
	virtual const CSecondaryViewRenderInformation *GetSecondaryViewRenderInfo( int nIndex ) = 0;

	virtual void SetOverrideLODLevel( int nLevel ) = 0;
	virtual int GetOverrideLODLevel() = 0;

	// The scene system only stores the flag
	virtual bool GetPostProcessEnabled() = 0;
	virtual void SetPostProcessEnabled( bool bEnabled ) = 0;

	// Set from a counter of the scene system when the view is added
	virtual void SetIssueSortIndex( int nIndex ) = 0;

	// Clears the culling boxes AddWorldToRenderList copied from the worlds
	virtual void RemoveCullBoxes() = 0;

	// Copied into the view (0x180 bytes on Windows, 0x178 on Linux)
	virtual void SetVolumeCuller( const CVolumeCuller &culler ) = 0;

	// Registers a render target of the view under pszName. MSVC orders the four overloads in reverse
	virtual HSceneViewRenderTarget FindOrCreateRenderTarget( const char *pszName, const SceneViewRenderTargetInfo_t &info, uint8 nFlags ) = 0;
	virtual HSceneViewRenderTarget FindOrCreateRenderTarget( const char *pszName, HRenderTexture hTexture, uint8 nFlags ) = 0;
	virtual HSceneViewRenderTarget FindOrCreateRenderTarget( const char *pszName, SwapChainHandle_t hSwapChain, int nSwapChainBuffer, uint8 nFlags ) = 0;
	virtual HSceneViewRenderTarget FindOrCreateRenderTarget( const char *pszName, ISceneView *pOtherView, HSceneViewRenderTarget hOtherViewTarget ) = 0;

	// Named by what it does: adds a scratch render target to the render target list of the view. The argument names
	// are not traced beyond their types
	virtual HSceneViewRenderTarget AllocateScratchRenderTarget( int nWidth, int nHeight, int nFormat, const SceneViewRenderTargetInfo_t *pInfo, const char *pszName, uint32 nFlags, uint32 nUsage, bool bPersistent, uint32 nArraySize ) = 0;

	virtual void AddRefRenderTarget( HSceneViewRenderTarget hTarget ) = 0;
	virtual void ReleaseRenderTarget( HSceneViewRenderTarget hTarget ) = 0;

	// If hTarget is multisampled, adds a layer (pszLayerName, which defaults to the function name) that resolves it
	// into a new "resolved_rt" and returns that; otherwise returns hTarget
	virtual HSceneViewRenderTarget ResolveMSAARenderTargetIfNecessary( const RenderViewport_t &viewport, HSceneViewRenderTarget hTarget, const char *pszLayerName, ISceneLayer *pAssociatedLayer ) = 0;

	// Named by what it does: adds an "MSAAResolve" layer into a new "resolved_rt" if hTarget is multisampled, else a
	// "CopyRT" blit into a new "copy_rt"
	virtual HSceneViewRenderTarget ResolveOrCopyRenderTarget( const RenderViewport_t &viewport, HSceneViewRenderTarget hTarget ) = 0;

	// Named by what they do: the render target of hTexture (pMultisampleType gets its multisample type) and its
	// reference count
	virtual bool FindRenderTargetForTexture( HRenderTexture hTexture, int *pIndex, uint8 *pMultisampleType ) = 0;
	virtual int GetRenderTargetRefCount( HRenderTexture hTexture ) = 0;

	// Named by what they do: the layer that render layers added afterwards are associated with
	virtual void SetAssociatedLayerForNewLayers( ISceneLayer *pLayer ) = 0;
	virtual ISceneLayer *GetAssociatedLayerForNewLayers() = 0;

	// Empty in the game
	virtual void CaptureRenderDocForView() = 0;

	// The fog volumes AddWorldToRenderList copied from the worlds
	virtual const CUtlVector< SceneVolumetricFogVolume_t > &GetVolumetricFogVolumes() = 0;

	// Named by what they do: two std::shared_ptr members the scene system only stores
	virtual void SetSharedUserData( const std::shared_ptr< void > &pData ) = 0;
	virtual void SetSharedUserData2( std::shared_ptr< void > pData ) = 0;
	virtual std::shared_ptr< void > GetSharedUserData2() = 0;

	// Appends the frustum to the auxiliary cameras of the view
	virtual void AddAuxiliaryRenderCamera( const CFrustum &frustum ) = 0;

	// Named by what they do: values the scene system only stores (the client sets and reads them)
	virtual void SetUserFlag( bool bValue ) = 0;
	virtual bool GetUserFlag() = 0;

	virtual bool HasDebugOverlayList() = 0;

	virtual void SetUserValue( int nValue ) = 0;
	virtual SceneViewUserValue_t GetUserValue() = 0;

	virtual void SetUserFlag2( bool bValue ) = 0;
	virtual bool GetUserFlag2() = 0;
	virtual void SetUserFlag3( bool bValue ) = 0;
	virtual bool GetUserFlag3() = 0;
};

#endif // ISCENEVIEW_H
