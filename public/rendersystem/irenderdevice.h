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
#include "tier0/platwindow.h"
#include "tier0/threadtools.h"
#include "tier0/utlvector.h"
#include "tier0/utlstringtoken.h"
#include "tier1/utldelegate.h"
#include "bitmap/imageformat_declarations.h"
#include "mathlib/vector.h"
#include "mathlib/vector4d.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcesystem/resourcehandletypes.h"
#include "rendersystem/rendersystemtypes.h"
#include "rendersystem/renderdevicetypes.h"

#include <memory>
#include <stdarg.h>

class IRenderContext;
class CBufferString;
class CDisplayList;
class CDependencyDescriptor;
class CTextureDesc;
class CSamplerStateDesc;
class CRenderPipelineFrameStats;
class CPipelineFence;
class ITextureResidencyListener;
class IMaxSwapChainDimensionsChangedListener;
class IRaytraceShaderBindingTable;
class IRenderShaderByteCode;
class IRenderDevicePresentCallback;
class IReadTexturePixelsCallback;
class CTextureCreationDesc;
class CDeviceFence;
class FloatBitMap_t;
class IRenderAsyncShaderCompileListener;
class IUpscaler;

// Installed with IRenderDevice::SetTextureDataHandler: slot 0 receives the data of textures with the handler flag
// instead of their upload, slot 1 their destruction, slot 4 the texture manager on install and NULL on removal
class ITextureDataHandler;
class Vector2D;
struct RsRasterizerStateDesc_t;
struct RsDepthStencilStateDesc_t;
struct RsBlendStateDesc_t;
struct DisplayListToSubmit_t;
struct RenderTargetDesc_t;
struct RenderDescriptorDesc_t;
struct RenderDescriptorBinding_t;
struct BLASCreateDesc_t;
struct TLASCreateDesc_t;
struct TLASInstance_t;
struct RayTracePipelineStateCreateDesc_t;
struct ShaderBindingTableCreateDesc_t;
struct TextureResidencyInfo_t;
struct TextureResidencyStats_t;
struct Sheet_t;

FORWARD_DECLARE_HANDLE( OsSpecificWindowHandle_t );
FORWARD_DECLARE_HANDLE( RenderQueryHandle_t );
FORWARD_DECLARE_HANDLE( RsRasterizerStateHandle_t );
FORWARD_DECLARE_HANDLE( RsDepthStencilStateHandle_t );
FORWARD_DECLARE_HANDLE( RsBlendStateHandle_t );

using HRenderTexture = CWeakHandle< InfoForResourceTypeCTextureBase >;

typedef CUtlDelegate< void ( const void * ) > DataRecycleDelegate_t;

// A specialization constant of IRenderDevice::CreateShader
struct RenderShaderSpecializationConstant_t
{
	uint32 m_nId;

	// When set, m_nId is matched against the first key of the shader's specialization constant table and
	// replaced by the constant id stored with it; otherwise it is the constant id
	bool m_bIdIsKey;
	uint32 m_nValue;
};

// IRenderDevice::RequestTextureMipLevels
struct RenderTextureMipLevelRequest_t
{
	HRenderTexture m_hTexture;
	uint16 m_nRequiredMipSize;
};

// Filled by IRenderDevice::GetDeviceMemoryTextureAllocationInfo and GetDeviceMemoryBufferAllocationInfo from the
// backend allocation
struct RenderDeviceMemoryAllocationInfo_t
{
	void *m_pDeviceMemory;
	uint64 m_nOffset;
	uint64 m_nSize;
};

// RenderIndirectCommandsToken_t::m_nType, translated to VkIndirectCommandsTokenTypeEXT
enum RenderIndirectCommandsTokenType_t
{
	RENDER_INDIRECT_COMMANDS_TOKEN_EXECUTION_SET = 0,
	RENDER_INDIRECT_COMMANDS_TOKEN_PUSH_CONSTANT,
	RENDER_INDIRECT_COMMANDS_TOKEN_SEQUENCE_INDEX,
	RENDER_INDIRECT_COMMANDS_TOKEN_INDEX_BUFFER,
	RENDER_INDIRECT_COMMANDS_TOKEN_VERTEX_BUFFER,
	RENDER_INDIRECT_COMMANDS_TOKEN_DRAW_INDEXED,
	RENDER_INDIRECT_COMMANDS_TOKEN_DRAW,
	RENDER_INDIRECT_COMMANDS_TOKEN_DRAW_INDEXED_COUNT,
	RENDER_INDIRECT_COMMANDS_TOKEN_DRAW_COUNT,
	RENDER_INDIRECT_COMMANDS_TOKEN_DISPATCH,
	RENDER_INDIRECT_COMMANDS_TOKEN_TRACE_RAYS,
};

// A token of IRenderDevice::CreateIndirectCommandsLayout
struct RenderIndirectCommandsToken_t
{
	RenderIndirectCommandsTokenType_t m_nType;

	// Token type specific dwords, NULL for the types without data
	const uint32 *m_pData;
	uint32 m_nOffset;
};

// OpacityMicromapUsage_t::m_nFormat, the VkOpacityMicromapFormatEXT values
enum RenderOpacityMicromapFormat_t : uint16
{
	RENDER_OPACITY_MICROMAP_FORMAT_2_STATE = 1,
	RENDER_OPACITY_MICROMAP_FORMAT_4_STATE,
};

// An entry of OpacityMicromapCreateDesc_t, copied into a VkMicromapUsageEXT
struct OpacityMicromapUsage_t
{
	uint32 m_nCount;
	uint32 m_nSubdivisionLevel;
	RenderOpacityMicromapFormat_t m_nFormat;
};

