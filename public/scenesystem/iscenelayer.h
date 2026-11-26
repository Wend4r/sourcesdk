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
#include "tier0/utlattributelist.h"
#include "tier0/utlvector.h"
#include "mathlib/vector4d.h"
#include "rendersystem/rendersystemtypes.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcesystem/resourcehandletypes.h"
#include "materialsystem2/imaterial2.h"

class ISceneView;
class ISceneLayer;
class IRenderContext;
class CFrustum;
struct RenderTargetDesc_t;
struct SceneSystemPerFrameStats_t;

FORWARD_DECLARE_HANDLE( HSceneViewRenderTarget );
FORWARD_DECLARE_HANDLE( RsRasterizerStateHandle_t );
FORWARD_DECLARE_HANDLE( RsDepthStencilStateHandle_t );
FORWARD_DECLARE_HANDLE( RsBlendStateHandle_t );
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

// Render state overrides of a layer, filled by the client render pipeline (depth bias from sc_shadow_depth_bias*).
// They belong to the render
// system; defined here until a rendersystem header declares them
struct RsStencilStateDesc_t
{
	// RsStencilOp_t / RsComparison_t values
	uint16 m_bStencilEnable : 1;
	uint16 m_frontStencilFailOp : 3;
	uint16 m_frontStencilDepthFailOp : 3;
	uint16 m_frontStencilPassOp : 3;
	uint16 m_frontStencilFunc : 3;
	uint16 m_backStencilFailOp : 3;
	uint16 m_backStencilDepthFailOp : 3;
	uint16 m_backStencilPassOp : 3;
	uint16 m_backStencilFunc : 3;
	uint8 m_nStencilReadMask;
	uint8 m_nStencilWriteMask;
};

struct RsStencilStateOverride_t
{
	uint16 m_nStencilRefValue;
	RsStencilStateDesc_t m_stencilState;
};

struct RsDepthBiasStateOverride_t
{
	int m_nDepthBias;
	float m_flDepthBiasClamp;
	float m_flSlopeScaledDepthBias;
};

// A 128-bit mask of scene object classes (classes are uint8 indices). Passed by value: a pointer on Windows,
// two registers on Linux
struct SceneObjectClassMask_t
{
	uint64 m_nBits[ 2 ];
};

// Storage of the layer's CRenderAttributes (0x3A0 bytes, 16-byte aligned). The class itself belongs to
// materialsystem2 and has no declaration in the SDK yet
struct alignas( 16 ) SceneLayerRenderAttributesStorage_t
{
	uint8 m_Storage[ 0x3A0 ];
};

// Draws a procedural layer; called while the view renders
abstract_class IProceduralLayerRenderer
{
public:
	// pStats receives the draw calls and primitives the renderer issues
	virtual void Render( ISceneView *pView, IRenderContext *pRenderContext, ISceneLayer *pLayer, SceneSystemPerFrameStats_t *pStats ) = 0;

	virtual ~IProceduralLayerRenderer() {}

	// Named by what it does: true makes AddProceduralLayer create a layer of type 2 instead of 1; every scene
	// system renderer returns false
	virtual bool UseAlternateProceduralLayerType() = 0;
};

