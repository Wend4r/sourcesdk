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

#include <memory>

#include "tier0/basetypes.h"
#include "tier0/platwindow.h"
#include "tier0/threadtools.h"
#include "tier1/utlvector.h"
#include "tier0/utlstringtoken.h"
#include "mathlib/vector.h"
#include "mathlib/vector4d.h"
#include "bitmap/imageformat_declarations.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcesystem/resourcehandletypes.h"

class IRenderContext;
class IRenderDevicePresentCallback;
class IRenderShaderByteCode;
class IRaytraceShaderBindingTable;
class ITextureResidencyListener;
class IMaxSwapChainDimensionsChangedListener;
class IRenderDeviceFrameListener;
class IRenderDeviceRefCounted;
class IRenderReadCallback;
class CBufferString;
class CTextureDesc;
class CTextureCreationDesc;
class CSamplerStateDesc;
class CDisplayList;
class CDependencyDescriptor;
class CRenderPipelineFrameStats;
struct RenderDeviceInfo_t;
struct RenderShaderStats_t;
struct RenderInputLayoutField_t;
struct RsRasterizerStateDesc_t;
struct RsDepthStencilStateDesc_t;
struct RsBlendStateDesc_t;
struct BufferDesc_t;
struct ResourceId_t;
struct RenderContextCreationInfo_t;
struct DisplayListToSubmit_t;
struct RenderTargetDesc_t;
struct TextureResidencyInfo_t;
struct TextureResidencyStats_t;
struct RenderDescriptorDesc_t;
struct RenderDescriptorBinding_t;
struct DataRecycleDelegate_t;
struct BLASCreateDesc_t;
struct TLASCreateDesc_t;
struct TLASInstance_t;
struct RayTracePipelineStateCreateDesc_t;
struct ShaderBindingTableCreateDesc_t;
struct OpacityMicromapCreateDesc_t;
struct DeviceMemoryPoolCreateDesc_t;
struct DeviceMemoryAllocationInfo_t;
struct IndirectCommandsLayoutCreateDesc_t;
struct IndirectExecutionSetCreateDesc_t;
struct RenderReadRequest_t;
struct Rect_t;
struct Rect3D_t;
class Color;

using HRenderTexture = CWeakHandle< InfoForResourceTypeCTextureBase >;
using HRenderTextureStrong = CStrongHandle< InfoForResourceTypeCTextureBase >;

FORWARD_DECLARE_HANDLE( SwapChainHandle_t );
FORWARD_DECLARE_HANDLE( RenderShaderHandle_t );
FORWARD_DECLARE_HANDLE( RenderInputLayout_t );
FORWARD_DECLARE_HANDLE( RsRasterizerStateHandle_t );
FORWARD_DECLARE_HANDLE( RsDepthStencilStateHandle_t );
FORWARD_DECLARE_HANDLE( RsBlendStateHandle_t );
FORWARD_DECLARE_HANDLE( VertexBufferHandle_t );
FORWARD_DECLARE_HANDLE( IndexBufferHandle_t );
FORWARD_DECLARE_HANDLE( RenderBufferHandle_t );
FORWARD_DECLARE_HANDLE( PooledVertexBufferHandle_t );
FORWARD_DECLARE_HANDLE( PooledIndexBufferHandle_t );
FORWARD_DECLARE_HANDLE( SamplerStateHandle_t );
FORWARD_DECLARE_HANDLE( RenderQueryHandle_t );
FORWARD_DECLARE_HANDLE( ConstantBufferHandle_t );
FORWARD_DECLARE_HANDLE( RenderDescriptorSetHandle_t );
FORWARD_DECLARE_HANDLE( RenderBLASHandle_t );
FORWARD_DECLARE_HANDLE( RenderTLASHandle_t );
FORWARD_DECLARE_HANDLE( RenderRayTracePipelineHandle_t );
FORWARD_DECLARE_HANDLE( RenderOpacityMicromapHandle_t );
FORWARD_DECLARE_HANDLE( RenderDeviceMemoryPoolHandle_t );
FORWARD_DECLARE_HANDLE( RenderIndirectCommandsLayoutHandle_t );
FORWARD_DECLARE_HANDLE( RenderIndirectExecutionSetHandle_t );
FORWARD_DECLARE_HANDLE( SharedResourceHandle_t );
FORWARD_DECLARE_HANDLE( GraphicsAPISpecificTextureHandle_t );

enum RenderMultisampleType_t : uint8;
enum RenderShaderType_t : int;
enum RenderBufferType_t : int;
enum RenderBufferFlags_t : int;
enum RenderQueryType_t : int;
enum RenderQueryState_t : int;
enum RenderTextureDimension_t : int8;
enum RenderColorSpace_t : uint8;
enum RenderSystemAssetFileLoadMode_t : int;
enum RenderDebugFlag_t : int;
enum RenderDeviceFlag_t : uint64;
enum DeviceSpecificInfo_t : int;
enum ConstantBufferType_t : int;
enum RenderTextureStreamingPriority_t : uint8;
enum RenderDefragFlags_t : uint8;
enum SwapChainBuffer_t : int;
enum IsReadyToCompactResult_t : int;
enum RenderAsyncTextureUpdateFlags_t : int;