// IRenderDevice::CreateOpacityMicromap reads the usages and the inline debug name ("OMM %s"); its length is not
// determinable because nothing in the available builds calls CreateOpacityMicromap
struct OpacityMicromapCreateDesc_t
{
	int m_nUsageCount;
	const OpacityMicromapUsage_t *m_pUsages;
	uint8 m_pad010[ 8 ];
	char m_pDebugName[ 1 ];
};

// Returned by IRenderDevice::GetVariableRefreshRateStatus
enum RenderVariableRefreshRateStatus_t : int
{
	RENDER_VRR_STATUS_UNKNOWN = 0,
	RENDER_VRR_STATUS_UNAVAILABLE,
	RENDER_VRR_STATUS_INACTIVE,
	RENDER_VRR_STATUS_ACTIVE,
};

// Shader stages of IRenderDevice::CreateShader and the IRenderContext binding slots; the Vulkan device creates
// the shaders for the fragment, vertex, geometry, compute, mesh and ray tracing stages
enum RenderShaderType_t
{
	// Programs without a render shader (libmaterialsystem2 maps its features program to it)
	RENDER_SHADER_TYPE_INVALID = -1,
	RENDER_SHADER_TYPE_PIXEL = 0,
	RENDER_SHADER_TYPE_VERTEX,
	RENDER_SHADER_TYPE_GEOMETRY,
	RENDER_SHADER_TYPE_COMPUTE,
	RENDER_SHADER_TYPE_MESH,
	RENDER_SHADER_TYPE_RAYTRACING = 6,
};

// nRenderContextFlags of IRenderDevice::CreateRenderContext and Present
#define RENDER_CONTEXT_SOFTWARE_COMMAND_LISTS ( 1 << 0 ) // records into software command lists
#define RENDER_CONTEXT_SIMULTANEOUS_USE ( 1 << 1 ) // the command buffer may be submitted more than once
#define RENDER_CONTEXT_ASYNC_COMPUTE ( 1 << 2 ) // records for and submits to the async compute queue
#define RENDER_CONTEXT_HOLD_TEXTURE_LOCK ( 1 << 4 ) // holds the texture manager read lock while recording
#define RENDER_CONTEXT_NO_SOFTWARE_COMMAND_LISTS ( 1 << 5 ) // ignores the device's software command list default

// IRenderDevice::SetRenderDebugFlags bits
#define RENDERDEBUGFLAG_ENABLE_GPU_TIMESTAMPS ( 1 << 4 )

// Records the per-texture streaming state the texture manager reports
#define RENDERDEBUGFLAG_TRACK_TEXTURE_STREAMING ( 1 << 5 )

// nFlags of IRenderDevice::CreateDescriptorSet
#define RENDER_DESCRIPTOR_SET_CREATE_NO_TEXTURE_TRACKING ( 1 << 0 ) // keeps no references to the bound textures
#define RENDER_DESCRIPTOR_SET_CREATE_REGISTER_SHADER_LAYOUT ( 1 << 1 ) // for GetRegisteredDescriptorSetLayout

// IRenderDevice::PushRenderDeviceFlags bits (reference counted per bit)
#define RENDER_DEVICE_FLAG_TEXTURE_STREAMING_BUDGET_BOOST ( 1 << 1 ) // raises the texture streaming budget while set

// nFlags of IRenderDevice::InitDevice and IRenderDeviceMgr::CreateDevice
#define RENDER_DEVICE_INIT_ALLOW_WINDOW_RESIZING ( 1 << 0 ) // returned by IsWindowResizingEnabled

// nBindFlags of IRenderDevice::AllocatePooledVertexBuffer, the D3D11_BIND_* values
#define RENDER_POOLED_BUFFER_BIND_VERTEX_BUFFER ( 1 << 0 )
#define RENDER_POOLED_BUFFER_BIND_UNORDERED_ACCESS ( 1 << 7 )

// IRenderDevice::CreateQueryObject
enum RenderQueryType_t
{
	// Begun with the precise flag
	RENDER_QUERY_TYPE_OCCLUSION = 1,
	RENDER_QUERY_TYPE_TIMESTAMP,
	RENDER_QUERY_TYPE_PIPELINE_STATISTICS,
	RENDER_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE,
};

// IRenderDevice::GetDeviceSpecificInfo on the Vulkan device
enum RenderDeviceSpecificInfo_t
{
	RENDER_DEVICE_INFO_VK_INSTANCE = 4,
	RENDER_DEVICE_INFO_VK_GET_INSTANCE_PROC_ADDR,
	RENDER_DEVICE_INFO_VK_PHYSICAL_DEVICE,
	RENDER_DEVICE_INFO_VK_DEVICE,
	RENDER_DEVICE_INFO_VK_GRAPHICS_QUEUE,

	// Pointers to the device's VkPhysicalDeviceMemoryProperties, graphics queue family index and
	// VkPhysicalDeviceRayTracingPipelinePropertiesKHR
	RENDER_DEVICE_INFO_VK_MEMORY_PROPERTIES,
	RENDER_DEVICE_INFO_VK_GRAPHICS_QUEUE_FAMILY_INDEX,
	RENDER_DEVICE_INFO_VK_RAY_TRACING_PIPELINE_PROPERTIES,
};

// IRenderDevice::SetLatencyMarker, passed on as the NVIDIA Reflex latency markers
enum RenderLatencyMarker_t
{
	RENDER_LATENCY_MARKER_SIMULATION_START = 0,
	RENDER_LATENCY_MARKER_SIMULATION_END,
	RENDER_LATENCY_MARKER_RENDERSUBMIT_START,
	RENDER_LATENCY_MARKER_RENDERSUBMIT_END,
	RENDER_LATENCY_MARKER_PRESENT_START,
	RENDER_LATENCY_MARKER_PRESENT_END,
	RENDER_LATENCY_MARKER_INPUT_SAMPLE,

