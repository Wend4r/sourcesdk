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
#include <tier0/utlstringtoken.h>

#include <memory>

class ISceneView;
class ISceneLayer;
class IRenderContext;
class IRenderingPipeline;
class CViewId;
class CFrustum;
class Vector;
struct RenderViewport_t;
struct SceneLayerOutputs_t;
struct pixelvis_queryparams_t;
class ITonemapSystem;
class IVolumetricFog;
class CWindController;
class CCharacterDecalRenderer;

abstract_class ISceneUtils : public IAppSystem
{
public:
	virtual void Add2xDownsampleLayerToView( ISceneView *pView, void *pTexture, void *pUnk, bool bUnk ) = 0;

	virtual void Add4xDownsampleLayerToView( ISceneView *pView, void *pTexture, void *pUnk, bool bUnk ) = 0;
	virtual void Add2xDownsampleLayerToView( ISceneView *pView, uint64 nTextureIndex, void *pUnk, bool bUnk ) = 0;
	virtual void Add4xDownsampleLayerToView( ISceneView *pView, uint64 nTextureIndex, void *pUnk, bool bUnk ) = 0;
	virtual ISceneLayer *AddSimpleLayerToView( ISceneView *pView, const char *pszLayerName, const RenderViewport_t &viewport, const CUtlStringToken &token, uint64 nFlags, uint64 nUnk, const SceneLayerOutputs_t &outputs ) = 0;
	virtual void Unk_AddTranslucentScreenspaceLayerToView( void *p ) = 0;
	virtual void RenderScreenQuad( IRenderContext *pRenderContext, Vector *pUnk, const char *pszUnk ) = 0;
	virtual void Unk_CreateTextureBarrierLayerRenderer( void *p ) = 0;

	virtual std::shared_ptr< ITonemapSystem > CreateTonemapSystem() = 0;

	// NULL unless the scene system has volumetric fog enabled.
	virtual IVolumetricFog *CreateVolumetricFog() = 0;

	virtual void DestroyVolumetricFog( IVolumetricFog *pVolumetricFog ) = 0;

	// NULL unless the scene system has wind enabled.
	virtual CWindController *CreateWindController() = 0;

	virtual void DestroyWindController( CWindController *pWindController ) = 0;
	virtual void RegisterRenderingPipeline( IRenderingPipeline *pPipeline ) = 0;
	virtual void UnregisterRenderingPipeline( IRenderingPipeline *pPipeline ) = 0;
	virtual IRenderingPipeline *FindRenderingPipeline( CUtlStringToken token ) = 0;
	virtual void AliasRenderingPipeline( const char *pszAlias, CUtlStringToken token ) = 0;
	virtual void unk028() = 0;
	virtual void unk029() = 0;
	virtual CViewId AppendViewProducerToViewId( CViewId viewId, int nViewProducer ) = 0;
	virtual int AllocateViewProducerIndex() = 0;
	virtual bool PixelVisibility_IsAvailable() const = 0;
	virtual float PixelVisibility_FractionVisible( const pixelvis_queryparams_t &params, int *pQueryHandle, const CViewId &viewId ) = 0;
	virtual void PixelVisibility_IssueQuery( const pixelvis_queryparams_t &params, int *pQueryHandle, IRenderContext *pRenderContext, const CFrustum &frustum, const CViewId &viewId ) = 0;
	virtual void PixelVisibility_AppendViewProducerToViewIdsForPlayerQueries( int nUnk1, int nUnk2 ) = 0;
	virtual void Unk_ComputeScreenSpaceVertices( void *p ) = 0;
	virtual void Unk_CreateFullscreenQuadRenderer( void *p ) = 0;
	virtual void Unk_CreateScalableAORenderer( void *p ) = 0;
	virtual void Unk_CreateSignalSemaphoreRenderer( void *p ) = 0;
	virtual void unk040() = 0;
	virtual void Unk_FilterTexture( void *p ) = 0;
	virtual void Unk_CreateFilterLayerRenderer( void *p ) = 0;
	virtual void Unk_CreateMDL() = 0;
	virtual void unk044() = 0;

	// NULL unless the scene system has character decals enabled.
	virtual CCharacterDecalRenderer *CreateCharacterDecalRenderer() = 0;

	virtual void DestroyCharacterDecalRenderer( CCharacterDecalRenderer *pRenderer ) = 0;
	virtual void unk047() = 0;
	virtual void unk048() = 0;
	virtual void unk049() = 0;
	virtual void unk050() = 0;
	virtual void unk051() = 0;
	virtual void unk052() = 0;
	virtual void Unk_GPUSort( void *p ) = 0;
	virtual void unk054() = 0;
	virtual void Unk_CreateRenderGraphBuilder( void *p ) = 0;
	virtual void Unk_CreateSimplePostProcessingLayerRenderer( void *p ) = 0;
	virtual void Unk_CreateQuadRenderer( void *p ) = 0;
	virtual void unk058() = 0;
	virtual void unk059() = 0;
	virtual void unk060() = 0;
	virtual void unk061() = 0;
};

#endif // ISCENEUTILS_H