enum RenderDeviceAPI_t
{
	RENDER_DEVICE_API_DX11 = 0,
	RENDER_DEVICE_API_VULKAN,
	RENDER_DEVICE_API_EMPTY,

	RENDER_DEVICE_API_MAX
};

abstract_class IRenderDevice
{
public:
	virtual RenderDeviceAPI_t GetRenderDeviceAPI() = 0;

	// hOsWindow is the native handle of hWindow; either of the two may be null and is then derived from the other
	virtual SwapChainHandle_t CreateSwapChain( PlatWindow_t hWindow, OsSpecificWindowHandle_t hOsWindow, const RenderDeviceInfo_t &info, const char *pszName ) = 0;
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
	virtual void SpewDriverInfo() = 0;
	virtual bool IsWindowResizingEnabled() = 0;
	virtual const char *GetShaderVersionString( RenderShaderType_t nType ) = 0;
	virtual void BeginRendering() = 0;
	virtual void BeginSubmittingDisplayLists( SwapChainHandle_t hSwapChain ) = 0;

	// pCallback rides along with the present command. The present goes through a render context created
	// with nRenderContextFlags | 0x20, which first waits on hWaitSemaphore when set (Vulkan only; Direct3D 11
	// uses neither).
	virtual bool Present( SwapChainHandle_t hSwapChain, IRenderDevicePresentCallback *pCallback, uint32 nRenderContextFlags, void *hWaitSemaphore ) = 0;

	virtual void Flush() = 0;
	virtual void ForceFlushGPU( SwapChainHandle_t hSwapChain ) = 0;
	virtual void SetHardwareGammaRamp( SwapChainHandle_t hSwapChain, float flGamma, float flGammaTVRangeMin, float flGammaTVRangeMax, float flGammaTVExponent, bool bTVEnabled ) = 0;
	virtual std::unique_ptr< IRenderShaderByteCode > CompileShader( const char *pProgram, uint32 nBufLen, const char *pShaderVersion ) = 0;

	// nShaderArg0 and nShaderArg1 go untouched to the second virtual method of the created shader
	// object (named by what it does)
	virtual RenderShaderHandle_t CreateShader( RenderShaderType_t nType, std::unique_ptr< IRenderShaderByteCode > pShader, const char *pDebugName, uint32 nShaderArg0, uint64 nShaderArg1 ) = 0;

	virtual void DestroyShader( RenderShaderType_t nType, RenderShaderHandle_t hShader ) = 0;

	// Register counts and binary size of a compiled shader
	virtual void GetShaderStats( RenderShaderStats_t *pOutStats, RenderShaderType_t nType, RenderShaderHandle_t hShader ) = 0;

	virtual RenderInputLayout_t CreateInputLayout( const char *pLayoutName, int nFieldCount, const RenderInputLayoutField_t *pFieldDescs ) = 0;
	virtual void AddInputLayoutReference( RenderInputLayout_t hInputLayout ) = 0;
	virtual void DestroyInputLayout( RenderInputLayout_t hInputLayout ) = 0;
	virtual void ConcatentateInputLayouts( int nOutputLayoutVariations, RenderInputLayout_t *pOutputLayouts, int nInputLayouts, const RenderInputLayout_t *pInputLayouts ) = 0;

	// Returns the field count, also written to nFieldsOut
	virtual int GetInputLayoutFields( RenderInputLayout_t hInputLayout, const RenderInputLayoutField_t **ppFieldsOut, int &nFieldsOut ) = 0;

	virtual CThreadFastMutex &GetInputLayoutMutex() = 0;

	// Cached render states, created on first use
	virtual RsRasterizerStateHandle_t FindOrCreateRasterizerState( const RsRasterizerStateDesc_t *pRsDesc ) = 0;
	virtual RsDepthStencilStateHandle_t FindOrCreateDepthStencilState( const RsDepthStencilStateDesc_t *pDsDesc ) = 0;
	virtual RsBlendStateHandle_t FindOrCreateBlendState( const RsBlendStateDesc_t *pBlendDesc ) = 0;

	virtual VertexBufferHandle_t CreateVertexBuffer( RenderBufferType_t nType, const BufferDesc_t &desc, RenderBufferFlags_t nExtraUsageFlags ) = 0;
	virtual VertexBufferHandle_t CreateVertexBuffer( RenderBufferType_t nType, const BufferDesc_t &desc, int nLayoutFields, const RenderInputLayoutField_t *pLayoutFields, RenderBufferFlags_t nExtraUsageFlags ) = 0;
	virtual VertexBufferHandle_t CreateVertexBuffer( RenderBufferType_t nType, const BufferDesc_t &desc, RenderInputLayout_t hLayout, RenderBufferFlags_t nExtraUsageFlags ) = 0;
	virtual void DestroyVertexBuffer( VertexBufferHandle_t hVertexBuffer ) = 0;
	virtual IndexBufferHandle_t CreateIndexBuffer( RenderBufferType_t nType, const BufferDesc_t &desc, RenderBufferFlags_t nExtraUsageFlags ) = 0;
	virtual void DestroyIndexBuffer( IndexBufferHandle_t hIndexBuffer ) = 0;
	virtual void GetVertexBufferDesc( VertexBufferHandle_t hVertexBuffer, BufferDesc_t *pDesc ) = 0;
	virtual void GetIndexBufferDesc( IndexBufferHandle_t hIndexBuffer, BufferDesc_t *pDesc ) = 0;
	virtual void GetGPUBufferDesc( RenderBufferHandle_t hBuffer, BufferDesc_t *pDesc ) = 0;
	virtual RenderInputLayout_t GetVertexBufferInputLayout( VertexBufferHandle_t hVertexBuffer ) = 0;
	virtual RenderBufferHandle_t CreateGPUBuffer( RenderBufferType_t nType, const BufferDesc_t &desc, RenderBufferFlags_t nFlags, const void *pInitialData ) = 0;
	virtual RenderBufferHandle_t CreateGPUBufferFormatted( RenderBufferType_t nType, const BufferDesc_t &desc, RenderBufferFlags_t nFlags, ImageFormat nFormat, const void *pInitialData ) = 0;
	virtual uint64 GetGPUBufferDeviceAddress( RenderBufferHandle_t hBuffer ) = 0;
	virtual void DestroyGPUBuffer( RenderBufferHandle_t hBuffer ) = 0;

	// Buffer device addresses of the other buffer kinds; the pooled ones add the allocation offset to the
	// address of the pool buffer (named by what they do)
	virtual uint64 GetVertexBufferDeviceAddress( VertexBufferHandle_t hVertexBuffer ) = 0;
	virtual uint64 GetIndexBufferDeviceAddress( IndexBufferHandle_t hIndexBuffer ) = 0;
	virtual uint64 GetPooledVertexBufferDeviceAddress( PooledVertexBufferHandle_t hPooledBuffer ) = 0;
	virtual uint64 GetPooledIndexBufferDeviceAddress( PooledIndexBufferHandle_t hPooledBuffer ) = 0;

	// bUploadOption goes with the data to the render context upload call
	virtual HRenderTextureStrong FindOrCreateTexture( const char *pResourceName, bool bIsAnonymous, const CTextureCreationDesc *pCreationDesc, const CTextureDesc *pDesc, const void *pData, int nDataSize, bool bUploadOption ) = 0;

	virtual HRenderTexture GetErrorTexture( RenderTextureDimension_t nDimension ) = 0;
	virtual HRenderTextureStrong FindOrCreateFileTexture( const char *pFileName, RenderSystemAssetFileLoadMode_t nLoadMode ) = 0;
	virtual HRenderTextureStrong FindFileTexture( ResourceId_t nId, RenderSystemAssetFileLoadMode_t nLoadMode ) = 0;
	virtual HRenderTextureStrong FindOrCreateExternalTexture( const char *pResourceName, SharedResourceHandle_t hSharedTexture ) = 0;
	virtual bool UpdateExternalTexture( HRenderTexture hTexture, SharedResourceHandle_t hSharedTexture ) = 0;
	virtual SamplerStateHandle_t FindOrCreateSamplerState( const CSamplerStateDesc &samplerState, const char *pszDebugName ) = 0;
	virtual RenderQueryHandle_t CreateQueryObject( RenderQueryType_t nType ) = 0;
	virtual RenderQueryState_t GetQueryData( RenderQueryHandle_t hQueryObject, uint64 *pQueryData ) = 0;
	virtual void DeleteQueryObject( RenderQueryHandle_t hQueryObject ) = 0;

	// CreateRenderContext( nRenderContextFlags, NULL, NULL ) (named by what it does)
	virtual IRenderContext *CreateRenderContextWithFlags( uint32 nRenderContextFlags ) = 0;

	virtual IRenderContext *CreateRenderContext( uint32 nRenderContextFlags, RenderContextCreationInfo_t *pOptInfo, const char *pDebugName ) = 0;
	virtual void ReleaseRenderContext( IRenderContext *pRenderContext ) = 0;
	virtual void SubmitDisplayLists( const DisplayListToSubmit_t *pDisplayLists, int nCount ) = 0;
	virtual void DiscardDisplayList( CDisplayList *pStream ) = 0;
	virtual CDependencyDescriptor *GetDependencyDescriptor( int nNumBatchesWhichWillBeSubmitted, const char *pDebugString ) = 0;
	virtual ConstantBufferHandle_t CreateConstantBuffer( ConstantBufferType_t nType, uint32 nNumBytes, void *pData, const char *pDebugName ) = 0;
	virtual ConstantBufferHandle_t CreateMultiViewConstantBuffer( ConstantBufferType_t nType, uint32 nNumBytes, uint32 nViews, uint32 nCreateFlags, const char *pDebugName ) = 0;
	virtual void DestroyConstantBuffer( ConstantBufferHandle_t hConstantBuffer ) = 0;
	virtual uint32 GetConstantBufferSize( ConstantBufferHandle_t hConstantBuffer ) = 0;
	virtual uint32 GetMultiViewConstantBufferSizes( ConstantBufferHandle_t hConstantBuffer, uint32 *pViewBytes, uint32 *pNumViews ) = 0;
	virtual void ForceDeviceLost() = 0;
	virtual bool ReadTexturePixels( HRenderTexture hTexture, Rect_t *pSrcRect, int nFace, int nMip, Rect_t *pDstRect, void *pData, ImageFormat nDstFormat, int nDstStride ) = 0;

	// Reads the whole top mip into pOutImage, an image object it initialises with the texture
	// dimensions; bConvert goes to the image fill call (named by what it does)
	virtual bool ReadTexturePixels( void *pOutImage, HRenderTexture hTexture, bool bConvert ) = 0;

	virtual bool ReadBuffer( RenderBufferHandle_t hBuffer, uint32 nOffsetInBytes, void *pBuf, uint32 nBytesToRead ) = 0;

	// A read request: { int nSize; void *pData; bool bComplete; bool bConsumed } (named by what they do)
	virtual bool IsReadRequestComplete( RenderReadRequest_t *pRequest ) = 0;
	virtual bool GetReadRequestData( RenderReadRequest_t *pRequest, void **ppData, int *pSize ) = 0;
	// Marks the request consumed, returns whether it already was
	virtual bool MarkReadRequestConsumed( RenderReadRequest_t *pRequest ) = 0;

	virtual void *GetTextureObject( HRenderTexture hTexture ) = 0;

	// RENDERDEBUGFLAG_* bits
	virtual void SetDebugFlag( RenderDebugFlag_t nFlag, bool bOnOff ) = 0;
	virtual bool GetDebugFlag( RenderDebugFlag_t nFlag ) = 0;
	virtual uint32 GetDebugFlags() = 0;

	// Frame pacing target, 0 disables it
	virtual void SetFramePacingFPS( double flFPS ) = 0;
	virtual double GetFramePacingFPS() = 0;

	virtual void PreloadTextureBits( HRenderTexture hTexture, int32 nRequiredMipSize ) = 0;
	virtual void UnloadTextureBits( HRenderTexture hTexture ) = 0;
	virtual bool AreTextureBitsLoaded( HRenderTexture hTexture ) = 0;
	virtual void SetTextureStreamingPriority( HRenderTexture hTexture, RenderTextureStreamingPriority_t eTextureStreamingPriority ) = 0;
	virtual const CTextureDesc *GetOnDiskTextureDesc( HRenderTexture hTexture ) = 0;
	virtual const CTextureDesc *GetTextureDesc( HRenderTexture hTexture ) = 0;
	virtual Vector GetTextureAverageColor( HRenderTexture hTexture ) = 0;

	// The full four component average the texture manager stores (named by what it does)
	virtual Vector4D GetTextureAverageColorWithAlpha( HRenderTexture hTexture ) = 0;

	virtual bool GetCubemapRadianceSHConstants( HRenderTexture hTexture, int nMip, CUtlVector< float > &constants ) = 0;

	// Replaces the stored constants with a copy of the vector (named by what it does)
	virtual void SetCubemapRadianceSHConstants( HRenderTexture hTexture, uint32 nMip, const CUtlVector< float > &constants ) = 0;

	virtual size_t GetTextureStreamingDataOffset( HRenderTexture hTexture ) = 0;
	virtual RenderMultisampleType_t GetTextureMultisampleType( HRenderTexture hTexture ) = 0;
	virtual ImageFormat GetSwapChainDepthStencilFormat( SwapChainHandle_t hSwapChain ) = 0;
	virtual bool IsTextureSampleableRenderTarget( HRenderTexture hTexture ) = 0;
	virtual const char *GetMultisampleTypeName( RenderMultisampleType_t nType ) = 0;
	virtual bool IsTextureRenderTarget( HRenderTexture hTexture ) = 0;
	virtual void PinTextureInGPUMemory( HRenderTexture hTexture ) = 0;
	virtual void UnpinTextureInGPUMemory( HRenderTexture hTexture ) = 0;
	virtual bool IsTextureCubeMap( HRenderTexture hTexture ) = 0;

	// Compares the resident mips with the streaming target of the texture; true for a null handle
	// (named by what it does)
	virtual bool IsTextureStreamedToTargetMip( HRenderTexture hTexture ) = 0;

	virtual bool IsTextureFullyResident( HRenderTexture hTexture ) = 0;

	// The texture manager field next to the unthrottle counter
	virtual int GetMaxTextureRes() = 0;

	virtual void AddTextureResidencyListener( ITextureResidencyListener *pListener ) = 0;
	virtual void RemoveTextureResidencyListener( ITextureResidencyListener *pListener ) = 0;

	// Listeners whose first method gets ( nFrame, nCount ) every N frames, N from a convar (named by what they do)
	virtual void AddDeviceFrameListener( IRenderDeviceFrameListener *pListener ) = 0;
	virtual void RemoveDeviceFrameListener( IRenderDeviceFrameListener *pListener ) = 0;

	virtual HRenderTexture GetSwapChainTexture( SwapChainHandle_t hSwapChain, SwapChainBuffer_t nBuffer ) = 0;
	virtual void GetRenderTargetDescDimensions( const RenderTargetDesc_t &desc, int *pWidthOut, int *pHeightOut, RenderMultisampleType_t *pMultisampleTypeOut ) = 0;
	virtual bool GetPipelineFrameStats( CRenderPipelineFrameStats &frameStats ) = 0;
	virtual void ReleasePipelineFrameStats( CRenderPipelineFrameStats &frameStats ) = 0;
	virtual bool IsTextureSwapChain( HRenderTexture hTexture ) = 0;
	virtual bool GetSwapChainFromTexture( HRenderTexture hTexture, SwapChainHandle_t *pSwapChain, SwapChainBuffer_t *pBuffer ) = 0;
	virtual ImageFormat GetNearestSupportedColorFormat( ImageFormat nFormat, bool bIsRenderTarget ) = 0;
	virtual uint32 ComputeTextureMemorySize( const CTextureDesc &desc ) = 0;
	virtual void GetTextureResidencyInfo( CUtlVector< TextureResidencyInfo_t > &textures, TextureResidencyStats_t &textureResidencyStats ) = 0;
	virtual bool GetSheetSequenceTextureData( HRenderTexture hTexture, HRenderTexture *pTextureOut, Vector2D *pvecUVOut, int *pSequenceCount ) = 0;
	virtual void GetSheetCacheTextureSize( int &nWidth, int &nHeight ) = 0;
	virtual void CommitSheetCacheData() = 0;
	virtual HRenderTexture GetSheetMetaDataTexture() = 0;
	virtual void UnThrottleTextureStreamingForNFrames( uint32 nNumberOfFramesForUnthrottledTextureLoading ) = 0;
	virtual int GetNumTextureLoadsInFlight() = 0;
	virtual void OnBeginningOfClientShutdown() = 0;
	virtual void AppendStatsSummaryString( CBufferString *pOut ) = 0;
	virtual void *GetDeviceSpecificInfo( DeviceSpecificInfo_t nInfo ) = 0;
	virtual bool CanRenderToSwapChain( SwapChainHandle_t hSwapChain ) = 0;
	virtual void *GetDeviceSpecificTexture( HRenderTexture hTexture, RenderTextureDimension_t nDimension ) = 0;
	virtual void *GetDeviceSpecificImageView( HRenderTexture hTexture, RenderTextureDimension_t nDimension, RenderColorSpace_t nColorSpace ) = 0;
	virtual void *GetDeviceSpecificBuffer( RenderBufferHandle_t hGPUBuffer ) = 0;
	virtual void *GetDeviceSpecificBuffer( IndexBufferHandle_t hBuffer ) = 0;
	virtual void *GetDeviceSpecificBuffer( VertexBufferHandle_t hVertexBuffer ) = 0;
	virtual void *GetDeviceSpecificDescriptorSet( RenderDescriptorSetHandle_t hDescriptorSet ) = 0;

	// Finds or adds the entry of pObject in a map the device keeps under its own lock and returns it;
	// the entry holds the two device pointers the object stores at +32 and +24 (named by what it does)
	virtual void **GetDeviceSpecificObjectPair( void *pObject ) = 0;

	virtual void *GetDeviceSpecificRayTracePipeline( RenderRayTracePipelineHandle_t hPipeline ) = 0;
	virtual void RegisterMaxSwapChainDimensionsChangedListener( IMaxSwapChainDimensionsChangedListener *pCallback ) = 0;
	virtual void UnregisterMaxSwapChainDimensionsChangedListener( IMaxSwapChainDimensionsChangedListener *pCallback ) = 0;
	virtual void GetCurrentMaxSwapChainDimensions( int *pWidthOut, int *pHeightOut ) = 0;
	virtual ThreadHandle_t GetRenderThreadHandle() = 0;
	virtual ThreadId_t GetRenderThreadId() = 0;
	virtual bool HasOutstandingPresents( SwapChainHandle_t hSwapChain ) = 0;
	virtual void WaitForOutstandingPresents( SwapChainHandle_t hSwapChain ) = 0;

	// nWindowFlags (named by what it does): combined with the window style flags picked from the
	// mode usage bits
	virtual void ResizeEngineWindow( SwapChainHandle_t hSwapChain, PlatWindow_t hWindow, uint32 nWindowFlags, const RenderDeviceInfo_t &info ) = 0;

	virtual int GetLastValidSheetCacheFrame() = 0;
	virtual int GetTexturePinCount( HRenderTexture hTexture ) = 0;

	// nCreateFlags bit 0 is the old bFullyRewriteEachFrame, bit 1 registers the shader layout
	// (RENDER_DESCRIPTOR_SET_CREATE_REGISTER_SHADER_LAYOUT)
	virtual RenderDescriptorSetHandle_t CreateDescriptorSet( CUtlStringToken nameToken, int32 nNumDescriptors, const RenderDescriptorDesc_t *pDescriptorDescs, int32 nNumBindings, const RenderDescriptorBinding_t *pDescriptorBindings, uint8 nCreateFlags ) = 0;

	virtual void UpdateDescriptorSet( RenderDescriptorSetHandle_t hDescriptorSet, int32 nNumBindings, const RenderDescriptorBinding_t *pDescriptorBindings ) = 0;
	virtual RenderDescriptorSetHandle_t AllocateUpdateAfterBindDescriptorSet( RenderDescriptorSetHandle_t hDescriptorSet ) = 0;

	// Same arguments as UpdateDescriptorSet, applied under the descriptor set write lock (named by what it does)
	virtual void UpdateDescriptorSetImmediate( RenderDescriptorSetHandle_t hDescriptorSet, int32 nNumBindings, const RenderDescriptorBinding_t *pDescriptorBindings ) = 0;

	virtual void DestroyDescriptorSet( RenderDescriptorSetHandle_t hDescriptorSet ) = 0;

	// Copies the bindings of the set into pOut, false when the set has none (named by what it does)
	virtual bool GetDescriptorSetBindings( RenderDescriptorSetHandle_t hDescriptorSet, void *pOut ) = 0;

	virtual bool AddDescriptorsToGlobalPerFrameDescriptorSet( int32 nNumDescriptors, const RenderDescriptorDesc_t *pDescriptorDescs ) = 0;
	virtual void UpdateGlobalPerFrameDescriptorBindings( int32 nNumBindings, const RenderDescriptorBinding_t *pDescriptorBindings ) = 0;

	// Vulkan device group rendering, see "Vulkan MultiGPU Rendering - Enabled!" (named by what they do)
	virtual bool IsMultiGPURenderingEnabled() = 0;
	virtual int GetMultiGPUDeviceCount() = 0;
	virtual uint32 GetMultiGPUDeviceMask() = 0;
	// 1 / ( device count / 2 ), 1 without device groups
	virtual float GetMultiGPUFrameScale() = 0;

	virtual float GetLastPresentTime() = 0;

	// bUploadOption: see FindOrCreateTexture
	virtual void AsyncSetTextureData( HRenderTexture hTexture, const CTextureDesc *pDataDesc, const void *pData, uint32 nDataSize, bool bUploadOption, uint32 nSpecificMipLevelToSet, const Rect3D_t *pSubRectToUpdate, uint32 nTextureUpdateFlags, const DataRecycleDelegate_t *pDataRecycle, RenderAsyncTextureUpdateFlags_t nAsyncFlags ) = 0;

	virtual GraphicsAPISpecificTextureHandle_t GetGraphicsAPISpecificTextureHandle( HRenderTexture hTexture ) = 0;
	virtual void GetLargestMonitorDimensions( int *pWidth, int *pHeight ) = 0;
	virtual void PushRenderDeviceFlags( RenderDeviceFlag_t nFlags ) = 0;
	virtual void PopRenderDeviceFlags( RenderDeviceFlag_t nFlags ) = 0;

	// Needs RENDERDEBUGFLAG_ENABLE_GPU_TIMESTAMPS
	virtual bool GetGPUFrameTimeMS( SwapChainHandle_t hSwapChain, float *pGPUFrameTimeMSOut, uint32 *pFrameNumberOut ) = 0;

	// m_nHighestFrameSubmitted of the device (named by what it does)
	virtual uint32 GetHighestFrameSubmitted() = 0;

	// m_bUseReverseDepth (RenderSystem/UseReverseDepth of gameinfo)
	virtual bool IsUsingReverseDepth() = 0;
	virtual void LowLatencySleep() = 0;
	virtual void SetLatencyMarker( uint32 nMarker ) = 0;

	// Returns 0 on Vulkan and Direct3D 11; named after its place between the latency marker methods
	virtual int GetLatencyMarkerFrameId() = 0;

	// VK_GOOGLE_display_timing, NVIDIA low latency or AMD anti-lag available
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
	// Acceleration structure device address (named by what it does)
	virtual uint64 GetBLASDeviceAddress( RenderBLASHandle_t hBLAS ) = 0;
	virtual IsReadyToCompactResult_t IsReadyToCompact( RenderBLASHandle_t hBLAS ) = 0;
	virtual void DestroyBLAS( RenderBLASHandle_t hBLAS ) = 0;
	virtual RenderTLASHandle_t CreateTLAS( const TLASCreateDesc_t &desc, TLASInstance_t *pInstances, int nInstanceCount ) = 0;
	virtual void DestroyTLAS( RenderTLASHandle_t hTLAS ) = 0;
	// Acceleration structure device address (named by what it does)
	virtual uint64 GetTLASDeviceAddress( RenderTLASHandle_t hTLAS ) = 0;
	virtual RenderRayTracePipelineHandle_t CreateRayTracePipeline( const RayTracePipelineStateCreateDesc_t &desc ) = 0;
	virtual void DestroyRayTracePipeline( RenderRayTracePipelineHandle_t hPipeline ) = 0;
	virtual bool IsCreatingRayTracePipelines() = 0;
	virtual IRaytraceShaderBindingTable *CreateRayTraceShaderBindingTable( const ShaderBindingTableCreateDesc_t &desc ) = 0;
	virtual void DestroyRayTraceShaderBindingTable( IRaytraceShaderBindingTable *pTable ) = 0;

	// VK_EXT_opacity_micromap
	virtual RenderOpacityMicromapHandle_t CreateOpacityMicromap( const OpacityMicromapCreateDesc_t &desc ) = 0;
	virtual void DestroyOpacityMicromap( RenderOpacityMicromapHandle_t hMicromap ) = 0;
	// Null without VK_EXT_opacity_micromap (named by what it does)
	virtual uint64 GetOpacityMicromapDeviceAddress( RenderOpacityMicromapHandle_t hMicromap ) = 0;

	virtual bool IsRayTracingSupported() = 0;
	virtual bool IsAsyncPresentEnabled() = 0;
	virtual void EnableAsyncPresent( bool bEnable ) = 0;
	virtual bool StartRenderDocCapture( PlatWindow_t hWnd ) = 0;
	virtual bool EndRenderDocCapture( PlatWindow_t hWnd ) = 0;

	// pEntries are { HRenderTexture hTexture; uint16 nValue }, nValue goes to the texture manager per texture,
	// then the whole array to a texture manager virtual (named by what it does)
	virtual void SetTexturesStreamingValues( int nCount, const void *pEntries ) = 0;

	virtual uint32 GetTextureViewIndex( HRenderTexture hTexture, RenderColorSpace_t nColorSpace, RenderTextureDimension_t nDimension ) = 0;

	// Inverse of GetTextureViewIndex (named by what it does)
	virtual HRenderTexture GetTextureFromViewIndex( uint32 nViewIndex ) = 0;

	virtual uint32 GetSamplerIndex( SamplerStateHandle_t hSamplerState ) = 0;
	virtual RenderDescriptorSetHandle_t GetGlobalLateBoundBindlessDescriptorSet() = 0;

	virtual PooledIndexBufferHandle_t AllocatePooledIndexBuffer( uint32 nIndexCount, uint32 nIndexSize, const void *pIndices, uint32 *pFirstIndexOut ) = 0;
	virtual void FreePooledIndexBuffer( PooledIndexBufferHandle_t hHandle ) = 0;
	// Returns the first index, writes the index size and count (named by what it does)
	virtual uint32 GetPooledIndexBufferRange( PooledIndexBufferHandle_t hHandle, uint32 *pIndexSizeOut, uint32 *pIndexCountOut ) = 0;
	virtual IndexBufferHandle_t GetPooledIndexBuffer() = 0;
	virtual PooledVertexBufferHandle_t AllocatePooledVertexBuffer( uint32 nVertexCount, uint32 nVertexSize, const void *pVertices, uint32 *pFirstVertexOut ) = 0;
	virtual void FreePooledVertexBuffer( PooledVertexBufferHandle_t hHandle ) = 0;
	// Byte offset of the allocation in the pool buffer (named by what it does)
	virtual uint32 GetPooledVertexBufferOffset( PooledVertexBufferHandle_t hHandle ) = 0;
	// Returns the vertex count, writes the vertex size (named by what it does)
	virtual uint32 GetPooledVertexBufferCount( PooledVertexBufferHandle_t hHandle, uint32 *pVertexSizeOut ) = 0;
	virtual VertexBufferHandle_t GetPooledVertexBuffer() = 0;

	virtual void TriggerValidationError() = 0;

	// nType 2 is XeSS, 3 FidelityFX (ffxCreateContext); null when unavailable (named by what it does)
	virtual void *CreateSuperSamplingContext( int nType ) = 0;
	virtual bool IsSuperSamplingAvailable( int nType ) = 0;

	// Wraps pObject in a ref counted holder released once the GPU finished the current frame
	// (named by what it does)
	virtual void ReleaseAfterCurrentFrame( IRenderDeviceRefCounted *pObject ) = 0;

	virtual void DefragmentGPUMemory( RenderDefragFlags_t nDefragFlags, bool bAsyncDefrag ) = 0;
	virtual void BeginRenderingToSwapChain( SwapChainHandle_t hSwapChain ) = 0;

	// Feeds a lazily created helper the device frees on client shutdown; failing that, it is destroyed
	// again (named by what it does)
	virtual bool SubmitFrameCaptureData( void *pData, int nUnused, void *pContext ) = 0;

	virtual RenderDeviceMemoryPoolHandle_t CreateDeviceMemoryPool( const DeviceMemoryPoolCreateDesc_t &desc ) = 0;
	virtual void DestroyDeviceMemoryPool( RenderDeviceMemoryPoolHandle_t hPool ) = 0;
	virtual bool AllocateTextureInPool( RenderDeviceMemoryPoolHandle_t hPool, HRenderTexture hTexture, uint64 nOffset ) = 0;
	virtual void FreeTextureFromPool( RenderDeviceMemoryPoolHandle_t hPool, HRenderTexture hTexture ) = 0;
	virtual bool AllocateBufferInPool( RenderDeviceMemoryPoolHandle_t hPool, RenderBufferHandle_t *phBuffer, uint64 nOffset ) = 0;
	virtual void FreeBufferFromPool( RenderDeviceMemoryPoolHandle_t hPool, RenderBufferHandle_t hBuffer ) = 0;
	virtual bool GetDeviceMemoryTextureAllocationInfo( HRenderTexture hTexture, DeviceMemoryAllocationInfo_t *pInfo ) = 0;
	virtual bool GetDeviceMemoryBufferAllocationInfo( RenderBufferHandle_t hBuffer, DeviceMemoryAllocationInfo_t *pInfo ) = 0;

	// Aliases the memory of pool allocated textures (TSPEC_POOL_ALLOCATED) onto hTexture (named by what it does)
	virtual bool AliasPoolTextures( HRenderTexture hTexture, int nCount, const HRenderTexture *pTextures ) = 0;

	// Format feature flags of the device for nFormat
	virtual uint32 CheckFormatSupport( ImageFormat nFormat ) = 0;

	virtual void *GetDevicePtr() = 0;

	// VK_EXT_device_generated_commands
	virtual RenderIndirectCommandsLayoutHandle_t CreateIndirectCommandsLayout( RenderShaderHandle_t hShader, int nTokens, const uint32 *pTokens, int nStride, bool bUnordered ) = 0;
	virtual void DestroyIndirectCommandsLayout( RenderIndirectCommandsLayoutHandle_t hLayout ) = 0;
	virtual RenderBufferHandle_t CreateIndirectCommandsPreprocessBuffer( RenderIndirectCommandsLayoutHandle_t hLayout, RenderIndirectExecutionSetHandle_t hExecutionSet, int nMaxSequences ) = 0;
	virtual RenderIndirectExecutionSetHandle_t CreateIndirectExecutionSet( RenderShaderHandle_t hShader, int nMaxPipelines ) = 0;
	virtual bool UpdateIndirectExecutionSet( RenderIndirectExecutionSetHandle_t hExecutionSet, RenderShaderHandle_t hShader, int nIndex ) = 0;
	virtual void DestroyIndirectExecutionSet( RenderIndirectExecutionSetHandle_t hExecutionSet ) = 0;

	// Stores the name and reports shaders that were not compiled during the level load
	// ("Shader '%s' not compiled during level load") (named by what it does)
	virtual void SetLevelLoadName( const char *pName ) = 0;

	// m_nHighestFrameSubmitted + 1 (named by what it does)
	virtual uint32 GetCurrentFrameNumber() = 0;

	// Counter the Vulkan memory manager bumps for every fenced resource it releases (named by what it does)
	virtual int GetReleasedFencedResourceCount() = 0;

	virtual bool IsRenderDocEnabled() = 0;

	// Detaches the previous object and attaches pObject to the texture manager (named by what it does)
	virtual void SetTextureManagerObserver( IRenderDeviceRefCounted *pObject ) = 0;

	// Reads nMipCount mips (all when <= 0) from nFirstMip of the compiled texture data and hands them to
	// pCallback; nRequestTag and nRequestFlags are stored in the request with the mip range. The callback
	// type is the read callback interface of the module (IRenderReadCallback in its RTTI)
	virtual bool ReadCompiledTextureMips( HRenderTexture hTexture, int nRequestTag, int nFirstMip, int nMipCount, int nRequestFlags, std::shared_ptr< IRenderReadCallback > pCallback ) = 0;

	// From RenderSystem/UseReverseDepth and RenderSystem/Use32BitDepthBuffer of gameinfo, set by InitDevice
	bool m_bUseReverseDepth;
	bool m_bUse32BitDepthBuffer;
};

#endif // IRENDERDEVICE_H
