//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Scene system utilities
//
//===========================================================================//

#ifndef ISCENEUTILS_H
#define ISCENEUTILS_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <scenesystem/isceneview.h>

#include <memory>

class ITonemapSystem;
class IVolumetricFog;
class CWindController;
class CCharacterDecalRenderer;
class IRenderingPipeline;
class IRenderGraphBuilder;
class IProceduralLayerRenderer;
class IRenderContext;
class ISceneWorld;
class CSceneObject;
class CModel;
class VMatrix;
class Vector2D;
struct RenderViewport_t;
struct SceneLayerOutputs_t;

// Methods without an s&box (2026, DWARF) name are named by what they do; their comments say so.
abstract_class ISceneUtils : public IAppSystem
{
public:
	// Named by what they do: a "Downsample2x" / "Downsample4x" pass of the render graph from a texture or from a
	// graph resource index (-1 is none). nTextureCreateFlags goes to the graph builder's texture creation
	// unchanged, bPassFlag into the pass constants. MSVC orders each pair in reverse
	virtual int Downsample2x( IRenderGraphBuilder *pGraphBuilder, ResourceHandle_t hTexture, uint64 nTextureCreateFlags, bool bPassFlag ) = 0;
	virtual int Downsample2x( IRenderGraphBuilder *pGraphBuilder, uint64 nTextureIndex, uint64 nTextureCreateFlags, bool bPassFlag ) = 0;
	virtual int Downsample4x( IRenderGraphBuilder *pGraphBuilder, ResourceHandle_t hTexture, uint64 nTextureCreateFlags, bool bPassFlag ) = 0;
	virtual int Downsample4x( IRenderGraphBuilder *pGraphBuilder, uint64 nTextureIndex, uint64 nTextureCreateFlags, bool bPassFlag ) = 0;

	// Adds a render layer with the extra object flags and the outputs
	virtual ISceneLayer *AddSimpleLayerToView( ISceneView *pView, const char *pszName, const RenderViewport_t &viewport, const CUtlStringToken &shaderMode, uint64 nObjectFlagsRequired, uint64 nObjectFlagsExcluded, const SceneLayerOutputs_t &outputs ) = 0;

	// Named by what it does: a "TranslucentScreenSpace" pass
	virtual int AddTranslucentScreenSpacePass( IRenderGraphBuilder *pGraphBuilder, ISceneView *pView, ISceneLayer *pLayer, uint64 nTargetIndex, uint64 nDepthIndex, bool bHalfResolution ) = 0;

	// Named by what it does: a full screen "screenquad" draw
	virtual void DrawScreenQuad( IRenderContext *pRenderContext, const RenderViewport_t &viewport, IMaterial2 *pMaterial ) = 0;

	virtual IProceduralLayerRenderer *CreateTextureBarrierLayerRenderer( const int *pTextures, int nTextureCount, const int *pLayouts, int nLayoutCount, const SceneLayerOutputs_t *pOutputs, ISceneLayer *pLayer ) = 0;

	virtual std::shared_ptr< ITonemapSystem > CreateTonemapSystem() = 0;

	// NULL unless the scene system has volumetric fog enabled.
	virtual IVolumetricFog *CreateVolumetricFog() = 0;
	virtual void DestroyVolumetricFog( IVolumetricFog *pVolumetricFog ) = 0;

	// NULL unless the scene system has wind enabled.
	virtual CWindController *CreateWindController() = 0;
	virtual void DestroyWindController( CWindController *pWindController ) = 0;

	virtual void RegisterRenderingPipeline( IRenderingPipeline *pPipeline ) = 0;
	virtual void UnregisterRenderingPipeline( IRenderingPipeline *pPipeline ) = 0;
	virtual IRenderingPipeline *FindRenderingPipeline( CUtlStringToken name ) = 0;
	virtual IRenderingPipeline *AliasRenderingPipeline( const char *pszAlias, CUtlStringToken name ) = 0;

	// s&box has two overloads of the same name here; MSVC orders them in reverse
	virtual ISceneLayer *AddTextureDebugLayer( ISceneView *pView, const char *pszName, const SceneLayerOutputs_t &outputs, const RenderViewport_t &viewport, HRenderTexture hTexture, int nMip, bool bAlpha ) = 0;
	virtual ISceneLayer *AddTextureDebugLayer( ISceneView *pView, const char *pszName, const SceneLayerOutputs_t &outputs, const RenderViewport_t &viewport, HSceneViewRenderTarget hTarget, int nMip, bool bAlpha ) = 0;

	// Named by what it does: the CRC64 of the parent view id and nIndex
	virtual CViewId CreateChildViewId( const CViewId &parentId, int nIndex ) = 0;
	virtual int AllocateViewProducerIndex() = 0;

	// Named by what it does: the pixel visibility convar and hardware occlusion query support
	virtual bool IsPixelVisibilityEnabled() = 0;
	virtual float PixelVisibility_FractionVisible( void *pQueryHandle, ISceneView *pView, float flDefault ) = 0;
	virtual void PixelVisibility_IssueQuery( int nQuery, void *pQueryHandle, int nPlayer, const Vector *pOrigin, const float *pRadius ) = 0;
	// Named by what it does: resizes the query pool under its lock
	virtual void PixelVisibility_SetQueryCount( uint32 nQueries, uint32 nPerPlayer ) = 0;

	virtual float *ComputeScreenSpaceVertices( const int *pViewport, int nX, int nY, int nWidth, int nHeight, float flU0, float flV0, float flU1, float flV1, int nTextureWidth, int nTextureHeight, float *pVertices ) = 0;
	virtual IProceduralLayerRenderer *CreateFullscreenQuadRenderer( IMaterial2 *pMaterial, const uint32 *pColor ) = 0;
	virtual IProceduralLayerRenderer *CreateScalableAORenderer( int nMultisampleType, bool bHalfRes, float flRadius, int nQuality, int nSamples, bool bTemporal, int nFlags ) = 0;
	virtual IProceduralLayerRenderer *CreateSignalSemaphoreRenderer( const char *pszName ) = 0;

	// Named by what it does: fills 608 bytes of filter constants for an nWidth x nHeight kernel
	virtual void ComputeTextureFilterConstants( int nWidth, int nHeight, const float *pWeights, float *pConstants, bool bNormalize ) = 0;
	virtual void FilterTexture( IRenderContext *pRenderContext, ResourceHandle_t hSource, ResourceHandle_t hDest, int nFilter, const float *pParams, bool bHorizontal, const RenderViewport_t *pViewport ) = 0;
	virtual IProceduralLayerRenderer *CreateFilterLayerRenderer( const char *pszName, IMaterial2 *pMaterial, int nFilter, const float *pParams ) = 0;
	// Named by what it does: a new 368-byte model object
	virtual CModel *CreateMDL() = 0;
	// Named by what it does: a scene object of the scene system's model preview type in pWorld
	virtual CSceneObject *CreateModelPreviewObject( ISceneWorld *pWorld, CModel *pModel, int nFlags ) = 0;

	// NULL unless the scene system has character decals enabled.
	virtual CCharacterDecalRenderer *CreateCharacterDecalRenderer() = 0;
	virtual void DestroyCharacterDecalRenderer( CCharacterDecalRenderer *pRenderer ) = 0;

	// Named by the data they work on, only partly traced: a transform composed from two matrices and bounds, a
	// render mode set on an object, the screen bounds of an object, its draw list for a pass, the bounds of its
	// extra data block and a draw of well-known texture 17. The arguments follow the types the code reads
	virtual void ComputeProjectionForBounds( const VMatrix &worldToView, const Vector *pMins, const Vector *pMaxs, float *pOutRange, float flScale, VMatrix *pOut ) = 0;
	virtual void SetObjectRenderMode( CSceneObject *pObject, int nMode, uint8 nPass, uint8 nFlags ) = 0;
	virtual bool ComputeObjectScreenBounds( const CSceneObject *pObject, float *pMins, float *pMaxs ) = 0;
	virtual void BuildObjectDrawList( IRenderContext *pRenderContext, CSceneObject *pObject, uint8 nPass, const float *pParams, void *pOut ) = 0;
	virtual void ComputeObjectExtraDataBounds( const CSceneObject *pObject, float *pBounds ) = 0;
	virtual void RenderWellKnownTextureDebug( IRenderContext *pRenderContext, int nTexture ) = 0;

	virtual void GPUSort( IRenderContext *pRenderContext, void *pSortParams ) = 0;
	// Named by what it does: forwards to a scene system internal helper
	virtual void FlushRenderGraphCache() = 0;
	virtual std::shared_ptr< IRenderGraphBuilder > CreateRenderGraphBuilder( const char *pszName ) = 0;
	virtual IProceduralLayerRenderer *CreateSimplePostProcessingLayerRenderer( IMaterial2 *pMaterial, bool bRendererFlag ) = 0;
	virtual IProceduralLayerRenderer *CreateQuadRenderer( IMaterial2 *pMaterial, const int *pRect, const RenderViewport_t *pViewport ) = 0;

	// Named by what it does: reads eye_data_list (EyeBallDataConstants_t) of a model
	virtual bool LoadEyeData( void *pScheduler, CModel *pModel, char *pszError ) = 0;

	// Named by what they do: add a "<name>Layer" render layer of a rendering pipeline mode to the view, two variants
	virtual ISceneLayer *AddPipelineLayer( ISceneView *pView, IRenderingPipeline **ppPipeline, const char *pszModeName, bool bOpaque ) = 0;
	virtual ISceneLayer *AddPipelineLayerAfter( ISceneView *pView, IRenderingPipeline **ppPipeline, const char *pszModeName, bool bOpaque ) = 0;

	// Named by what it does: the standard Opaque / AlphaTest / Translucent / DrawDepth layers of a view
	virtual void AddStandardRenderLayers( ISceneView *pView, IRenderingPipeline **ppPipeline, const SceneLayerOutputs_t *pOutputs ) = 0;
};

#endif // ISCENEUTILS_H
