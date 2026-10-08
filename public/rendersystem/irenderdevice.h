//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: The render device
//
//===========================================================================//

#ifndef IRENDERDEVICE_H
#define IRENDERDEVICE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/basetypes.h"

class IRenderContext;
class CBufferString;
struct RenderDeviceInfo_t;
enum RenderMultisampleType_t : uint8;

FORWARD_DECLARE_HANDLE( SwapChainHandle_t );

abstract_class IRenderDevice
{
public:
	// Shader API of the device: 0 Direct3D 11, 1 Vulkan
	virtual int GetDeviceAPI() = 0;

	virtual SwapChainHandle_t Unk_CreateSwapChain( void *p ) = 0;
	virtual void DestroySwapChain( SwapChainHandle_t hSwapChain ) = 0;

	// Applies a new mode to the swap chain
	virtual bool UpdateSwapChain( SwapChainHandle_t hSwapChain, const RenderDeviceInfo_t &mode ) = 0;

	virtual const RenderDeviceInfo_t &GetSwapChainInfo( SwapChainHandle_t hSwapChain ) = 0;
	virtual void *unk005( SwapChainHandle_t hSwapChain ) = 0;
	virtual void *unk006( SwapChainHandle_t hSwapChain ) = 0;
	virtual int unk007( SwapChainHandle_t hSwapChain ) = 0;
	virtual void GetBackBufferDimensions( SwapChainHandle_t hSwapChain, int *pWidth, int *pHeight ) = 0;
	virtual int unk009() = 0;
	virtual bool unk010() = 0;

	// Prints the kind of render device
	virtual void unk011() = 0;

	virtual bool unk012() = 0;

	virtual const char *GetShaderModelString( int nShaderType ) = 0;

	virtual void unk014() = 0;
	virtual void BeginSubmittingDisplayLists( void *p ) = 0;
	virtual void Unk_Present( void *p ) = 0;
	virtual void Flush() = 0;
	virtual void ForceFlushGPU( void *p ) = 0;
	virtual void SetHardwareGammaRamp( SwapChainHandle_t hSwapChain, float flGamma ) = 0;
	virtual void Unk_CompileShader( void *p ) = 0;
	virtual void Unk_CreateShader( void *p ) = 0;
	virtual void DestroyShader( int nShaderType, void *hShader ) = 0;

	// Register counts and binary size of a compiled shader
	virtual void Unk_GetShaderStatistics( void *p ) = 0;

	virtual void *CreateInputLayout( const char *pName, int nFields, const void *pFields ) = 0;
	virtual void AddRefInputLayout( void *hLayout ) = 0;
	virtual void ReleaseInputLayout( void *hLayout ) = 0;
	virtual void Unk_ConcatenateInputLayouts( void *p ) = 0;
	virtual int GetInputLayoutFields( void *hLayout, const void **ppFields, int *pCount ) = 0;
	virtual void *unk029() = 0;

	// Cached render states, created on first use
	virtual void *FindOrCreateRasterizerState( const void *pDesc ) = 0;
	virtual void *FindOrCreateDepthStencilState( const void *pDesc ) = 0;
	virtual void *FindOrCreateBlendState( const void *pDesc ) = 0;

	virtual void unk033( void *p ) = 0;
	virtual void unk034( void *p ) = 0;
	virtual void unk035( void *p ) = 0;
	virtual void unk036( void *hBuffer ) = 0;

	// Same as unk035 with flag bit 1 added
	virtual void unk037( void *p ) = 0;

	virtual void unk038( void *hBuffer ) = 0;
	virtual void unk039( void *p ) = 0;
	virtual void unk040( void *p ) = 0;
	virtual void unk041( void *p ) = 0;
	virtual void *unk042( void *hBuffer ) = 0;
	virtual void Unk_CreateGPUBuffer( void *p ) = 0;
	virtual void Unk_CreateGPUBufferFormatted( void *p ) = 0;
	virtual uint64 unk045( void *hBuffer ) = 0;
	virtual void unk046( void *hBuffer ) = 0;
	virtual uint64 unk047( void *hBuffer ) = 0;
	virtual uint64 unk048( void *hBuffer ) = 0;
	virtual uint64 unk049( void *hBuffer ) = 0;
	virtual uint64 unk050( void *hBuffer ) = 0;
	virtual void Unk_FindOrCreateTexture( void *p ) = 0;
	virtual void unk052( void *p ) = 0;
	virtual void unk053( void *p ) = 0;
	virtual void unk054( void *p ) = 0;
	virtual void unk055( void *p ) = 0;
	virtual void unk056( void *p ) = 0;
	virtual void unk057( void *p ) = 0;

	virtual void *CreateQueryObject( int nType ) = 0;

	// Returns 0 once the result is available
	virtual int GetQueryObjectData( void *hQuery, void *pData ) = 0;

	virtual void DeleteQueryObject( void *hQuery ) = 0;

	// Forwards to CreateRenderContext( nFlags, NULL, NULL )
	virtual IRenderContext *unk061( uint32 nFlags ) = 0;

	virtual IRenderContext *CreateRenderContext( uint32 nFlags, void *pUnk1, void *pUnk2 ) = 0;
	virtual void ReleaseRenderContext( IRenderContext *pRenderContext ) = 0;
	virtual void Unk_SubmitDisplayLists( void *p ) = 0;
	virtual void unk065( void *p ) = 0;
	virtual void *unk066( int nUnk ) = 0;
	virtual void Unk_CreateConstantBuffer( void *p ) = 0;
	virtual void unk068( void *p ) = 0;
	virtual void unk069( void *p ) = 0;
	virtual int unk070( void *hBuffer ) = 0;
	virtual int unk071( void *hBuffer, int *pUnk1, int *pUnk2 ) = 0;
	virtual void unk072() = 0;
	virtual void unk073( void *p ) = 0;
	virtual void Unk_ReadTexturePixels( void *p ) = 0;
	virtual void unk075( void *p ) = 0;
	virtual bool unk076( void *hRequest ) = 0;
	virtual bool unk077( void *hRequest, void **ppData, int *pSize ) = 0;
	virtual bool unk078( void *hRequest ) = 0;
	virtual void *unk079( void *hTexture ) = 0;

	// RENDERDEBUGFLAG_* bits
	virtual void SetRenderDebugFlags( uint32 nFlags, bool bSet ) = 0;
	virtual bool IsRenderDebugFlagSet( uint32 nFlags ) = 0;
	virtual uint32 GetRenderDebugFlags() = 0;

	// Frame pacing target, 0 disables it
	virtual void SetFramePacingTargetFPS( double flFPS ) = 0;

	virtual double GetFramePacingTargetFPS() = 0;
	virtual void unk085( void *p ) = 0;
	virtual void unk086( void *p ) = 0;
	virtual void unk087( void *p ) = 0;
	virtual void unk088( void *p ) = 0;
	virtual void unk089( void *p ) = 0;
	virtual void unk090( void *p ) = 0;
	virtual void unk091( void *p ) = 0;
	virtual void unk092( void *p ) = 0;
	virtual void unk093( void *p ) = 0;
	virtual void unk094( void *p ) = 0;
	virtual void unk095( void *p ) = 0;
	virtual void unk096( void *p ) = 0;
	virtual void unk097( void *p ) = 0;
	virtual void unk098( void *p ) = 0;
	virtual const char *MultisampleTypeToString( RenderMultisampleType_t nType ) = 0;
	virtual void unk100( void *p ) = 0;
	virtual void unk101( void *p ) = 0;
	virtual void unk102( void *p ) = 0;
	virtual void unk103( void *p ) = 0;
	virtual void unk104( void *p ) = 0;
	virtual void unk105( void *p ) = 0;
	virtual void unk106( void *p ) = 0;
	virtual void unk107( void *p ) = 0;
	virtual void unk108( void *p ) = 0;
	virtual void unk109( void *p ) = 0;
	virtual void unk110( void *p ) = 0;
	virtual void unk111( void *p ) = 0;
	virtual void unk112( void *p ) = 0;
	virtual bool unk113() = 0;
	virtual void unk114() = 0;
	virtual void unk115( void *p ) = 0;
	virtual void unk116( void *p ) = 0;
	virtual void unk117( void *p ) = 0;
	virtual void unk118( void *p ) = 0;
	virtual void unk119( void *p ) = 0;
	virtual void unk120( void *p ) = 0;
	virtual void unk121( void *p ) = 0;
	virtual void unk122( void *p ) = 0;
	virtual void unk123( void *p ) = 0;
	virtual void unk124( void *p ) = 0;
	virtual void unk125( void *p ) = 0;
	virtual void unk126( void *p ) = 0;

	virtual void AppendResourceStats( CBufferString *pOut ) = 0;

	virtual void unk128( void *p ) = 0;
	virtual void unk129( void *p ) = 0;
	virtual void Unk_GetDeviceSpecificTexture( void *p ) = 0;
	virtual void unk131( void *p ) = 0;
	virtual void unk132( void *p ) = 0;
	virtual void unk133( void *p ) = 0;
	virtual void unk134( void *p ) = 0;
	virtual void unk135( void *p ) = 0;
	virtual void unk136( void *p ) = 0;
	virtual void unk137( void *p ) = 0;
	virtual void unk138( void *p ) = 0;
	virtual void unk139( void *p ) = 0;
	virtual void unk140( int *pUnk1, int *pUnk2 ) = 0;
	virtual void unk141( void *p ) = 0;
	virtual void unk142( void *p ) = 0;
	virtual void unk143( void *p ) = 0;
	virtual void unk144( void *p ) = 0;
	virtual void unk145( void *p ) = 0;
	virtual void unk146( void *p ) = 0;
	virtual void unk147( void *p ) = 0;
	virtual void Unk_CreateDescriptorSet( void *p ) = 0;
	virtual void Unk_UpdateDescriptorSet( void *p ) = 0;
	virtual void Unk_AllocateUpdateAfterBindDescriptorSet( void *p ) = 0;
	virtual void unk151( void *p ) = 0;
	virtual void unk152( void *p ) = 0;
	virtual void unk153( void *p ) = 0;
	virtual void Unk_AddDescriptorsToGlobalPerFrameDescriptorSet( void *p ) = 0;
	virtual void unk155( void *p ) = 0;
	virtual void unk156( void *p ) = 0;
	virtual void unk157( void *p ) = 0;
	virtual void unk158( void *p ) = 0;
	virtual void unk159( void *p ) = 0;
	virtual float unk160() = 0;
	virtual void Unk_AsyncSetTextureData( void *p ) = 0;
	virtual void unk162( void *p ) = 0;
	virtual void unk163( void *p ) = 0;
	virtual void unk164( void *p ) = 0;
	virtual void unk165( void *p ) = 0;

	// Needs RENDERDEBUGFLAG_ENABLE_GPU_TIMING
	virtual bool GetGPUFrameTimeMS( void *pUnk, float *pTimeMS, uint32 *pUnk2 ) = 0;

	virtual void unk167( void *p ) = 0;
	virtual void unk168( void *p ) = 0;
	virtual void LowLatencySleep() = 0;
	virtual void SetLatencyMarker( uint32 nMarker ) = 0;
	virtual void unk171( void *p ) = 0;
	virtual bool IsFramePacingSupported() = 0;
	virtual void unk173( void *p ) = 0;
	virtual void Unk_FreeTextureGPUResources( void *p ) = 0;
	virtual void unk175( void *p ) = 0;
	virtual void unk176( float flValue ) = 0;
	virtual float unk177() = 0;
	virtual void unk178( void *p ) = 0;
	virtual void unk179( void *p ) = 0;
	virtual void unk180( void *p ) = 0;
	virtual void unk181( void *p ) = 0;
	virtual void unk182( void *p ) = 0;
	virtual void unk183( void *p ) = 0;
	virtual void unk184( void *p ) = 0;
	virtual void Unk_CreateTLAS( void *p ) = 0;
	virtual void unk186( void *p ) = 0;
	virtual void unk187( void *p ) = 0;
	virtual void Unk_CreateRayTracePipeline( void *p ) = 0;
	virtual void unk189( void *p ) = 0;
	virtual void unk190( void *p ) = 0;
	virtual void unk191( void *p ) = 0;
	virtual void unk192( void *p ) = 0;
	virtual void Unk_CreateOpacityMicromap( void *p ) = 0;
	virtual void unk194( void *p ) = 0;
	virtual void unk195( void *p ) = 0;
	virtual bool IsRayTracingSupported() = 0;
	virtual void unk197( void *p ) = 0;
	virtual void unk198( void *p ) = 0;
	virtual void Unk_StartRenderDocCapture( void *p ) = 0;
	virtual void Unk_EndRenderDocCapture( void *p ) = 0;
	virtual void unk201( void *p ) = 0;
	virtual void unk202( void *p ) = 0;
	virtual void unk203( void *p ) = 0;
	virtual void unk204( void *p ) = 0;
	virtual void unk205( void *p ) = 0;
	virtual void Unk_AllocatePooledIndexBuffer( void *p ) = 0;
	virtual void unk207( void *p ) = 0;
	virtual void unk208( void *p ) = 0;
	virtual void unk209( void *p ) = 0;
	virtual void Unk_AllocatePooledVertexBuffer( void *p ) = 0;
	virtual void unk211( void *p ) = 0;
	virtual void unk212( void *p ) = 0;
	virtual void unk213( void *p ) = 0;
	virtual void unk214( void *p ) = 0;
	virtual void unk215( void *p ) = 0;
	virtual void *unk216( int nType ) = 0;

	// XeSS / FidelityFX availability
	virtual bool IsSuperSamplingAvailable( int nType ) = 0;

	virtual void unk218( void *p ) = 0;
	virtual void unk219( void *p ) = 0;
	virtual void Unk_BeginRenderingToSwapChain( void *p ) = 0;
	virtual void unk221( void *p ) = 0;

	virtual void Unk_CreateDeviceMemoryPool( void *p ) = 0;
	virtual void Unk_DestroyDeviceMemoryPool( void *p ) = 0;
	virtual void Unk_AllocateTextureInPool( void *p ) = 0;
	virtual void Unk_FreeTextureFromPool( void *p ) = 0;
	virtual void Unk_AllocateBufferInPool( void *p ) = 0;
	virtual void Unk_FreeBufferFromPool( void *p ) = 0;
	virtual void Unk_GetDeviceMemoryTextureAllocationInfo( void *p ) = 0;
	virtual void Unk_GetDeviceMemoryBufferAllocationInfo( void *p ) = 0;
	virtual void unk230( void *p ) = 0;
	virtual void Unk_CheckFormatSupport( void *p ) = 0;
	virtual void unk232( void *p ) = 0;

	// Device generated commands
	virtual void Unk_CreateIndirectCommandsLayout( void *p ) = 0;
	virtual void Unk_DestroyIndirectCommandsLayout( void *p ) = 0;
	virtual void Unk_CreateIndirectCommandsPreprocessBuffer( void *p ) = 0;
	virtual void Unk_CreateIndirectExecutionSet( void *p ) = 0;
	virtual void Unk_UpdateIndirectExecutionSet( void *p ) = 0;
	virtual void unk238( void *p ) = 0;

	virtual void unk239( const char *pName ) = 0;

	virtual int unk240() = 0;
	virtual int unk241() = 0;
	virtual void unk242( void *p ) = 0;
	virtual void unk243( void *p ) = 0;
	virtual void Unk_ReadCompiledTextureMips( void *p ) = 0;
	virtual uint64 unk245() = 0;
	virtual void unk246( void *p ) = 0;
	virtual void unk247( void *p ) = 0;
	virtual void unk248( void *p ) = 0;
	virtual void unk249( void *p ) = 0;
	virtual void unk250( void *p ) = 0;
	virtual bool InitDevice( int nAdapter, int nUnk1, uint32 nUnk2 ) = 0;
	virtual void ShutdownDevice() = 0;
	virtual bool unk253() = 0;
	virtual bool IsMultisampleTypeSupported( int nFormat, RenderMultisampleType_t nType ) = 0;
};

#endif // IRENDERDEVICE_H
