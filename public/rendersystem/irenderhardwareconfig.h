//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: Capabilities and limits of the graphics adapter
//
//===========================================================================//

#ifndef IRENDERHARDWARECONFIG_H
#define IRENDERHARDWARECONFIG_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "mathlib/intvector3d.h"

struct RenderAdapterInfo_t;
enum RenderMultisampleType_t : uint8;

abstract_class IRenderHardwareConfig
{
public:
	virtual int unk000() = 0;

	virtual int GetMaxTextureWidth() = 0;
	virtual int GetMaxTextureHeight() = 0;

	// Texture memory budget in bytes
	virtual uint64 GetTextureMemorySize() = 0;

	// Current DX level (110, 111, 120)
	virtual int GetDXSupportLevel() = 0;

	virtual int GetMaxTextureDepth() = 0;
	virtual int GetMaxViewports() = 0;
	virtual bool unk007() = 0;

	// r_prefer_loop_unrolling
	virtual bool PreferLoopUnrolling() = 0;

	virtual bool ReverseDepth() = 0;
	virtual bool IsFastSubrectClearSupported() = 0;

	virtual const RenderAdapterInfo_t &AdapterInfo() = 0;

	virtual int GetRenderSystemType() = 0;

	virtual int unk013() = 0;

	// The render system the engine offers to switch to, 0 for none; the Vulkan module recommends DX11 only with
	// "-force_recommend_dx11" and when the current render system is not Vulkan
	virtual int GetRecommendedRenderSystemType() = 0;

	virtual const char *RenderSystemTypeToString( int nType ) = 0;
	virtual const char *RenderSystemTypeToDisplayName( int nType ) = 0;
	virtual const char *RenderSystemTypeToCommandLineSwitch( int nType ) = 0;

	virtual int GetAvailableRenderSystems( void *pRenderSystems ) = 0;

	// Search such a list, return -1 when not found
	virtual int FindRenderSystemByName( const void *pRenderSystems, const char *pName ) = 0;
	virtual int FindRenderSystemByCommandLineSwitch( const void *pRenderSystems, const char *pSwitch ) = 0;
	virtual int FindRenderSystemByType( const void *pRenderSystems, int nType ) = 0;

	virtual bool IsSubrectResolveSupported( bool bMSAA ) = 0;

	virtual bool IsMultisampleTypeSupported( int nFormat, RenderMultisampleType_t nType ) = 0;

	// Highest supported multisample type not above nDesired
	virtual RenderMultisampleType_t GetBestSupportedMultisampleType( int nFormat, RenderMultisampleType_t nDesired ) = 0;

	virtual bool SupportsDescriptorSets() = 0;

	virtual int GetMaxPerStageSampledTextures() = 0;
	virtual int GetMaxPushConstantsSize() = 0;
	virtual int GetMinConstantBufferOffsetAlignment() = 0;

	virtual bool SupportsBarriers() = 0;

	// Separate async compute queue
	virtual bool SupportsAsyncCompute() = 0;

	// r_mipgen_compute_shader
	virtual bool UseComputeShaderMipGen() = 0;

	virtual bool SupportsShaderClipDistance() = 0;

	virtual bool SupportsLowLatency() = 0;

	// HLSL SM6.0 subgroup wave operations
	virtual bool SupportsWaveOps() = 0;

	// GPU texture compression writes the blocks straight into mip views of the compressed texture when set,
	// through intermediate textures and copies otherwise
	virtual bool SupportsDirectCompressedTextureWrites() = 0;

	// Ray tracing pipelines; also requires DX level 120
	virtual bool SupportsRayTracing() = 0;

	virtual bool SupportsRayQuery() = 0;
	virtual bool SupportsExtendedDynamicState() = 0;
	virtual bool SupportsDynamicRendering() = 0;
	virtual bool SupportsBufferDeviceAddress() = 0;

	// Descriptor indexing
	virtual bool SupportsBindless() = 0;

	// Graphics pipeline libraries with async pipeline compilation
	virtual bool SupportsGraphicsPipelineLibrary() = 0;

	virtual bool SupportsDrawIndirectCount() = 0;

	virtual int GetMinSubgroupSize() = 0;
	virtual int GetMaxSubgroupSize() = 0;

	virtual int GetLowLatencyMode() = 0;

	// The Vulkan device sets it when it has the VK_KHR_shader_float16_int8 and VK_KHR_16bit_storage features
	virtual bool SupportsNative16BitShaderOps() = 0;

	virtual bool unk048() = 0;
	virtual bool SupportsDeviceGeneratedCommands() = 0;
	virtual bool SupportsShaderInvocationReorder() = 0;

	// The Vulkan device sets it when its second queue family can present to the swap chain surface
	virtual bool SupportsPresentFromSecondQueue() = 0;

	virtual bool SupportsMeshShaders() = 0;

	virtual int GetMaxMeshWorkGroupTotalCount() = 0;
	virtual IntVector3D GetMaxMeshWorkGroupCount() = 0;
	virtual int GetMaxPreferredMeshWorkGroupInvocations() = 0;
	virtual bool MeshShaderPrefersLocalInvocationVertexOutput() = 0;

	virtual bool SupportsConditionalRendering() = 0;

	// NVIDIA Reflex is initialized and reports "Latewarp.Available"
	virtual bool SupportsLateWarp() = 0;

	virtual bool SupportsRayTracingPositionFetch() = 0;
	virtual int GetMinAccelerationStructureScratchOffsetAlignment() = 0;
	virtual bool SupportsVariableRateShading() = 0;

	// Variable rate shading limits; either pointer may be NULL
	virtual void GetMaxShadingRateFragmentSize( int *pWidth, int *pHeight ) = 0;
	virtual int GetMaxShadingRateFragmentSizeAspectRatio() = 0;
	virtual void GetMinShadingRateAttachmentTexelSize( int *pWidth, int *pHeight ) = 0;
	virtual void GetMaxShadingRateAttachmentTexelSize( int *pWidth, int *pHeight ) = 0;

	virtual bool SupportsComputeShaderDerivatives() = 0;
	virtual bool SupportsOpacityMicromap() = 0;
	virtual bool unk068() = 0;
};

#endif // IRENDERHARDWARECONFIG_H
