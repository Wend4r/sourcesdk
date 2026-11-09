//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Scene layers and the renderers of procedural layers
//
//===========================================================================//

#ifndef ISCENELAYER_H
#define ISCENELAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/basetypes.h"
#include "tier0/bufferstring.h"
#include "tier0/utlstringtoken.h"
#include "mathlib/vector4d.h"
#include "rendersystem/rendersystemtypes.h"

class ISceneView;
class ISceneLayer;
class IRenderContext;
class CFrustum;
struct RenderTargetDesc_t;
struct SceneSystemPerFrameStats_t;

FORWARD_DECLARE_HANDLE( HSceneViewRenderTarget );
#define SCENE_VIEW_RENDER_TARGET_INVALID ( (HSceneViewRenderTarget)-1 )

// Computes the sort keys of the layer's draw items before they are sorted. The items are records of the
// scene system's own type.
typedef void LayerComputeSortKeyFunc_t( void *pDrawItems, uint32 nCount );

// What a layer renders into
struct SceneLayerOutputs_t
{
	int m_nColorTargetCount;
	HSceneViewRenderTarget m_pColorTargets[ 8 ];
	uint8 m_allowSrgbWrites;
	HSceneViewRenderTarget m_hDepthTarget;
	int m_nColorTargetIndices[ 8 ];
	int m_nDepthTargetIndex;
};

// Draws a procedural layer; called while the view renders
abstract_class IProceduralLayerRenderer
{
public:
	// pStats receives the draw calls and primitives the renderer issues
	virtual void Render( ISceneView *pView, IRenderContext *pRenderContext, ISceneLayer *pLayer, SceneSystemPerFrameStats_t *pStats ) = 0;

	virtual ~IProceduralLayerRenderer() {}

	// Picks the type of the layer AddProceduralLayer creates (true selects type 2 instead of 1); every
	// scene system renderer returns false
	virtual bool unk002() = 0;
};

// Declared up to the last field and slot in use.
abstract_class ISceneLayer
{
public:
	virtual const RenderTargetDesc_t &GetRenderTargetDesc() = 0;
	virtual void SetRenderTargetDesc( const RenderTargetDesc_t &desc ) = 0;

	// The per-frame stats of this layer alone
	virtual SceneSystemPerFrameStats_t &GetPerFrameStats() = 0;

	virtual void DisableReflectionsAndRefractionsForObjectsInThisLayer( bool bDisable ) = 0;

	// Keeps a copy of the frustum
	virtual void OverrideFrustum( const CFrustum &frustum ) = 0;

	// Adds pLayer to the layers that feed this one, as SetFadeTargetLayer does for its target
	virtual void unk005( ISceneLayer *pLayer ) = 0;

	// The layer that receives the LOD-fading objects of this one
	virtual void SetFadeTargetLayer( ISceneLayer *pLayer ) = 0;
	virtual ISceneLayer *GetFadeTargetLayer() = 0;

	// Returns the stencil bit index of the token, or -1 when all 8 bits are taken
	virtual int FindOrAllocateStencilBit( CUtlStringToken token ) = 0;

	// Accepts "#<bit>" or a stencil bit name
	virtual bool ParseStringForStencilMaskAndRefValueForRead( const char *pszString, int *pMask, int *pRefValue ) = 0;

	// -1 disables the override
	virtual void SetTonemapOverrideScaleValue( float flScale ) = 0;

	virtual void SetCustomSortKeyFunction( LayerComputeSortKeyFunc_t *pFunc ) = 0;

	virtual ~ISceneLayer() {}

	// Enables a culling plane (normal, distance) for the objects of this layer
	virtual void unk013( const Vector4D &vPlane ) = 0;

	// Kept only while the scene system's debug tracking is on
	virtual void unk014( const char *psz ) = 0;

	// -1 falls back to sc_max_framebuffer_copies_per_layer
	virtual void SetMaxFramebufferCopies( int nMaxCopies ) = 0;

public:
#ifdef _WIN32
	// MSVC aligns the first member past the vtable pointer to the 16-byte alignment of the class
	uint8 m_pad008[ 8 ];
#endif

	RenderViewport_t m_viewport;
	int m_eLayerType;
	CUtlStringToken m_shaderMode;

	// The name m_shaderMode is the token of
	CBufferString m_shaderModeName;

	uint64 m_nLayerFlags;
	uint32 m_nClearFlags;
	SceneLayerOutputs_t m_RenderTargetOutputs;

#ifdef _WIN32
	// Alignment of m_Attrs
	uint8 m_pad0D8[ 8 ];
#endif

	// CRenderAttributes, 16-byte aligned
	uint8 m_Attrs[ 0x3A0 ];

	uint64 m_nObjectFlagsRequiredMask;
	uint64 m_nObjectFlagsExcludedMask;
	uint8 m_unk490[ 0x28 ];
	char m_szDebugName[ 64 ];

	// Alignment of m_RenderTargetAttribute
	uint8 m_pad4F8[ 8 ];

	// CUtlAttributeList of 16 render target attributes of 8 bytes each
	uint8 m_RenderTargetAttribute[ 0xD0 ];

	int16 m_nNumClearColors;
	Vector4D m_vecClearColor[ 8 ];
	Rect_t m_scissorRect;
	IProceduralLayerRenderer *m_pRenderProcedure;
	bool m_bDeleteRenderProcedureWhenFinished;
};

#endif // ISCENELAYER_H
