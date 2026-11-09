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

class InfoForResourceTypeCTextureBase;
template < class T > class CWeakHandle;
using HRenderTexture = CWeakHandle< InfoForResourceTypeCTextureBase >;

FORWARD_DECLARE_HANDLE( SwapChainHandle_t );

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

	// A null or empty pszName defaults to "GpuCull", "GpuCull-PrepareReculling" or "GpuRecull" for nCullPass 0, 1 or 2
	virtual ISceneLayer *Unk_AddGpuCullLayer( const char *pszName, const RenderViewport_t &viewport, uint8 nCullPass ) = 0;

	// An empty layer other layers are ordered against; named "label: <pszLabel>"
	virtual ISceneLayer *AddLabelLayer( const char *pszLabel ) = 0;
	virtual bool HasLabelLayer( CUtlStringToken label ) = 0;

	virtual void MoveLayerToBeAfterLayer( ISceneLayer *pLayer, ISceneLayer *pAfterLayer ) = 0;

	// The layer waits on pSemaphore before it renders
	virtual void Unk_AddLabelLayerSemaphore( CUtlStringToken label, void *pSemaphore ) = 0;
	virtual void Unk_AddLayerSemaphore( ISceneLayer *pLayer, void *pSemaphore ) = 0;

	// Appends nUnk to a byte list of pLayer when a global per-value table enables it
	virtual void unk010( uint8 nUnk, ISceneLayer *pLayer ) = 0;

	// Draws pMaterial full screen into outputs
	virtual ISceneLayer *AddSimplePostProcessingLayer( const char *pszName, const RenderViewport_t &viewport, const IMaterial2 *pMaterial, const SceneLayerOutputs_t &outputs, bool bUnk ) = 0;

	// Same as AddSimplePostProcessingLayer, but pUnk is handed to the new layer instead of the outputs.
	// Both platforms keep this slot after the previous one, so the two are not overloads of one name
	virtual ISceneLayer *Unk_AddSimplePostProcessingLayer( const char *pszName, const RenderViewport_t &viewport, const IMaterial2 *pMaterial, const void *pUnk, bool bUnk ) = 0;

	// Draws the srcRect texels of a nTextureWidth x nTextureHeight texture into dstRect with a material
	virtual ISceneLayer *Unk_AddSimpleQuadLayer( const char *pszName, const RenderViewport_t &viewport, const void *pUnk, const void *hMaterial, int nTextureWidth, int nTextureHeight, const RenderViewport_t &srcRect, const RenderViewport_t &dstRect ) = 0;

	// Resolves color target nColorTargetIndex of the previous layer into hTarget; fails as the first layer
	virtual ISceneLayer *AddResolveRenderTargetLayer( const char *pszName, const RenderViewport_t &viewport, int nUnk1, int nUnk2, int nColorTargetIndex, HSceneViewRenderTarget hTarget ) = 0;

	// Resolves the depth target of the previous layer into hTarget; fails as the first layer
	virtual ISceneLayer *AddResolveDepthStencilLayer( const char *pszName, const RenderViewport_t &viewport, int nUnk1, int nUnk2, HSceneViewRenderTarget hTarget ) = 0;

	virtual void AddWorldToRenderList( ISceneWorld *pWorld, const uint32 *pVisBits, bool bUnk ) = 0;
	virtual void AddSceneObjectToRenderList( CSceneObject *pObject ) = 0;

	virtual SwapChainHandle_t GetSwapChain() = 0;

	virtual ~ISceneView() {}

	virtual ISceneLayer *AddDebugInfoLayer( const char *pszName, const RenderViewport_t &viewport, uint32 nUnk ) = 0;

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
	virtual const SceneFogParams_t &Unk_GetFogstate() = 0;

	// Forwards to SetPlayerViewingIndex
	virtual void SetSplitscreenView( int nSlot ) = 0;

	// Copied into every layer added afterwards
	virtual void SetDefaultLayerObjectRequiredFlags( uint64 nFlags ) = 0;
	virtual void SetDefaultLayerObjectExcludedFlags( uint64 nFlags ) = 0;
	virtual uint64 GetDefaultLayerObjectRequiredFlags() = 0;
	virtual uint64 GetDefaultLayerObjectExcludedFlags() = 0;

	// A second set/set/get/get pair of 64-bit layer defaults, copied into every layer added afterwards
	virtual void unk035( uint64 nUnk ) = 0;
	virtual void unk036( uint64 nUnk ) = 0;
	virtual uint64 unk037() = 0;
	virtual uint64 unk038() = 0;

	// Get and set of a 16-byte value passed by value (two registers on Linux), copied into every layer added afterwards
	virtual void unk039() = 0;
	virtual void unk040() = 0;

	virtual ISceneLayer *GetFinalOutputLayer() = 0;
	virtual void GetRenderTargetInfo( HSceneViewRenderTarget hTarget, SceneViewRenderTargetInfo_t *pInfo ) = 0;

	// Copies srcViewport of hSource into dstViewport of outputs
	virtual ISceneLayer *AddBlitTextureToTargetLayer( const char *pszName, HSceneViewRenderTarget hSource, const RenderViewport_t &srcViewport, const RenderViewport_t &dstViewport, const SceneLayerOutputs_t &outputs, Vector4D *pUnk1, bool bUnk, RsBlendStateDesc_t *pBlendState, Vector4D *pUnk2 ) = 0;

	virtual void MarkRenderTargetRead( HSceneViewRenderTarget hTarget ) = 0;
	virtual HRenderTexture GetRenderTargetTexture( HSceneViewRenderTarget hTarget ) = 0;

	// Only plane 0 is stored
	virtual void SetUserClipPlane( int nIndex, const VPlane &plane ) = 0;

	virtual bool unk047() = 0;
	virtual void unk048( bool bUnk ) = 0;

	virtual void *unk049() = 0;
	virtual void unk050( void *pUnk ) = 0;

	// Created on first use
	virtual ISceneViewDebugOverlays *GetDebugOverlayList() = 0;

	// Shares the debug overlay list of pOtherView; true if there is one
	virtual bool Unk_ShareDebugOverlayList( ISceneView *pOtherView ) = 0;

	virtual const CRenderAttributes &GetRenderAttributes() const = 0;
	virtual CRenderAttributes &GetRenderAttributes() = 0;

	virtual void SetDynamicViewTemplate( CUtlStringToken name, ISceneView *pTemplate ) = 0;

	// Walks up the parent views until one has the template
	virtual ISceneView *GetDynamicViewTemplate( CUtlStringToken name ) = 0;

	virtual bool IsTemplate() = 0;

	// Clones the view and its layers as a child of pParentView, taking its view id
	virtual ISceneView *Duplicate( ISceneView *pParentView ) = 0;

	virtual CViewId GetViewId() = 0;

	// The index of the view among the views of the frame; -1 for dynamic views
	virtual int unk060() = 0;
	virtual int unk061() = 0;

	virtual void ComputeRenderTargetAllocations() = 0;

	// Both values are percentages; the second one is clamped to [ 0, 100 ]
	virtual void SetBoundingVolumeSizeCullThresholdInPercent( float flThreshold, float flUnk ) = 0;
	virtual void unk064( float flUnk1, float flUnk2 ) = 0;

	virtual void SetDistanceScaleForLODAndCulling( float flScale ) = 0;
	virtual float GetSquaredDistanceScaleForLODAndCulling() = 0;

	virtual void unk067( float flUnk ) = 0;

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

	virtual bool unk079() = 0;
	virtual void unk080( bool bUnk ) = 0;

	// Set from a counter of the scene system when the view is added
	virtual void SetIssueSortIndex( int nIndex ) = 0;

	// Clears what AddWorldToRenderList copied from the worlds when bUnk is set
	virtual void unk082() = 0;

	// Copies a 0x180-byte (Windows) / 0x178-byte (Linux) block into the view
	virtual void unk083( const void *pUnk ) = 0;

	// Registers a render target of the view under pszName. MSVC orders the four overloads in reverse
	virtual HSceneViewRenderTarget FindOrCreateRenderTarget( const char *pszName, const SceneViewRenderTargetInfo_t &info, uint8 nFlags ) = 0;
	virtual HSceneViewRenderTarget FindOrCreateRenderTarget( const char *pszName, HRenderTexture hTexture, uint8 nFlags ) = 0;
	virtual HSceneViewRenderTarget FindOrCreateRenderTarget( const char *pszName, SwapChainHandle_t hSwapChain, int nSwapChainBuffer, uint8 nFlags ) = 0;
	virtual HSceneViewRenderTarget FindOrCreateRenderTarget( const char *pszName, ISceneView *pOtherView, HSceneViewRenderTarget hOtherViewTarget ) = 0;

	// Allocates a scratch render target; nine arguments
	virtual void unk088() = 0;

	virtual void Unk_AddRefRenderTarget( HSceneViewRenderTarget hTarget ) = 0;
	virtual void Unk_ReleaseRenderTarget( HSceneViewRenderTarget hTarget ) = 0;

	// If hTarget is multisampled, adds a layer (pszLayerName, which defaults to the function name) that resolves it
	// into a new "resolved_rt" and returns that; otherwise returns hTarget
	virtual HSceneViewRenderTarget ResolveMSAARenderTargetIfNecessary( const RenderViewport_t &viewport, HSceneViewRenderTarget hTarget, const char *pszLayerName, ISceneLayer *pUnkLayer ) = 0;

	// Adds an "MSAAResolve" layer into a new "resolved_rt" if hTarget is multisampled, else a "CopyRT" blit into a new "copy_rt"
	virtual HSceneViewRenderTarget Unk_ResolveOrCopyRenderTarget( const RenderViewport_t &viewport, HSceneViewRenderTarget hTarget ) = 0;

	// Looks up the render target of hTexture; pMultisampleType gets its multisample type
	virtual bool unk093( HRenderTexture hTexture, int *pIndex, uint8 *pMultisampleType ) = 0;

	// The reference count of the render target of hTexture
	virtual int unk094( HRenderTexture hTexture ) = 0;

	// The layer render layers added afterwards are linked to
	virtual void unk095( ISceneLayer *pLayer ) = 0;
	virtual ISceneLayer *unk096() = 0;

	// Empty
	virtual void unk097() = 0;

	virtual void *unk098() = 0;

	// Two std::shared_ptr members: set (by const reference), set (by value), get (by value)
	virtual void unk099() = 0;
	virtual void unk100() = 0;
	virtual void unk101() = 0;

	virtual void unk102( void *pUnk ) = 0;

	virtual void unk103( bool bUnk ) = 0;
	virtual bool unk104() = 0;

	virtual bool Unk_HasDebugOverlayList() = 0;

	// Set and get of a 4-byte value; the getter returns a class type (hidden return pointer on MSVC)
	virtual void unk106( int nUnk ) = 0;
	virtual void unk107() = 0;

	virtual void unk108( bool bUnk ) = 0;
	virtual bool unk109() = 0;
	virtual void unk110( bool bUnk ) = 0;
	virtual bool unk111() = 0;
};

#endif // ISCENEVIEW_H
