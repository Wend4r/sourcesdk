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
#include "tier0/utlvector.h"
#include "bitmap/imageformat_declarations.h"

struct RenderAdapterInfo_t;
enum RenderMultisampleType_t : uint8;

// The render system modules, named by IRenderHardwareConfig::RenderSystemTypeToString ("None", "DX11", "Empty",
// "Vulkan") and RenderSystemTypeToCommandLineSwitch ("-dx11", "-empty", "-vulkan")
enum RenderSystemType_t
{
	RENDER_SYSTEM_TYPE_NONE = 0,
	RENDER_SYSTEM_TYPE_DX11 = 2,
	RENDER_SYSTEM_TYPE_EMPTY = 4,
	RENDER_SYSTEM_TYPE_VULKAN = 5,
};

// IRenderHardwareConfig::GetLowLatencyMode
enum RenderLowLatencyMode_t
{
	RENDER_LOW_LATENCY_MODE_NONE = 0,

	// NVIDIA adapters
	RENDER_LOW_LATENCY_MODE_NVIDIA_REFLEX,

	// AMD adapters
	RENDER_LOW_LATENCY_MODE_AMD_ANTI_LAG,
};

// An entry of IRenderHardwareConfig::GetAvailableRenderSystems
struct RenderSystemInfo_t
{
	CUtlString m_Name;
	CUtlString m_CommandLineSwitch;
	RenderSystemType_t m_nType;
};

abstract_class IRenderHardwareConfig
{
public:
	// Highest anisotropic filtering level, 0 for none (16 on Vulkan)
	virtual int GetMaxAnisotropy() = 0;

	virtual int GetMaxTextureWidth() = 0;
	virtual int GetMaxTextureHeight() = 0;

	// Texture memory budget in bytes
	virtual uint64 GetTextureMemorySize() = 0;

	// Current DX level (110, 111, 120)
	virtual int GetDXSupportLevel() = 0;

	virtual int GetMaxTextureDepth() = 0;
	virtual int GetMaxViewports() = 0;
	// Bit 2 of caps +0x2A9 as detected, before the dxlevel config is applied; the Vulkan and empty adapter
	// enumerations clear it, no GPU config key sets it and nothing calls it
	virtual bool HasDetectedCaps2A9Bit2() = 0;

	// r_prefer_loop_unrolling
	virtual bool PreferLoopUnrolling() = 0;

	virtual bool ReverseDepth() = 0;
	// "setting.HasBrokenPipelineStats" of the GPU config entry matching the adapter
	virtual bool HasBrokenPipelineStats() = 0;

	virtual const RenderAdapterInfo_t &AdapterInfo() = 0;

	virtual RenderSystemType_t GetRenderSystemType() = 0;

	// The render device manager's render system type, GetRenderSystemType() when the manager set none
	virtual RenderSystemType_t GetEffectiveRenderSystemType() = 0;

	// The render system the engine offers to switch to, RENDER_SYSTEM_TYPE_NONE for none; the Vulkan module
	// recommends DX11 only with "-force_recommend_dx11" and when the current render system is not Vulkan
	virtual RenderSystemType_t GetRecommendedRenderSystemType() = 0;

	virtual const char *RenderSystemTypeToString( RenderSystemType_t nType ) = 0;
	virtual const char *RenderSystemTypeToDisplayName( RenderSystemType_t nType ) = 0;
	virtual const char *RenderSystemTypeToCommandLineSwitch( RenderSystemType_t nType ) = 0;

	virtual int GetAvailableRenderSystems( CUtlVector< RenderSystemInfo_t > &renderSystems ) = 0;

	// Search such a list, return -1 when not found
	virtual int FindRenderSystemByName( const CUtlVector< RenderSystemInfo_t > &renderSystems, const char *pName ) = 0;
	virtual int FindRenderSystemByCommandLineSwitch( const CUtlVector< RenderSystemInfo_t > &renderSystems, const char *pSwitch ) = 0;
	virtual int FindRenderSystemByType( const CUtlVector< RenderSystemInfo_t > &renderSystems, RenderSystemType_t nType ) = 0;

	virtual bool IsSubrectResolveSupported( bool bMSAA ) = 0;

	virtual bool IsMultisampleTypeSupported( ImageFormat nFormat, RenderMultisampleType_t nType ) = 0;

	// Highest supported multisample type not above nDesired
	virtual RenderMultisampleType_t GetBestSupportedMultisampleType( ImageFormat nFormat, RenderMultisampleType_t nDesired ) = 0;

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

	virtual RenderLowLatencyMode_t GetLowLatencyMode() = 0;

	// The Vulkan device sets it when it has the VK_KHR_shader_float16_int8 and VK_KHR_16bit_storage features
	virtual bool SupportsNative16BitShaderOps() = 0;

	// Bit 6 of caps +0x2AB, which CRenderDeviceMgrVulkan::Connect sets unconditionally and the empty render system
	// leaves clear; no GPU config key sets it and nothing calls it
	virtual bool HasCaps2ABBit6() = 0;
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
	// Bit 2 of caps +0x2AD, which CRenderDeviceMgrVulkan::Connect sets unconditionally and the empty render system
	// leaves clear; no GPU config key sets it and nothing calls it
	virtual bool HasCaps2ADBit2() = 0;
};

#endif // IRENDERHARDWARECONFIG_H