	// Sent only while r_low_latency_trigger_flash is set
	RENDER_LATENCY_MARKER_TRIGGER_FLASH,
	RENDER_LATENCY_MARKER_PC_LATENCY_PING,
};

// IRenderDevice::DefragmentDeviceMemory, as r_vma_defrag_algorithm
enum RenderDefragAlgorithm_t : uint8
{
	RENDER_DEFRAG_ALGORITHM_FAST = 0,
	RENDER_DEFRAG_ALGORITHM_BALANCED,
	RENDER_DEFRAG_ALGORITHM_FULL,
	RENDER_DEFRAG_ALGORITHM_EXTENSIVE,
};

// Conversion of the read pixels in IRenderDevice::ReadTexturePixels; 1 converts like RENDER_GAMMA_LINEAR
enum RenderGammaType_t : uint8
{
	RENDER_GAMMA_LINEAR = 0,
	RENDER_GAMMA_SRGB = 2,
	RENDER_GAMMA_1_8,
	RENDER_GAMMA_2_0,
	RENDER_GAMMA_2_2,
	RENDER_GAMMA_1_0,
};

abstract_class IRenderDevice
{
public:
	virtual RenderDeviceAPI_t GetDeviceAPI() = 0;

	virtual SwapChainHandle_t CreateSwapChain( PlatWindow_t hPlatWnd, OsSpecificWindowHandle_t hOsWnd, const RenderDeviceInfo_t &info, const char *pDebugName ) = 0;
	virtual void DestroySwapChain( SwapChainHandle_t hSwapChain ) = 0;

	// Applies a new mode to the swap chain
	virtual bool UpdateSwapChain( SwapChainHandle_t hSwapChain, const RenderDeviceInfo_t &mode ) = 0;

	virtual const RenderDeviceInfo_t &GetSwapChainInfo( SwapChainHandle_t hSwapChain ) = 0;
	virtual OsSpecificWindowHandle_t GetSwapChainOsSpecificWindow( SwapChainHandle_t hSwapChain ) = 0;
	virtual PlatWindow_t GetSwapChainPlatWindow( SwapChainHandle_t hSwapChain ) = 0;
	virtual ImageFormat GetBackBufferFormat( SwapChainHandle_t hSwapChain ) = 0;
	virtual void GetBackBufferDimensions( SwapChainHandle_t hSwapChain, int *pWidth, int *pHeight ) = 0;
	virtual int GetCurrentAdapter() = 0;
	virtual bool IsUsingGraphics() = 0;

	// Prints the kind of render device
	virtual void SpewDriverInfo() = 0;

	virtual bool IsWindowResizingEnabled() = 0;

	virtual const char *GetShaderModelString( RenderShaderType_t nShaderType ) = 0;

	virtual void BeginRendering() = 0;
	virtual void BeginSubmittingDisplayLists( SwapChainHandle_t hSwapChain ) = 0;
	virtual bool Present( SwapChainHandle_t hSwapChain, IRenderDevicePresentCallback *pCallback, uint32 nRenderContextFlags, RenderSemaphoreHandle_t hWaitSemaphore ) = 0;
	virtual void Flush() = 0;
	virtual void ForceFlushGPU( SwapChainHandle_t hSwapChain ) = 0;
	virtual void SetHardwareGammaRamp( SwapChainHandle_t hSwapChain, float flGamma ) = 0;
	virtual std::unique_ptr< IRenderShaderByteCode > CompileShader( const char *pProgram, uint32 nBufLen, const char *pShaderVersion ) = 0;
	virtual RenderShaderHandle_t CreateShader( RenderShaderType_t nType, std::unique_ptr< IRenderShaderByteCode > pShader, const char *pDebugName, int nSpecializationConstantCount, const RenderShaderSpecializationConstant_t *pSpecializationConstants ) = 0;
	virtual void DestroyShader( RenderShaderType_t nShaderType, RenderShaderHandle_t hShader ) = 0;

	// Register counts and binary size of a compiled shader
	virtual void GetShaderStats( RenderShaderStats_t *pOutStats, RenderShaderType_t nType, RenderShaderHandle_t hShader ) = 0;

	virtual RenderInputLayout_t CreateInputLayout( const char *pName, int nFields, const RenderInputLayoutField_t *pFields ) = 0;
	virtual void AddRefInputLayout( RenderInputLayout_t hLayout ) = 0;
	virtual void ReleaseInputLayout( RenderInputLayout_t hLayout ) = 0;
	virtual void ConcatentateInputLayouts( int nOutputLayoutVariations, RenderInputLayout_t *pOutputLayouts, int nInputLayouts, const RenderInputLayout_t *pInputLayouts ) = 0;
	virtual int GetInputLayoutFields( RenderInputLayout_t hLayout, const RenderInputLayoutField_t **ppFields, int *pCount ) = 0;
	virtual CThreadFastMutex &GetInputLayoutMutex() = 0;

	// Cached render states, created on first use
	virtual RsRasterizerStateHandle_t FindOrCreateRasterizerState( const RsRasterizerStateDesc_t *pDesc ) = 0;
	virtual RsDepthStencilStateHandle_t FindOrCreateDepthStencilState( const RsDepthStencilStateDesc_t *pDesc ) = 0;
	virtual RsBlendStateHandle_t FindOrCreateBlendState( const RsBlendStateDesc_t *pDesc ) = 0;

	// Overloads in declaration order; MSVC groups them in reverse
	virtual VertexBufferHandle_t CreateVertexBuffer( int nType, const BufferDesc_t &desc, RenderBufferFlags_t nExtraUsageFlags ) = 0;
	virtual VertexBufferHandle_t CreateVertexBuffer( int nType, const BufferDesc_t &desc, int nLayoutFields, const RenderInputLayoutField_t *pLayoutFields, RenderBufferFlags_t nExtraUsageFlags ) = 0;
	virtual VertexBufferHandle_t CreateVertexBuffer( int nType, const BufferDesc_t &desc, RenderInputLayout_t hLayout, RenderBufferFlags_t nExtraUsageFlags ) = 0;

	virtual void DestroyVertexBuffer( VertexBufferHandle_t hVertexBuffer ) = 0;
	virtual IndexBufferHandle_t CreateIndexBuffer( int nType, const BufferDesc_t &desc, RenderBufferFlags_t nExtraUsageFlags ) = 0;
	virtual void DestroyIndexBuffer( IndexBufferHandle_t hIndexBuffer ) = 0;
	virtual void GetVertexBufferDesc( VertexBufferHandle_t hBuffer, BufferDesc_t *pDesc ) = 0;
	virtual void GetIndexBufferDesc( IndexBufferHandle_t hBuffer, BufferDesc_t *pDesc ) = 0;
	virtual void GetGPUBufferDesc( RenderBufferHandle_t hBuffer, BufferDesc_t *pDesc ) = 0;
	virtual RenderInputLayout_t GetVertexBufferInputLayout( VertexBufferHandle_t hVertexBuffer ) = 0;
	virtual RenderBufferHandle_t CreateGPUBuffer( int nType, const BufferDesc_t &desc, RenderBufferFlags_t nFlags, const void *pInitialData ) = 0;
	virtual RenderBufferHandle_t CreateGPUBufferFormatted( int nType, const BufferDesc_t &desc, RenderBufferFlags_t nFlags, ImageFormat nFormat, const void *pInitialData ) = 0;
	virtual uint64 GetGPUBufferDeviceAddress( RenderBufferHandle_t hGPUBuffer ) = 0;
	virtual void DestroyGPUBuffer( RenderBufferHandle_t hGPUBuffer ) = 0;
	virtual uint64 GetVertexBufferDeviceAddress( VertexBufferHandle_t hVertexBuffer ) = 0;
	virtual uint64 GetIndexBufferDeviceAddress( IndexBufferHandle_t hIndexBuffer ) = 0;
	virtual uint64 GetPooledVertexBufferDeviceAddress( RenderResourceHandle_t hPooledVertexBuffer ) = 0;
	virtual uint64 GetPooledIndexBufferDeviceAddress( PooledIndexBufferHandle_t hPooledIndexBuffer ) = 0;
	// bUnused is passed on to the texture upload of the render context, which the Vulkan backend ignores; every
	// caller passes false
	virtual HRenderTexture FindOrCreateTexture( const char *pResourceName, bool bIsAnonymous, const CTextureCreationDesc *pDescriptor, const CTextureDesc *pDataDesc, const void *pData, int nDataSize, bool bUnused ) = 0;
	virtual HRenderTexture GetErrorTexture( RenderTextureDimension_t nDimension ) = 0;
	virtual HRenderTexture FindOrCreateFileTexture( const char *pFileName, int nLoadMode ) = 0;
	virtual HRenderTexture FindFileTexture( ResourceId_t nId, int nLoadMode ) = 0;
	virtual HRenderTexture FindOrCreateExternalTexture( const char *pResourceName, uint64 hSharedTexture ) = 0;
	virtual bool UpdateExternalTexture( HRenderTexture hTexture, uint64 hSharedTexture ) = 0;
	virtual SamplerStateHandle_t FindOrCreateSamplerState( const CSamplerStateDesc &samplerState, const char *pDebugName ) = 0;

	virtual RenderQueryHandle_t CreateQueryObject( RenderQueryType_t nType ) = 0;

	// Returns 0 once the result is available
	virtual int GetQueryObjectData( RenderQueryHandle_t hQuery, uint64 *pData ) = 0;

	virtual void DeleteQueryObject( RenderQueryHandle_t hQuery ) = 0;

	virtual IRenderContext *CreateRenderContext( uint32 nRenderContextFlags, const char *pDebugStringFmt, ... ) = 0;
	virtual IRenderContext *VaCreateRenderContext( uint32 nRenderContextFlags, const char *pDebugStringFmt, va_list *pArgList ) = 0;
	virtual void ReleaseRenderContext( IRenderContext *pRenderContext ) = 0;
	virtual void SubmitDisplayLists( const DisplayListToSubmit_t *pLists, int nCount ) = 0;
	virtual void DiscardDisplayList( CDisplayList *pCommandList ) = 0;
	virtual CDependencyDescriptor *GetDependencyDescriptor( int nNumBatchesWhichWillBeSubmitted, const char *pDebugString ) = 0;
	virtual ConstantBufferHandle_t CreateConstantBuffer( int nType, uint32 nNumBytes, void *pData, const char *pDebugName ) = 0;
	virtual ConstantBufferHandle_t CreateMultiViewConstantBuffer( int nType, uint32 nNumBytes, uint32 nViews, uint32 nCreateFlags, const char *pDebugName ) = 0;
	virtual void DestroyConstantBuffer( ConstantBufferHandle_t hConstantBuffer ) = 0;
	virtual uint32 GetConstantBufferSize( ConstantBufferHandle_t hConstantBuffer ) = 0;
	virtual uint32 GetMultiViewConstantBufferSizes( ConstantBufferHandle_t hConstantBuffer, uint32 *pViewBytes, uint32 *pNumViews ) = 0;
	virtual void ForceDeviceLost() = 0;

	// Overloads in declaration order; MSVC groups them in reverse
	virtual bool ReadTexturePixels( HRenderTexture hTexture, Rect_t *pSrcRect, int nSrcSlice, int nSrcMip, Rect_t *pDstRect, void *pData, ImageFormat dstFormat, int nDstStride ) = 0;
	virtual bool ReadTexturePixels( FloatBitMap_t *pBitmap, HRenderTexture hTexture, RenderGammaType_t nGammaType ) = 0;

	virtual bool ReadBuffer( RenderBufferHandle_t hBuffer, uint32 nOffsetInBytes, void *pBuf, uint32 nBytesToRead ) = 0;
	virtual bool IsPolledBufferReadbackReady( void *hRequest ) = 0;
	virtual bool GetPolledBufferReadbackData( void *hRequest, void **ppData, int *pSize ) = 0;
	virtual bool ReleasePolledBufferReadback( void *hRequest ) = 0;
	virtual const Sheet_t *GetTextureSheet( HRenderTexture hTexture ) = 0;

	virtual void SetRenderDebugFlags( uint32 nFlags, bool bSet ) = 0;
	virtual bool IsRenderDebugFlagSet( uint32 nFlags ) = 0;
	virtual uint32 GetRenderDebugFlags() = 0;

	// Frame pacing target, 0 disables it
	virtual void SetFramePacingTargetFPS( double flFPS ) = 0;

	virtual double GetFramePacingTargetFPS() = 0;
	virtual void PreloadTextureBits( HRenderTexture hTexture, int32 nRequiredMipSize ) = 0;
	virtual void UnloadTextureBits( HRenderTexture hTexture ) = 0;
	virtual bool AreTextureBitsLoaded( HRenderTexture hTexture ) = 0;
	virtual void SetTextureStreamingPriority( HRenderTexture hTexture, int nPriority ) = 0;
	virtual const CTextureDesc *GetOnDiskTextureDesc( HRenderTexture hTexture ) = 0;
	virtual const CTextureDesc *GetTextureDesc( HRenderTexture hTexture ) = 0;
	virtual Vector GetTextureAverageColor( HRenderTexture hTexture ) = 0;
	virtual Vector4D GetTextureAverageColorRGBA( HRenderTexture hTexture ) = 0;
	virtual bool GetCubemapRadianceSHConstants( HRenderTexture hTexture, int nArrayIndex, CUtlVector< float > &outConstants ) = 0;
	virtual void SetCubemapRadianceSHConstants( HRenderTexture hTexture, int nArrayIndex, const CUtlVector< float > &constants ) = 0;
	virtual size_t GetTextureStreamingDataOffset( HRenderTexture hTexture ) = 0;
	virtual RenderMultisampleType_t GetTextureMultisampleType( HRenderTexture hTexture ) = 0;
	virtual ImageFormat GetSwapChainDepthStencilFormat( SwapChainHandle_t hSwapChain ) = 0;
	virtual bool IsTextureSampleableRenderTarget( HRenderTexture hTexture ) = 0;
	virtual const char *MultisampleTypeToString( RenderMultisampleType_t nType ) = 0;
	virtual bool IsTextureRenderTarget( HRenderTexture hTexture ) = 0;
	virtual void PinTextureInGPUMemory( HRenderTexture hTexture ) = 0;
	virtual void UnpinTextureInGPUMemory( HRenderTexture hTexture ) = 0;
	virtual bool IsTextureCubeMap( HRenderTexture hTexture ) = 0;
	virtual bool IsTextureFullyResident( HRenderTexture hTexture ) = 0;
	virtual bool IsTextureStandIn( HRenderTexture hTexture ) = 0;
	// "Setting max texture resolution to: %dx%d", clamped to the maximum texture width and height of the hardware
	virtual int GetMaxTextureResolution() = 0;
	virtual void AddTextureResidencyListener( ITextureResidencyListener *pListener ) = 0;
	virtual void RemoveTextureResidencyListener( ITextureResidencyListener *pListener ) = 0;
	virtual void AddAsyncShaderCompileListener( IRenderAsyncShaderCompileListener *pListener ) = 0;
	virtual void RemoveAsyncShaderCompileListener( IRenderAsyncShaderCompileListener *pListener ) = 0;
	virtual HRenderTexture GetSwapChainTexture( SwapChainHandle_t hSwapChain, int nBuffer ) = 0;
	virtual void GetRenderTargetDescDimensions( const RenderTargetDesc_t &desc, int *pWidthOut, int *pHeightOut, RenderMultisampleType_t *pMultisampleTypeOut ) = 0;
	virtual bool GetPipelineFrameStats( CRenderPipelineFrameStats &frameStats ) = 0;
	virtual void ReleasePipelineFrameStats( CRenderPipelineFrameStats &frameStats ) = 0;
	virtual bool IsTextureSwapChain( HRenderTexture hTexture ) = 0;
	virtual bool GetSwapChainFromTexture( HRenderTexture hTexture, SwapChainHandle_t *pSwapChain, int *pBuffer ) = 0;
	virtual ImageFormat GetNearestSupportedColorFormat( ImageFormat nFormat, bool bIsRenderTarget, bool bIsVertexTexture, bool bIsFilterableRequired ) = 0;
	virtual uint32 ComputeTextureMemorySize( const CTextureDesc &desc ) = 0;
	virtual void GetTextureResidencyInfo( CUtlVector< TextureResidencyInfo_t > &textures, TextureResidencyStats_t &textureResidencyStats ) = 0;
	virtual bool GetSheetSequenceTextureData( HRenderTexture hTexture, HRenderTexture *pTextureOut, Vector2D *pvecUVOut, int *pSequenceCount ) = 0;
	virtual void GetSheetCacheTextureSize( int &nWidth, int &nHeight ) = 0;
	virtual void CommitSheetCacheData() = 0;
	virtual HRenderTexture GetSheetMetaDataTexture() = 0;
	virtual void UnThrottleTextureStreamingForNFrames( uint32 nNumberOfFramesForUnthrottledTextureLoading ) = 0;
	virtual int GetNumTextureLoadsInFlight() = 0;
	virtual void OnBeginningOfClientShutdown() = 0;

	virtual void AppendResourceStats( CBufferString *pOut ) = 0;

	virtual void *GetDeviceSpecificInfo( RenderDeviceSpecificInfo_t nInfo ) = 0;
	virtual bool CanRenderToSwapChain( SwapChainHandle_t hSwapChain ) = 0;
	virtual void *GetDeviceSpecificTexture( HRenderTexture hTexture, RenderTextureDimension_t nDimension ) = 0;
	virtual void *GetDeviceSpecificImageView( HRenderTexture hTexture, RenderTextureDimension_t nDimension, RenderColorSpace_t nColorSpace ) = 0;

	// Overloads in declaration order; MSVC groups them in reverse
	virtual void *GetDeviceSpecificBuffer( RenderBufferHandle_t hBuffer ) = 0;
	virtual void *GetDeviceSpecificBuffer( IndexBufferHandle_t hBuffer ) = 0;
	virtual void *GetDeviceSpecificBuffer( VertexBufferHandle_t hBuffer ) = 0;

	virtual void *GetDeviceSpecificDescriptorSet( RenderDescriptorSetHandle_t hDescriptorSet ) = 0;
	virtual void *GetDeviceSpecificRayTracePipeline( RenderRayTracePipelineHandle_t hRayTracePipeline ) = 0;
	virtual void *GetDeviceSpecificCommandBuffer( IRenderContext *pRenderContext ) = 0;
	virtual void RegisterMaxSwapChainDimensionsChangedListener( IMaxSwapChainDimensionsChangedListener *pCallback ) = 0;
	virtual void UnregisterMaxSwapChainDimensionsChangedListener( IMaxSwapChainDimensionsChangedListener *pCallback ) = 0;
	virtual void GetCurrentMaxSwapChainDimensions( int *pWidthOut, int *pHeightOut ) = 0;
	virtual ThreadHandle_t GetRenderThreadHandle() = 0;
	virtual ThreadId_t GetRenderThreadId() = 0;
	virtual bool HasOutstandingPresents( SwapChainHandle_t hSwapChain ) = 0;
	virtual void WaitForOutstandingPresents( SwapChainHandle_t hSwapChain ) = 0;
	virtual void ResizeEngineWindow( SwapChainHandle_t hSwapChain, PlatWindow_t hEngineWindow, int nMonitorIndex, const RenderDeviceInfo_t &actualMode ) = 0;
	virtual int GetLastValidSheetCacheFrame() = 0;
	virtual int GetTexturePinCount( HRenderTexture hTexture ) = 0;
	virtual RenderDescriptorSetHandle_t CreateDescriptorSet( const char *pName, int32 nNumDescriptors, const RenderDescriptorDesc_t *pDescriptorDescs, int32 nNumBindings, const RenderDescriptorBinding_t *pDescriptorBindings, uint8 nFlags ) = 0;
	virtual void UpdateDescriptorSet( RenderDescriptorSetHandle_t hDescriptorSet, int32 nNumBindings, const RenderDescriptorBinding_t *pDescriptorBindings ) = 0;
	virtual void AllocateUpdateAfterBindDescriptorSet( RenderDescriptorSetHandle_t hDescriptorSet ) = 0;
	// UpdateDescriptorSet() of a set allocated by AllocateUpdateAfterBindDescriptorSet(), without allocating it again
	virtual void UpdateUpdateAfterBindDescriptorSet( RenderDescriptorSetHandle_t hDescriptorSet, int32 nNumBindings, const RenderDescriptorBinding_t *pDescriptorBindings ) = 0;
	virtual void DestroyDescriptorSet( RenderDescriptorSetHandle_t hDescriptorSet ) = 0;
	virtual bool GetDescriptorSetDescs( RenderDescriptorSetHandle_t hDescriptorSet, CUtlVector< RenderDescriptorDesc_t > &descriptorDescs ) = 0;
	virtual bool AddDescriptorsToGlobalPerFrameDescriptorSet( int32 nNumDescriptors, const RenderDescriptorDesc_t *pDescriptorDescs ) = 0;
	virtual void UpdateGlobalPerFrameDescriptorBindings( int32 nNumBindings, const RenderDescriptorBinding_t *pDescriptorBindings ) = 0;

	// Multi-GPU state
	virtual bool IsMultiGPUEnabled() = 0;
	virtual int GetMultiGPUCount() = 0;
	virtual uint32 GetMultiGPUDeviceMask() = 0;
	// 1 / ( GetMultiGPUCount() / 2 ) with multi-GPU enabled, 1 otherwise
	virtual float GetMultiGPUFrameFactor() = 0;

	virtual float GetLastPresentTime() = 0;
	// pDataRecycleDelegate is invoked with pData and pRecycleContext once the upload has consumed the data; the
	// Vulkan backend does not read nTextureUpdateFlags
	virtual void AsyncSetTextureData( HRenderTexture hTexture, const CTextureDesc *pDataDesc, const void *pData, int nDataSize, bool bUnused, int nSpecificMipLevelToSet, const Rect3D_t *pSubRectToUpdate, uint32 nTextureUpdateFlags, const DataRecycleDelegate_t *pDataRecycleDelegate, void *pRecycleContext ) = 0;
	virtual void *GetGraphicsAPISpecificTextureHandle( HRenderTexture hTexture ) = 0;
	virtual void GetLargestMonitorDimensions( int *pWidth, int *pHeight ) = 0;
	virtual void PushRenderDeviceFlags( uintp nFlags ) = 0;
	virtual void PopRenderDeviceFlags( uintp nFlags ) = 0;

	// Needs RENDERDEBUGFLAG_ENABLE_GPU_TIMESTAMPS
	virtual bool GetGPUFrameTimeMS( SwapChainHandle_t hSwapChain, float *pGPUFrameTimeMSOut, uint32 *pFrameNumberOut ) = 0;

	virtual int GetPresentCount() = 0;
	virtual bool IsUsing32BitDepthBuffer() = 0;
	virtual void LowLatencySleep() = 0;
	virtual void SetLatencyMarker( RenderLatencyMarker_t nMarker ) = 0;
	virtual bool IsLatencyStatsPingMessage( uint32 nWindowMessage ) = 0;
	virtual bool IsFramePacingSupported() = 0;
	virtual double ConvertTimestampToSeconds( uint64 nTimestamp ) = 0;
	virtual void FreeTextureGPUResources( int32 nTextureCount, const HRenderTexture *pTextures ) = 0;
	virtual bool IsProfilerAttached() = 0;
	virtual void SetTextureGlobalMipLODBias( float flMipLODBias ) = 0;
	virtual float GetTextureGlobalMipLODBias() = 0;
	virtual void UnThrottlePipelineCreationForNFrames( uint32 nNumberOfFramesForUnthrottledPipelineCreation ) = 0;
	virtual RenderDescriptorSetHandle_t GetRegisteredDescriptorSetLayout( CUtlStringToken descriptorSetName ) = 0;
	virtual RenderBLASHandle_t CreateBLAS( const BLASCreateDesc_t &desc, uint32 *pSizeCounter ) = 0;
	virtual bool IsCompactible( RenderBLASHandle_t hBLAS ) = 0;
	virtual uint64 GetBLASUpdateScratchSize( RenderBLASHandle_t hBLAS ) = 0;
	virtual IsReadyToCompactResult_t IsReadyToCompact( RenderBLASHandle_t hBLAS ) = 0;
	virtual void DestroyBLAS( RenderBLASHandle_t hBLAS ) = 0;
	virtual RenderTLASHandle_t CreateTLAS( const TLASCreateDesc_t &desc, TLASInstance_t *pInstances, int nInstanceCount ) = 0;
	virtual void DestroyTLAS( RenderTLASHandle_t hTLAS ) = 0;
	virtual uint64 GetAccelerationStructureUpdateScratchSize( RenderResourceHandle_t hAccelerationStructure ) = 0;
	virtual RenderRayTracePipelineHandle_t CreateRayTracePipeline( const RayTracePipelineStateCreateDesc_t &desc ) = 0;
	virtual void DestroyRayTracePipeline( RenderRayTracePipelineHandle_t hPipeline ) = 0;
	virtual bool IsCreatingRayTracePipelines() = 0;
	virtual IRaytraceShaderBindingTable *CreateRayTraceShaderBindingTable( const ShaderBindingTableCreateDesc_t &desc ) = 0;
	virtual void DestroyRayTraceShaderBindingTable( IRaytraceShaderBindingTable *pSBT ) = 0;
	virtual RenderResourceHandle_t CreateOpacityMicromap( const OpacityMicromapCreateDesc_t *pDesc ) = 0;
	virtual void DestroyOpacityMicromap( RenderResourceHandle_t hOpacityMicromap ) = 0;
	// buildScratchSize of the VkMicromapBuildSizesInfoEXT queried when the micromap was created
	virtual uint64 GetOpacityMicromapBuildScratchSize( RenderResourceHandle_t hOpacityMicromap ) = 0;
	virtual bool IsRayTracingSupported() = 0;
	virtual bool IsAsyncPresentEnabled() = 0;
	virtual void EnableAsyncPresent( bool bEnable ) = 0;
	virtual bool StartRenderDocCapture( PlatWindow_t hWnd ) = 0;
	virtual bool EndRenderDocCapture( PlatWindow_t hWnd, int ( *pfnCaptureCallback )( const char *pCaptureFilePath, void *pRenderDocAPI, void *pContext ), void *pContext ) = 0;
	virtual void RequestTextureMipLevels( int nCount, const RenderTextureMipLevelRequest_t *pTextureRequests ) = 0;
	virtual uint32 GetTextureViewIndex( HRenderTexture hTexture, RenderColorSpace_t nColorSpace, RenderTextureDimension_t nDimension ) = 0;
	virtual HRenderTexture GetTextureFromViewIndex( uint32 nTextureViewIndex ) = 0;
	virtual uint32 GetSamplerIndex( SamplerStateHandle_t hSamplerState ) = 0;
	virtual RenderDescriptorSetHandle_t GetGlobalLateBoundBindlessDescriptorSet() = 0;
	virtual PooledIndexBufferHandle_t AllocatePooledIndexBuffer( uint nIndexCount, uint nIndexSize, const void *pIndices, uint *pOutStartIndex ) = 0;
	virtual void FreePooledIndexBuffer( PooledIndexBufferHandle_t hBuffer ) = 0;
	virtual uint GetPooledIndexBufferOffset( PooledIndexBufferHandle_t hBuffer, uint32 *pIndexSizeOut, uint32 *pIndexCountOut ) = 0;
	virtual IndexBufferHandle_t GetPooledIndexBuffer() = 0;
	// nBindFlags: RENDER_POOLED_BUFFER_BIND_* bits, ignored by the Vulkan backend
	virtual RenderResourceHandle_t AllocatePooledVertexBuffer( uint nVertexCount, uint nVertexSize, const void *pVertices, uint32 nBindFlags, const char *pDebugName ) = 0;
	virtual void FreePooledVertexBuffer( RenderResourceHandle_t hBuffer ) = 0;
	virtual uint32 GetPooledVertexBufferBaseVertex( RenderResourceHandle_t hPooledVertexBuffer ) = 0;
	virtual uint32 GetPooledVertexBufferVertexCount( RenderResourceHandle_t hPooledVertexBuffer, uint32 *pVertexSizeOut ) = 0;
	virtual VertexBufferHandle_t GetPooledVertexBuffer() = 0;
	// Calls vkCreateShaderModule with a wrong structure type and no code, provoking a validation error
	virtual void TriggerValidationError() = 0;
	virtual IUpscaler *CreateUpscaler( UpscalerType_t nType ) = 0;

	// XeSS / FidelityFX availability
	virtual bool IsSuperSamplingAvailable( UpscalerType_t nType ) = 0;

	virtual void DestroyUpscaler( IUpscaler *pUpscaler ) = 0;
	// What vulkan_vma_defrag starts
	virtual void DefragmentDeviceMemory( RenderDefragAlgorithm_t nAlgorithm, bool bAsync ) = 0;
	virtual void BeginRenderingToSwapChain( SwapChainHandle_t hSwapChain ) = 0;
	// Draws the visualizer while r_show_gpu_memory_visualizer is set and clears the convar when drawing fails; the
	// arguments go to a drawing routine that is a stub in the shipped builds and reads none of them
	virtual void DrawGPUMemoryVisualizer( void *pUnused1, void *pUnused2, void *pUnused3 ) = 0;

	virtual void *CreateDeviceMemoryPool( uint64 nMinBlockSize ) = 0;
	virtual void DestroyDeviceMemoryPool( void *hPool ) = 0;
	virtual bool AllocateTextureInPool( void *hPool, HRenderTexture hTexture ) = 0;
	virtual void FreeTextureFromPool( void *hPool, HRenderTexture hTexture ) = 0;
	virtual bool AllocateBufferInPool( void *hPool, RenderBufferHandle_t hBuffer ) = 0;
	virtual void FreeBufferFromPool( void *hPool, RenderBufferHandle_t hBuffer ) = 0;
	virtual bool GetDeviceMemoryTextureAllocationInfo( HRenderTexture hTexture, RenderDeviceMemoryAllocationInfo_t *pInfoOut ) = 0;
	virtual bool GetDeviceMemoryBufferAllocationInfo( RenderBufferHandle_t hBuffer, RenderDeviceMemoryAllocationInfo_t *pInfoOut ) = 0;
	// Binds every texture of pTextures to the device memory of hTexture
	virtual bool AliasTexturesToTextureMemory( HRenderTexture hTexture, int nCount, const HRenderTexture *pTextures ) = 0;
	virtual uint32 CheckFormatSupport( ImageFormat nFormat ) = 0;
	// Variable refresh rate state of the swap chain output; the Vulkan backend returns RENDER_VRR_STATUS_UNKNOWN
	virtual RenderVariableRefreshRateStatus_t GetVariableRefreshRateStatus( SwapChainHandle_t hSwapChain ) = 0;

	// Device generated commands
	virtual void *CreateIndirectCommandsLayout( IRenderContext *pRenderContext, uint32 nTokenCount, const RenderIndirectCommandsToken_t *pTokens, uint32 nIndirectStride, bool bExplicitPreprocess ) = 0;
	virtual void DestroyIndirectCommandsLayout( void *hIndirectCommandsLayout ) = 0;
	virtual RenderBufferHandle_t CreateIndirectCommandsPreprocessBuffer( void *hIndirectCommandsLayout, void *hIndirectExecutionSet, uint32 nMaxSequenceCount ) = 0;
	virtual void *CreateIndirectExecutionSet( IRenderContext *pRenderContext, int nMaxPipelineCount ) = 0;
	virtual bool UpdateIndirectExecutionSet( void *hIndirectExecutionSet, IRenderContext *pRenderContext, int nPipelineIndex ) = 0;
	virtual void DestroyIndirectExecutionSet( void *hIndirectExecutionSet ) = 0;

	// Called by the engine when level loading finishes. Keeps pName, whose file name is appended to RenderDoc
	// capture names, warns about shaders that were not compiled during the load and flushes the pipeline cache
	virtual void OnLevelLoadingFinished( const char *pName ) = 0;

	// GetPresentCount() + 1
	virtual int GetNextPresentCount() = 0;

	// Frame number below which PerformQueuedTextureDeletions frees queued textures
	virtual int GetCompletedFrameNumber() = 0;

	// Pointer to the RENDERDOC_API_1_6_0 pointer, NULL when RenderDoc is not loaded
	virtual void *GetRenderDocAPI() = 0;

	// The handler gets the data of textures with a handler flag instead of the upload, and their destruction
	virtual void SetTextureDataHandler( ITextureDataHandler *pHandler ) = 0;
	virtual bool ReadCompiledTextureMips( HRenderTexture hTexture, int nSlice, int nFirstMip, int nMipCount, ImageFormat nFormat, std::shared_ptr< IReadTexturePixelsCallback > pCallback ) = 0;
	virtual void OnPresentSubmitted() = 0;
	virtual void QueueFencedRecycling( CPipelineFence *pFence ) = 0;
	virtual void OnFlushSubmitted() = 0;
	virtual void OnPresentCompleted( SwapChainHandle_t hSwapChain, int nPresentId, CDeviceFence *pDeviceFence, IRenderDevicePresentCallback *pCallback ) = 0;
	virtual void OnRenderDeviceFlagsEnabled( uintp nFlags ) = 0;
	virtual void OnRenderDeviceFlagsDisabled( uintp nFlags ) = 0;
	virtual bool InitDevice( int nAdapter, int nFlags, int nDXLevel ) = 0;
	virtual void ShutdownDevice() = 0;
	virtual bool IsDeactivated() = 0;
	virtual bool IsMultisampleTypeSupported( ImageFormat nFormat, RenderMultisampleType_t nType ) = 0;
};

#endif // IRENDERDEVICE_H
