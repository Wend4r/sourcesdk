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
class InfoForResourceTypeCTextureBase;
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

	virtual const char *GetShaderModelString( int nShaderType ) = 0;

	virtual void BeginRendering() = 0;
	virtual void BeginSubmittingDisplayLists( SwapChainHandle_t hSwapChain ) = 0;
	virtual bool Present( SwapChainHandle_t hSwapChain, IRenderDevicePresentCallback *pCallback, uint32 nRenderContextFlags, RenderSemaphoreHandle_t hWaitSemaphore ) = 0;
	virtual void Flush() = 0;
	virtual void ForceFlushGPU( SwapChainHandle_t hSwapChain ) = 0;
	virtual void SetHardwareGammaRamp( SwapChainHandle_t hSwapChain, float flGamma ) = 0;
	virtual std::unique_ptr< IRenderShaderByteCode > CompileShader( const char *pProgram, uint32 nBufLen, const char *pShaderVersion ) = 0;
	virtual RenderShaderHandle_t CreateShader( int nType, std::unique_ptr< IRenderShaderByteCode > pShader, const char *pDebugName, int nSpecializationConstantCount, const void *pSpecializationConstants ) = 0;
	virtual void DestroyShader( int nShaderType, RenderShaderHandle_t hShader ) = 0;

	// Register counts and binary size of a compiled shader
	virtual void GetShaderStats( RenderShaderStats_t *pOutStats, int nType, RenderShaderHandle_t hShader ) = 0;

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
	virtual VertexBufferHandle_t CreateVertexBuffer( int nType, const BufferDesc_t &desc, int nExtraUsageFlags ) = 0;
	virtual VertexBufferHandle_t CreateVertexBuffer( int nType, const BufferDesc_t &desc, int nLayoutFields, const RenderInputLayoutField_t *pLayoutFields, int nExtraUsageFlags ) = 0;
	virtual VertexBufferHandle_t CreateVertexBuffer( int nType, const BufferDesc_t &desc, RenderInputLayout_t hLayout, int nExtraUsageFlags ) = 0;

	virtual void DestroyVertexBuffer( VertexBufferHandle_t hVertexBuffer ) = 0;
	virtual IndexBufferHandle_t CreateIndexBuffer( int nType, const BufferDesc_t &desc, int nExtraUsageFlags ) = 0;
	virtual void DestroyIndexBuffer( IndexBufferHandle_t hIndexBuffer ) = 0;
	virtual void GetVertexBufferDesc( VertexBufferHandle_t hBuffer, BufferDesc_t *pDesc ) = 0;
	virtual void GetIndexBufferDesc( IndexBufferHandle_t hBuffer, BufferDesc_t *pDesc ) = 0;
	virtual void GetGPUBufferDesc( RenderBufferHandle_t hBuffer, BufferDesc_t *pDesc ) = 0;
	virtual RenderInputLayout_t GetVertexBufferInputLayout( VertexBufferHandle_t hVertexBuffer ) = 0;
	virtual RenderBufferHandle_t CreateGPUBuffer( int nType, const BufferDesc_t &desc, int nFlags, const void *pInitialData ) = 0;
	virtual RenderBufferHandle_t CreateGPUBufferFormatted( int nType, const BufferDesc_t &desc, int nFlags, ImageFormat nFormat, const void *pInitialData ) = 0;
	virtual uint64 GetGPUBufferDeviceAddress( RenderBufferHandle_t hGPUBuffer ) = 0;
	virtual void DestroyGPUBuffer( RenderBufferHandle_t hGPUBuffer ) = 0;
	virtual uint64 GetVertexBufferDeviceAddress( VertexBufferHandle_t hVertexBuffer ) = 0;
	virtual uint64 GetIndexBufferDeviceAddress( IndexBufferHandle_t hIndexBuffer ) = 0;
	virtual uint64 GetPooledVertexBufferDeviceAddress( RenderResourceHandle_t hPooledVertexBuffer ) = 0;
	virtual uint64 GetPooledIndexBufferDeviceAddress( PooledIndexBufferHandle_t hPooledIndexBuffer ) = 0;
	// bUnk is passed on to the texture upload of the render context, which the Vulkan backend ignores
	virtual HRenderTexture FindOrCreateTexture( const char *pResourceName, bool bIsAnonymous, const CTextureCreationDesc *pDescriptor, const CTextureDesc *pDataDesc, const void *pData, int nDataSize, bool bUnk ) = 0;
	virtual HRenderTexture GetErrorTexture( RenderTextureDimension_t nDimension ) = 0;
	virtual HRenderTexture FindOrCreateFileTexture( const char *pFileName, int nLoadMode ) = 0;
	virtual HRenderTexture FindFileTexture( ResourceId_t nId, int nLoadMode ) = 0;
	virtual HRenderTexture FindOrCreateExternalTexture( const char *pResourceName, uint64 hSharedTexture ) = 0;
	virtual bool UpdateExternalTexture( HRenderTexture hTexture, uint64 hSharedTexture ) = 0;
	virtual SamplerStateHandle_t FindOrCreateSamplerState( const CSamplerStateDesc &samplerState, const char *pDebugName ) = 0;

	virtual RenderQueryHandle_t CreateQueryObject( int nType ) = 0;

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
	virtual bool ReadTexturePixels( FloatBitMap_t *pBitmap, HRenderTexture hTexture, uint8 nGammaType ) = 0;

	virtual bool ReadBuffer( RenderBufferHandle_t hBuffer, uint32 nOffsetInBytes, void *pBuf, uint32 nBytesToRead ) = 0;
	virtual bool IsPolledBufferReadbackReady( void *hRequest ) = 0;
	virtual bool GetPolledBufferReadbackData( void *hRequest, void **ppData, int *pSize ) = 0;
	virtual bool ReleasePolledBufferReadback( void *hRequest ) = 0;
	virtual const Sheet_t *GetTextureSheet( HRenderTexture hTexture ) = 0;

	// RENDERDEBUGFLAG_* bits
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
	virtual int unk106() = 0;
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

	virtual void *GetDeviceSpecificInfo( int nInfo ) = 0;
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
	virtual void unk151( RenderDescriptorSetHandle_t hDescriptorSet, int32 nNumBindings, const RenderDescriptorBinding_t *pDescriptorBindings ) = 0;
	virtual void DestroyDescriptorSet( RenderDescriptorSetHandle_t hDescriptorSet ) = 0;
	virtual bool GetDescriptorSetDescs( RenderDescriptorSetHandle_t hDescriptorSet, CUtlVector< RenderDescriptorDesc_t > &descriptorDescs ) = 0;
	virtual bool AddDescriptorsToGlobalPerFrameDescriptorSet( int32 nNumDescriptors, const RenderDescriptorDesc_t *pDescriptorDescs ) = 0;
	virtual void UpdateGlobalPerFrameDescriptorBindings( int32 nNumBindings, const RenderDescriptorBinding_t *pDescriptorBindings ) = 0;

	// Multi-GPU state
	virtual bool IsMultiGPUEnabled() = 0;
	virtual int GetMultiGPUCount() = 0;
	virtual uint32 GetMultiGPUDeviceMask() = 0;
	virtual float unk159() = 0;

	virtual float GetLastPresentTime() = 0;
	virtual void AsyncSetTextureData( HRenderTexture hTexture, const CTextureDesc *pDataDesc, const void *pData, int nDataSize, bool bUnk, int nSpecificMipLevelToSet, const Rect3D_t *pSubRectToUpdate, uint32 nTextureUpdateFlags, const DataRecycleDelegate_t *pDataRecycleDelegate, uint32 nAsyncTextureUpdateFlags ) = 0;
	virtual void *GetGraphicsAPISpecificTextureHandle( HRenderTexture hTexture ) = 0;
	virtual void GetLargestMonitorDimensions( int *pWidth, int *pHeight ) = 0;
	virtual void PushRenderDeviceFlags( uintp nFlags ) = 0;
	virtual void PopRenderDeviceFlags( uintp nFlags ) = 0;

	// Needs RENDERDEBUGFLAG_ENABLE_GPU_TIMESTAMPS
	virtual bool GetGPUFrameTimeMS( SwapChainHandle_t hSwapChain, float *pGPUFrameTimeMSOut, uint32 *pFrameNumberOut ) = 0;

	virtual int GetPresentCount() = 0;
	virtual bool IsUsing32BitDepthBuffer() = 0;
	virtual void LowLatencySleep() = 0;
	virtual void SetLatencyMarker( uint32 nMarker ) = 0;
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
	virtual RenderResourceHandle_t CreateOpacityMicromap( const void *pDesc ) = 0;
	virtual void DestroyOpacityMicromap( RenderResourceHandle_t hOpacityMicromap ) = 0;
	virtual uint64 unk195( RenderResourceHandle_t hOpacityMicromap ) = 0;
	virtual bool IsRayTracingSupported() = 0;
	virtual bool IsAsyncPresentEnabled() = 0;
	virtual void EnableAsyncPresent( bool bEnable ) = 0;
	virtual bool StartRenderDocCapture( PlatWindow_t hWnd ) = 0;
	virtual bool EndRenderDocCapture( PlatWindow_t hWnd, int ( *pfnCaptureCallback )( const char *pCaptureFilePath, void *pRenderDocAPI, void *pContext ), void *pContext ) = 0;
	virtual void RequestTextureMipLevels( int nCount, const void *pTextureRequests ) = 0;
	virtual uint32 GetTextureViewIndex( HRenderTexture hTexture, RenderColorSpace_t nColorSpace, RenderTextureDimension_t nDimension ) = 0;
	virtual HRenderTexture GetTextureFromViewIndex( uint32 nTextureViewIndex ) = 0;
	virtual uint32 GetSamplerIndex( SamplerStateHandle_t hSamplerState ) = 0;
	virtual RenderDescriptorSetHandle_t GetGlobalLateBoundBindlessDescriptorSet() = 0;
	virtual PooledIndexBufferHandle_t AllocatePooledIndexBuffer( uint nIndexCount, uint nIndexSize, const void *pIndices, uint *pOutStartIndex ) = 0;
	virtual void FreePooledIndexBuffer( PooledIndexBufferHandle_t hBuffer ) = 0;
	virtual uint GetPooledIndexBufferOffset( PooledIndexBufferHandle_t hBuffer, uint32 *pIndexSizeOut, uint32 *pIndexCountOut ) = 0;
	virtual IndexBufferHandle_t GetPooledIndexBuffer() = 0;
	virtual RenderResourceHandle_t AllocatePooledVertexBuffer( uint nVertexCount, uint nVertexSize, const void *pVertices, uint32 nUnk, const char *pDebugName ) = 0;
	virtual void FreePooledVertexBuffer( RenderResourceHandle_t hBuffer ) = 0;
	virtual uint32 GetPooledVertexBufferBaseVertex( RenderResourceHandle_t hPooledVertexBuffer ) = 0;
	virtual uint32 GetPooledVertexBufferVertexCount( RenderResourceHandle_t hPooledVertexBuffer, uint32 *pVertexSizeOut ) = 0;
	virtual VertexBufferHandle_t GetPooledVertexBuffer() = 0;
	virtual void unk215() = 0;
	virtual IUpscaler *CreateUpscaler( int nType ) = 0;

	// XeSS / FidelityFX availability
	virtual bool IsSuperSamplingAvailable( int nType ) = 0;

	virtual void DestroyUpscaler( IUpscaler *pUpscaler ) = 0;
	virtual void unk219( bool bUnk, bool bAsync ) = 0;
	virtual void BeginRenderingToSwapChain( SwapChainHandle_t hSwapChain ) = 0;
	virtual void unk221( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;

	virtual void *CreateDeviceMemoryPool( uint64 nMinBlockSize ) = 0;
	virtual void DestroyDeviceMemoryPool( void *hPool ) = 0;
	virtual bool AllocateTextureInPool( void *hPool, HRenderTexture hTexture ) = 0;
	virtual void FreeTextureFromPool( void *hPool, HRenderTexture hTexture ) = 0;
	virtual bool AllocateBufferInPool( void *hPool, RenderBufferHandle_t hBuffer ) = 0;
	virtual void FreeBufferFromPool( void *hPool, RenderBufferHandle_t hBuffer ) = 0;
	virtual bool GetDeviceMemoryTextureAllocationInfo( HRenderTexture hTexture, void *pInfoOut ) = 0;
	virtual bool GetDeviceMemoryBufferAllocationInfo( RenderBufferHandle_t hBuffer, void *pInfoOut ) = 0;
	virtual bool unk230( HRenderTexture hTexture, int nCount, const HRenderTexture *pTextures ) = 0;
	virtual uint32 CheckFormatSupport( ImageFormat nFormat ) = 0;
	virtual int unk232( SwapChainHandle_t hSwapChain ) = 0;

	// Device generated commands
	virtual void *CreateIndirectCommandsLayout( IRenderContext *pRenderContext, uint32 nTokenCount, const void *pTokens, uint32 nIndirectStride, bool bExplicitPreprocess ) = 0;
	virtual void DestroyIndirectCommandsLayout( void *hIndirectCommandsLayout ) = 0;
	virtual RenderBufferHandle_t CreateIndirectCommandsPreprocessBuffer( void *hIndirectCommandsLayout, void *hIndirectExecutionSet, uint32 nMaxSequenceCount ) = 0;
	virtual void *CreateIndirectExecutionSet( IRenderContext *pRenderContext, int nMaxPipelineCount ) = 0;
	virtual bool UpdateIndirectExecutionSet( void *hIndirectExecutionSet, IRenderContext *pRenderContext, int nPipelineIndex ) = 0;
	virtual void DestroyIndirectExecutionSet( void *hIndirectExecutionSet ) = 0;

	// Keeps pName, whose file name is appended to RenderDoc capture names, and flushes the pipeline cache
	virtual void unk239( const char *pName ) = 0;

	virtual int unk240() = 0;
	virtual int unk241() = 0;
	virtual void *unk242() = 0;
	virtual void unk243( void *pObject ) = 0;
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
	virtual bool IsMultisampleTypeSupported( int nFormat, RenderMultisampleType_t nType ) = 0;
};

#endif // IRENDERDEVICE_H
