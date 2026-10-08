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

#include <memory>

class ITonemapSystem;
class IVolumetricFog;
class CWindController;
class CCharacterDecalRenderer;

abstract_class ISceneUtils : public IAppSystem
{
public:
	virtual void Unk_Downsample2x( void *pUnk1, void *pTexture, void *pUnk2, bool bUnk ) = 0;

	virtual void Unk_Downsample4x( void *pUnk1, void *pTexture, void *pUnk2, bool bUnk ) = 0;
	virtual void Unk_Downsample2x( void *pUnk1, uint64 nTextureIndex, void *pUnk2, bool bUnk ) = 0;
	virtual void Unk_Downsample4x( void *pUnk1, uint64 nTextureIndex, void *pUnk2, bool bUnk ) = 0;
	virtual void unk015() = 0;
	virtual void Unk_AddTranslucentScreenSpacePass( void *p ) = 0;
	virtual void unk017() = 0;
	virtual void Unk_CreateTextureBarrierLayerRenderer( void *p ) = 0;

	virtual std::shared_ptr< ITonemapSystem > CreateTonemapSystem() = 0;

	// NULL unless the scene system has volumetric fog enabled.
	virtual IVolumetricFog *CreateVolumetricFog() = 0;

	virtual void DestroyVolumetricFog( IVolumetricFog *pVolumetricFog ) = 0;

	// NULL unless the scene system has wind enabled.
	virtual CWindController *CreateWindController() = 0;

	virtual void DestroyWindController( CWindController *pWindController ) = 0;
	virtual void Unk_RegisterRenderingPipeline( void *p ) = 0;
	virtual void Unk_UnregisterRenderingPipeline( void *p ) = 0;
	virtual void unk026() = 0;
	virtual void Unk_AliasRenderingPipeline( void *p ) = 0;
	virtual void unk028() = 0;
	virtual void unk029() = 0;
	virtual void unk030() = 0;
	virtual void unk031() = 0;
	virtual void unk032() = 0;
	virtual void Unk_PixelVisibility_FractionVisible( void *p ) = 0;
	virtual void Unk_PixelVisibility_IssueQuery( void *p ) = 0;
	virtual void unk035() = 0;
	virtual void unk036() = 0;
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