// 1920 bytes on Windows, 1904 on Linux
abstract_class ISceneLayer
{
public:
	// 8 bytes
	struct RenderTargetAttribute_t
	{
		HSceneViewRenderTarget m_hRenderTarget;
	};

	struct InputTexture_t
	{
		HRenderTexture m_hTexture;
		// RenderImageLayout_t
		int m_nImageLayout;
	};

	virtual const RenderTargetDesc_t &GetRenderTargetDesc() = 0;
	virtual void SetRenderTargetDesc( const RenderTargetDesc_t &desc ) = 0;

	// The per-frame stats of this layer alone
	virtual SceneSystemPerFrameStats_t &GetPerFrameStats() = 0;

	virtual void DisableReflectionsAndRefractionsForObjectsInThisLayer( bool bDisable ) = 0;

	// Keeps a copy of the frustum
	virtual void OverrideFrustum( const CFrustum &frustum ) = 0;

	// Named by what it does: adds this layer to the layers that wait for pLayer (SetFadeTargetLayer does the
	// same for its target)
	virtual void AddToDependentsOf( ISceneLayer *pLayer ) = 0;

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
	virtual void SetCullPlane( const Vector4D &vPlane ) = 0;

	// Kept only while the scene system's debug tracking is on
	virtual void SetPerfMarker( const char *pszMarker ) = 0;

	// -1 falls back to sc_max_framebuffer_copies_per_layer
	virtual void SetMaxFramebufferCopies( int nMaxCopies ) = 0;

public:
	// MSVC places m_viewport at 16 because of the 16-byte alignment of the class, g++ at 8
	RenderViewport_t m_viewport;
	int m_eLayerType;
	CUtlStringToken m_shaderMode;

	// The name m_shaderMode is the token of
	CBufferString m_shaderModeName;

	uint64 m_nLayerFlags;
	uint32 m_nClearFlags;
	SceneLayerOutputs_t m_RenderTargetOutputs;

	// CRenderAttributes
	SceneLayerRenderAttributesStorage_t m_Attrs;

	uint64 m_nObjectFlagsRequiredMask;
	uint64 m_nObjectFlagsExcludedMask;

	// The type flags half of the masks and the class mask; ISceneView copies its defaults of all three into
	// every layer it adds
	uint64 m_nObjectTypeFlagsRequiredMask;
	uint64 m_nObjectTypeFlagsExcludedMask;
	SceneObjectClassMask_t m_ObjectClassMask;

	// Named by position: zeroed by the constructor after the masks; no reader traced
	uint64 m_nExtraObjectFlagsMask;

	char m_szDebugName[ 64 ];

	CUtlAttributeList< RenderTargetAttribute_t, 16 > m_RenderTargetAttribute;

	int16 m_nNumClearColors;
	Vector4D m_vecClearColor[ 8 ];
	Rect_t m_scissorRect;
	IProceduralLayerRenderer *m_pRenderProcedure;
	bool m_bDeleteRenderProcedureWhenFinished;

	// Renders before the layer is submitted; deleted with the layer when
	// the flag is set
	IProceduralLayerRenderer *m_pPreSubmitRenderProcedure;
	bool m_bDeletePreSubmitRenderProcedureWhenFinished;

	// Applied to the render context before the layer draws: rasterizer state, depth
	// stencil state with m_nOverrideStencilRef, and blend state with its blend factor and sample mask (-1 by default)
	RsRasterizerStateHandle_t m_hOverrideRasterizerState;
	RsDepthStencilStateHandle_t m_hOverrideDepthStencilState;
	RsBlendStateHandle_t m_hOverrideBlendState;
	Vector4D m_vecOverrideBlendFactor;
	uint32 m_nOverrideBlendSampleMask;

	// Used instead of the material's states when the matching flag below is set
	RsStencilStateOverride_t m_StencilStateOverride;
	RsDepthBiasStateOverride_t m_DepthBiasStateOverride;
	bool m_bStencilStateOverridden;
	bool m_bDepthBiasStateOverridden;
	int m_nOverrideStencilRef;

	// Objects whose screen size is below it are culled; a negative value culls the objects above its magnitude
	float m_flSizeCullThreshold;

	// Fragment shading rate of the layer (1 x 1 by default), set when the hardware supports variable rate shading
	int m_nShadingRateWidth;
	int m_nShadingRateHeight;

	CUtlStringToken m_nObjectMatchID;
	HMaterialStrong m_hOverrideMaterial;

	// The label of a label layer (ISceneView::AddLabelLayer)
	CUtlStringToken m_nLabel;
	// The stencil bit token the layer depends on
	CUtlStringToken m_nStencilBitDependency;
	ISceneView *m_pParentView;

	CUtlVectorFixedGrowable< ISceneLayer *, 1 > m_layersThatMustRenderObjectsForThisLayerToBeDrawn;

	// Textures that get a barrier to their layout before the layer renders
	CUtlVectorFixedGrowable< InputTexture_t, 4 > m_inputTextures;

	// Zeroed by the constructor
	void *m_hSemaphore;

	// Named by what the code does: set while the procedural layer path works on the layer's own render target
	bool m_bOwnRenderTargetPending;
	// Named by what the code does: set together with the clear color (glow passes); copied when the layer is duplicated
	bool m_bClearColorSet;
};

#endif // ISCENELAYER_H
