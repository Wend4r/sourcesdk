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
#include <scenesystem/iscenesystem.h>
#include <tier0/utlstringtoken.h>

#include <memory>

class ISceneView;
class ISceneLayer;
class IRenderContext;
class IRenderingPipeline;
class CViewId;
class CFrustum;
class Vector;
class QAngle;
class Vector4D;
struct RenderViewport_t;
struct SceneLayerOutputs_t;
struct pixelvis_queryparams_t;
class ITonemapSystem;
class IVolumetricFog;
class CWindController;
class CCharacterDecalRenderer;
class IProceduralLayerRenderer;
class CRenderGraphBuilder;
struct VertexColor_t;
struct VertexPos2DTex2D_t;
enum SceneLayerMSAAMode_t : uint32;
enum RenderImageLayout_t : uint32;

FORWARD_DECLARE_HANDLE( HSceneViewRenderTarget );

using ScreenSpaceVertex_t = VertexPos2DTex2D_t;

abstract_class ISceneUtils : public IAppSystem
{
public:
	virtual void Add2xDownsampleLayerToView( ISceneView *pView, HRenderTexture hTexture, void *pUnk, bool bUnk ) = 0;

	virtual void Add4xDownsampleLayerToView( ISceneView *pView, HRenderTexture hTexture, void *pUnk, bool bUnk ) = 0;
	virtual void Add2xDownsampleLayerToView( ISceneView *pView, uint64 nTextureIndex, void *pUnk, bool bUnk ) = 0;
	virtual void Add4xDownsampleLayerToView( ISceneView *pView, uint64 nTextureIndex, void *pUnk, bool bUnk ) = 0;
	virtual ISceneLayer *AddSimpleLayerToView( ISceneView *pView, const char *pszLayerName, const RenderViewport_t &viewport, const CUtlStringToken &token, uint64 nFlags, uint64 nUnk, const SceneLayerOutputs_t &outputs ) = 0;
	virtual void AddTranslucentScreenspaceLayerToView( void *p ) = 0;
	virtual void RenderScreenQuad( IRenderContext *pRenderContext, Vector *pUnk, const char *pszUnk ) = 0;
	virtual ISceneLayer *AddTextureBarrierLayer( ISceneView *pView, const char *pszDebugName, int nTextureCount, const HRenderTexture *pTextures, int nRenderTargetCount, const HSceneViewRenderTarget *pRenderTargets, const SceneLayerMSAAMode_t *pMSAAModes, const RenderImageLayout_t *pImageLayouts ) = 0;

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

	// Overloads: MSVC places the texture one first.
	virtual ISceneLayer *AddTextureDebugLayer( ISceneView *pView, const char *pDebugName, const SceneLayerOutputs_t &outputs, const RenderViewport_t &viewport, HSceneViewRenderTarget hTexture, int nDebugShowTextureMode, bool bTextureIsSrgb ) = 0;
	virtual ISceneLayer *AddTextureDebugLayer( ISceneView *pView, const char *pDebugName, const SceneLayerOutputs_t &outputs, const RenderViewport_t &viewport, HRenderTexture hTexture, int nDebugShowTextureMode, bool bTextureIsSrgb ) = 0;

	virtual CViewId AppendViewProducerToViewId( CViewId viewId, int nViewProducer ) = 0;
	virtual int AllocateViewProducerIndex() = 0;
	virtual bool PixelVisibility_IsAvailable() const = 0;
	virtual float PixelVisibility_FractionVisible( const pixelvis_queryparams_t &params, int *pQueryHandle, const CViewId &viewId ) = 0;
	virtual void PixelVisibility_IssueQuery( const pixelvis_queryparams_t &params, int *pQueryHandle, IRenderContext *pRenderContext, const CFrustum &frustum, const CViewId &viewId ) = 0;
	virtual void PixelVisibility_AppendViewProducerToViewIdsForPlayerQueries( int nUnk1, int nUnk2 ) = 0;
	virtual void ComputeScreenSpaceVertices( const RenderViewport_t &viewport, int nDestX, int nDestY, int nDestWidth, int nDestHeight, float flSrcTextureX0, float flSrcTextureY0, float flSrcTextureX1, float flSrcTextureY1, int nSrcTextureWidth, int nSrcTextureHeight, ScreenSpaceVertex_t *pVertices ) = 0;
	virtual IProceduralLayerRenderer *CreateFullscreenQuadRenderer( HMaterialWeak hMaterial, const VertexColor_t &modulationColor ) = 0;
	virtual IProceduralLayerRenderer *CreateScalableAORenderer( void *pUnk, RenderMultisampleType_t nMultisampleType, float flRadius, float flBias, float flStrength, bool bBlur, int nUnk ) = 0;
	virtual IProceduralLayerRenderer *CreateSignalSemaphoreRenderer( const char *pszName ) = 0;
	virtual void unk040() = 0;
	virtual void FilterTexture( void *p ) = 0;
	virtual void CreateFilterLayerRenderer( void *p ) = 0;
	virtual void CreateMDL() = 0;
	virtual CSceneObject *Create6FaceSkybox( ISceneWorld *pWorld, HMaterialWeak *pMaterials, CUtlStringToken token ) = 0;

	// NULL unless the scene system has character decals enabled.
	virtual CCharacterDecalRenderer *CreateCharacterDecalRenderer() = 0;

	virtual void DestroyCharacterDecalRenderer( CCharacterDecalRenderer *pRenderer ) = 0;
	virtual void Compute3DSkyboxFrustum( const CFrustum &inFrustum, const Vector &vSkyboxOrigin, const QAngle &vSkyboxAngles, const Vector &vSkyboxCameraOrigin, float flSkyboxScale, CFrustum *pOutSkyboxFrustum ) = 0;
	// The fog colour of the view; fog_override, fog_color and fog_hdrcolorscale apply unless
	// bIgnoreFogOverride is set, and the colour is scaled by the HDR colour scale unless bNoHDRColorScale is set.
	virtual void ComputeFogColor( ISceneView *pView, Vector4D *pColor, bool bIgnoreFogOverride, bool bNoHDRColorScale ) = 0;
	virtual bool FindClipSpaceRectForObject( const CSceneObject *pObject, const ISceneView *pView, Vector4D &vRect ) = 0;
	virtual void unk050() = 0;
	virtual void unk051() = 0;
	virtual void unk052() = 0;
	virtual void GPUSort( void *p ) = 0;
	// Fills the "HairVolumeInfoBuffer" constant buffer and dispatches "HairShadowTileCompute".
	virtual void DispatchHairShadowTileCompute( void *p ) = 0;
	virtual std::shared_ptr< CRenderGraphBuilder > CreateRenderGraphBuilder( void *pUnk ) = 0;
	virtual IProceduralLayerRenderer *CreateSimplePostProcessingLayerRenderer( const void *pUnk, bool bUnk ) = 0;
	virtual IProceduralLayerRenderer *CreateQuadRenderer( HMaterialWeak hMaterial, const VertexColor_t &modulationColor, const RenderViewport_t &viewport ) = 0;
	// Builds the "EyeBallDataConstants_t" constant buffer from the "eye_data_list" of a model.
	virtual void CreateEyeballDataConstants( void *p ) = 0;

	// Adds a CSignalSemaphoreRenderer layer named "<pszLayerName>Layer"
	virtual void AddSignalSemaphoreLayerToView( ISceneView *pView, void *pUnk, const char *pszLayerName, bool bUnk ) = 0;

	// Same as AddSignalSemaphoreLayerToView, with the layer flag that puts the layer on the compute queue
	virtual void AddComputeSignalSemaphoreLayerToView( ISceneView *pView, void *pUnk, const char *pszLayerName, bool bUnk ) = 0;

	// Adds the "DrawDepthOpaque", "DrawDepthNeedsDynamic", "DrawDepthAlphaTest", "DrawDepthTranslucent" and "DrawDepth Fade" layers to the view
	virtual void AddDepthLayersToView( ISceneView *pView, const void *pUnk, const SceneLayerOutputs_t &outputs ) = 0;
};

#endif // ISCENEUTILS_H
