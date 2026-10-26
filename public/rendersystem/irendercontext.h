//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: Command recording interface of the render device
//
//===========================================================================//

#ifndef IRENDERCONTEXT_H
#define IRENDERCONTEXT_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/basetypes.h"
#include "rendersystem/irenderdevice.h"
#include "rendersystem/rendersystemtypes.h"

#include <functional>
#include <memory>

class IBarrierHelper;
class ICommandCallback;
class IReadBufferCallback;

// Value type of RenderDocAnnotation_t, translated to the RenderDoc annotation types
enum RenderDocAnnotationType_t : uint8
{
	RENDERDOC_ANNOTATION_BOOL = 0,
	RENDERDOC_ANNOTATION_FLOAT,
	RENDERDOC_ANNOTATION_INT32,
	RENDERDOC_ANNOTATION_UINT64,
	RENDERDOC_ANNOTATION_STRING,
	RENDERDOC_ANNOTATION_FLOAT4,
	RENDERDOC_ANNOTATION_API_OBJECT,
	RENDERDOC_ANNOTATION_EMPTY,
};

// Returned by the annotation callbacks of IRenderContext::SetRenderDocCommandAnnotations and
// SetRenderDocObjectAnnotations
struct RenderDocAnnotation_t
{
	CUtlString m_Key;

	union
	{
		bool m_bValue;
		float m_flValue;
		int32 m_nValue;
		uint64 m_nUInt64Value;

		// The buffer of a CUtlString owned by the record
		char *m_pszValue;
		float m_flValues[ 4 ];
		void *m_pAPIObject;
	};

	RenderDocAnnotationType_t m_nType;
};

// Bound by IRenderContext::SetRenderTargets
struct RenderTargetSet_t
{
	HRenderTexture m_hColorTargets[ 8 ];
	HRenderTexture m_hDepthStencilTarget;
	HRenderTexture m_hShadingRateTarget;

	// The mip level of a mipmapped target, the array slice of an array target
	int m_nColorTargetMipOrSlice[ 8 ];
	RenderColorSpace_t m_nColorTargetColorSpace[ 8 ];
	int m_nDepthStencilMipOrSlice;
	int16 m_nColorTargetCount;

	// Binds the depth-stencil target's read-only view with the "none" store ops
	bool m_bReadOnlyDepthStencil : 1;
};

// Used by IRenderContext::ClearRenderTargets and IRenderContext::SetViewport; the depth clear value
// comes from the device and the stencil one is 0
struct RenderClearDesc_t
{
	// One per cleared color target, the last one repeats for the remaining targets
	const Vector4D *m_pColors;
	int m_nColorCount;

	// Bits 0-7 select the color targets, 0x100 the depth and 0x200 the stencil
	uint32 m_nClearMask;
};

// A shader resource or storage image binding of IRenderContext::SetTextures and SetUnorderedAccessViews
struct RenderTextureBinding_t
{
	HRenderTexture m_hTexture;
	RenderBufferHandle_t m_hBuffer;
	uint16 m_nRequiredMipSize;
	RenderColorSpace_t m_nColorSpace;
	RenderTextureDimension_t m_nDimension;

	// -1 binds all mip levels
	int8 m_nMipLevel;
	bool m_bIsBuffer;
	const char *m_pDebugName;
};

// Barrier records of IRenderContext::TextureBarriers, BufferBarriers and MemoryBarriers; the stage and
// access masks are render system flags translated to the Vulkan ones
struct RenderTextureBarrier_t
{
	HRenderTexture m_hTexture;
	uint32 m_nSrcStageMask;
	uint32 m_nDstStageMask;
	uint32 m_nSrcAccessMask;
	uint32 m_nDstAccessMask;

	// A negative base mip level means 0, a non-positive mip count means all levels
	int m_nBaseMipLevel;
	int m_nMipLevelCount;
	uint32 m_nLayout;
};

struct RenderBufferBarrier_t
{
	RenderBufferHandle_t m_hBuffer;
	uint32 m_nSrcStageMask;
	uint32 m_nDstStageMask;
	uint32 m_nSrcAccessMask;
	uint32 m_nDstAccessMask;
};

struct RenderMemoryBarrier_t
{
	uint32 m_nSrcStageMask;
	uint32 m_nDstStageMask;
	uint32 m_nSrcAccessMask;
	uint32 m_nDstAccessMask;
};

// A triangle geometry of BLASBuildDesc_t
struct BLASGeometryDesc_t
{
	RenderBufferHandle_t m_hVertexBuffer;
	RenderBufferHandle_t m_hIndexBuffer;
	int m_nVertexStride;
	int m_nMaxVertex;
	int m_nTriangleCount;

	// 2 selects 16-bit indices, anything else 32-bit ones
	int m_nIndexSize;
	int m_nVertexOffset;
	int m_nIndexOffset;

	// Only the low 2 bits are passed on as the geometry flags
	uint32 m_nGeometryFlags;
	uint8 m_pad02C[ 68 ];

	// Optional opacity micromap of the geometry
	RenderResourceHandle_t m_hOpacityMicromap;
	uint32 m_nOpacityMicromapBaseTriangle;

	// 2 selects 16-bit and 4 32-bit micromap indices
	uint8 m_nOpacityMicromapIndexSize;
	uint32 m_nOpacityMicromapIndexStride;
	RenderBufferHandle_t m_hOpacityMicromapIndexBuffer;
};

// IRenderContext::BuildBLAS; a NULL m_hScratchBuffer makes the build allocate the scratch memory
struct BLASBuildDesc_t
{
	RenderBLASHandle_t m_hBLAS;
	const BLASGeometryDesc_t *m_pGeometries;
	int m_nGeometryCount;

	// Updates the BLAS in place instead of rebuilding it
	bool m_bUpdate;
	RenderBufferHandle_t m_hScratchBuffer;
	uint32 m_nScratchOffset;
};

// A texture and the layout IRenderContext::SubmitBarrierBatch transitions it to
struct RenderTextureLayoutTransition_t
{
	HRenderTexture m_hTexture;
	uint32 m_nLayout;
};

// IRenderContext::SubmitBarrierBatch
struct RenderBarrierBatch_t
{
	uint32 m_nLayoutTransitionCount;
	const RenderTextureLayoutTransition_t *m_pLayoutTransitions;
	uint32 m_nTextureBarrierCount;
	const RenderTextureBarrier_t *m_pTextureBarriers;
	uint32 m_nBufferBarrierCount;
	const RenderBufferBarrier_t *m_pBufferBarriers;
	uint32 m_nMemoryBarrierCount;
	const RenderMemoryBarrier_t *m_pMemoryBarriers;

	// Passed to FlushTextureBarriers first
	uint32 m_nFlushedTextureCount;
	const HRenderTexture *m_pFlushedTextures;
};

// IRenderContext::BuildOpacityMicromaps
struct OpacityMicromapBuildDesc_t
{
	RenderResourceHandle_t m_hOpacityMicromap;
	RenderBufferHandle_t m_hDataBuffer;
	RenderBufferHandle_t m_hTriangleArrayBuffer;
	uint8 m_pad018[ 8 ];
	RenderBufferHandle_t m_hScratchBuffer;
	uint32 m_nScratchOffset;
};

// Created by IRenderDevice::CreateRenderContext. Slots whose purpose is not known yet are
// placeholders named after their vtable index
abstract_class IRenderContext
{
public:
	// Resets the states selected by nStateFlags to their defaults: 1, 2, 4, 0x20 and 0x10000 unbind shader stages,
	// 0x80 the rasterizer state, 0x100 the depth-stencil state and 0x200 the blend state
	virtual void ResetState( uint32 nStateFlags ) = 0;
	virtual void SetRenderTargets( const RenderTargetSet_t &renderTargets ) = 0;
	// Resolves (or copies, when the sample counts match) the bound depth-stencil target into hDstTexture
	virtual void ResolveDepthStencil( const Rect_t *pSrcRect, int nDstX, int nDstY, HRenderTexture hDstTexture ) = 0;
	virtual void ResolveRenderTarget( int nRenderTarget, const Rect_t *pSrcRect, int nDstX, int nDstY, HRenderTexture hDstTexture ) = 0;
	virtual void ClearRenderTargets( const RenderClearDesc_t &clearDesc ) = 0;
	virtual void ClearUnorderedAccessViewUint( HRenderTexture hTexture, int nMipLevel, const uint32 *pValues ) = 0;
	virtual void ClearUnorderedAccessViewFloat( HRenderTexture hTexture, int nMipLevel, const float *pValues ) = 0;
	// nTargetMask: bits 0-7 color targets, 0x100 depth, 0x200 stencil; nDiscardFlags: 1 discards the contents
	// before the pass (load op "don't care"), 2 after it (store op "don't care")
	virtual void DiscardRenderTargets( uint16 nTargetMask, uint8 nDiscardFlags ) = 0;
	virtual void unk008() = 0;
	// Sets the viewport and then clears it with pClearDesc (see ClearRenderTargets)
	virtual void SetViewport( const RenderClearDesc_t &clearDesc, int nTopLeftX, int nTopLeftY, int nWidth, int nHeight ) = 0;
	// Ends the recording with EndDisplayList and hands the list to IRenderDevice::SubmitDisplayLists
	virtual void SubmitDisplayList() = 0;
	virtual CDisplayList *EndDisplayList() = 0;
	// The display list waits until the dependency is signalled by all its batches
	virtual void WaitOnDependency( CDependencyDescriptor *pDependency ) = 0;
	// Counts one of the dependency's batches as submitted when the display list is
	virtual void SignalDependency( CDependencyDescriptor *pDependency ) = 0;
	virtual void GetViewport( RenderViewport_t *pViewport, int nIndex ) = 0;
	virtual void SetViewports( int nCount, const RenderViewport_t *pViewports ) = 0;
	virtual void SetScissorRects( int nCount, const Rect_t *pRects ) = 0;
	// The input layout fields without a slot are matched by semantic against pInputSignature
	virtual void BindVertexShader( RenderShaderHandle_t hShader, RenderInputLayout_t hInputLayout, const VsInputSignature_t *pInputSignature ) = 0;
	virtual void BindShader( int nShaderType, RenderShaderHandle_t hShader ) = 0;
	virtual void SetTexture( int nSlot, HRenderTexture hTexture, RenderTextureDimension_t nDimension, int nShaderType, RenderColorSpace_t nColorSpace, int32 nRequiredMipSize ) = 0;
	// nMipLevel -1 binds all mip levels (SetTexture passes -1)
	virtual void SetTextureMip( int nSlot, HRenderTexture hTexture, int nMipLevel, RenderTextureDimension_t nDimension, int nShaderType, RenderColorSpace_t nColorSpace, int32 nRequiredMipSize ) = 0;
	virtual void SetTextures( int nFirstSlot, int nCount, const RenderTextureBinding_t *pBindings, int nShaderType ) = 0;
	// Binds storage images in the general layout
	virtual void SetUnorderedAccessViews( int nFirstSlot, int nCount, const RenderTextureBinding_t *pBindings, int nShaderType ) = 0;
	// Transitions the texture into the shader read layout (general when it is a storage image)
	virtual void TransitionTextureForShaderRead( HRenderTexture hTexture, RenderColorSpace_t nColorSpace, int32 nRequiredMipSize, int nMipLevel ) = 0;
	virtual void SetPixelShaderSamplerState( int nSlot, uint8 nFilter, int nAddressU, int nAddressV, int nAddressW, int nComparisonFunc ) = 0;
	virtual void SetVertexShaderSamplerState( int nSlot, uint8 nFilter, int nAddressU, int nAddressV, int nAddressW, int nComparisonFunc ) = 0;
	virtual void SetSamplerState( int nSlot, const CSamplerStateDesc &desc, int nShaderType ) = 0;
	virtual void SetSamplerStates( int nFirstSlot, int nCount, const CSamplerStateDesc *pDescs, int nShaderType ) = 0;
	virtual void SetDescriptorSets( uint32 nFirstSet, int nCount, const RenderDescriptorSetHandle_t *pDescriptorSets, const uint32 *pDynamicOffsetCounts, const uint32 *const *ppDynamicOffsets ) = 0;
	virtual RenderDescriptorSetHandle_t CreateDynamicDescriptorSet( RenderDescriptorSetHandle_t hLayout, int32 nNumBindings, const RenderDescriptorBinding_t *pDescriptorBindings ) = 0;
	virtual void UpdateConstantBuffer( ConstantBufferHandle_t hConstantBuffer, const void *pData, uint32 nNumBytes ) = 0;
	virtual void UpdateMultiViewConstantBuffer( ConstantBufferHandle_t hConstantBuffer, uint32 nFirstView, uint32 nViewCount, const void **ppData, uint32 nNumBytes ) = 0;
	virtual void *LockDynamicConstantBuffer( uint32 nNumBytes, void **ppData ) = 0;
	virtual void UnlockDynamicConstantBuffer( void *hBuffer, uint32 nNumBytes ) = 0;
	virtual void *LockMultiViewDynamicConstantBuffer( uint32 nNumBytes, uint32 nViewCount, void **ppData ) = 0;
	// Records the buffer for release like UnlockDynamicConstantBuffer, with one more value
	virtual void UnlockMultiViewDynamicConstantBuffer( void *hBuffer, int nViewCount ) = 0;
	virtual void SetConstantBuffer( int nShaderType, ConstantBufferHandle_t hConstantBuffer, int nSlot ) = 0;
	virtual void SetMultiViewConstantBuffer( int nShaderType, ConstantBufferHandle_t hConstantBuffer, uint32 nView, int nSlot ) = 0;
	virtual void SetPushConstants( const void *pData, uint32 nNumBytes ) = 0;
	virtual RsRasterizerStateHandle_t FindOrCreateRasterizerState( const RsRasterizerStateDesc_t *pDesc ) = 0;
	virtual void SetRasterizerState( RsRasterizerStateHandle_t hState ) = 0;
	virtual void SetRasterizerStateOverride( RsRasterizerStateHandle_t hState ) = 0;
	virtual RsDepthStencilStateHandle_t FindOrCreateDepthStencilState( const RsDepthStencilStateDesc_t *pDesc ) = 0;
	virtual void SetDepthStencilState( RsDepthStencilStateHandle_t hState, uint32 nStencilRef ) = 0;
	virtual void SetDepthStencilStateOverride( RsDepthStencilStateHandle_t hState, uint32 nStencilRef ) = 0;
	virtual RsBlendStateHandle_t FindOrCreateBlendState( const RsBlendStateDesc_t *pDesc ) = 0;
	virtual void SetBlendState( RsBlendStateHandle_t hState, const float *pBlendFactor, uint32 nSampleMask ) = 0;
	virtual void SetBlendStateOverride( RsBlendStateHandle_t hState, const float *pBlendFactor, uint32 nSampleMask ) = 0;
	// Selects one of the render device's predefined states
	virtual void SetStandardRasterizerState( int nState ) = 0;
	// Selects the alternate front face winding of the rasterizer state
	virtual void SetFrontFaceFlip( bool bFlip ) = 0;
	virtual void SetStandardBlendState( int nState, const float *pBlendFactor ) = 0;
	virtual void SetStandardDepthStencilState( int nState ) = 0;
	virtual void GetMaxDynamicVertexAndIndexCounts( uint32 nVertexSize, uint32 nIndexSize, int *pMaxVertices, int *pMaxIndices ) = 0;
	// Returns the backend buffer object, NULL for a deleted buffer
	virtual void *GetDeviceSpecificVertexBuffer( VertexBufferHandle_t hVertexBuffer ) = 0;
	// Creates a per-context dynamic buffer and locks desc.m_nElementCount * desc.m_nElementSizeInBytes bytes of it
	virtual VertexBufferHandle_t CreateDynamicVertexBuffer( const BufferDesc_t &desc, void **ppData ) = 0;
	virtual void UnlockDynamicVertexBuffer( VertexBufferHandle_t hVertexBuffer, uint32 nNumBytes ) = 0;
	virtual IndexBufferHandle_t CreateDynamicIndexBuffer( const BufferDesc_t &desc, void **ppData ) = 0;
	virtual void UnlockDynamicIndexBuffer( IndexBufferHandle_t hIndexBuffer, uint32 nNumBytes ) = 0;
	virtual RenderBufferHandle_t CreateDynamicGPUBuffer( const BufferDesc_t &desc, int nFlags, ImageFormat nFormat, void **ppData ) = 0;
	virtual void UnlockDynamicGPUBuffer( RenderBufferHandle_t hBuffer, uint32 nNumBytes ) = 0;
	virtual bool LockIndexBuffer( IndexBufferHandle_t hIndexBuffer, uint32 nNumBytes, void **ppData ) = 0;
	virtual void UnlockIndexBuffer( IndexBufferHandle_t hIndexBuffer, uint32 nNumBytes ) = 0;
	virtual bool LockVertexBuffer( VertexBufferHandle_t hVertexBuffer, uint32 nNumBytes, void **ppData ) = 0;
	virtual void UnlockVertexBuffer( VertexBufferHandle_t hVertexBuffer, uint32 nNumBytes ) = 0;
	virtual bool LockVertexBufferRange( VertexBufferHandle_t hVertexBuffer, uint32 nOffset, uint32 nNumBytes, void **ppData ) = 0;
	virtual void UnlockVertexBufferRange( VertexBufferHandle_t hVertexBuffer, uint32 nOffset, uint32 nNumBytes, void **ppData ) = 0;
	virtual bool LockIndexBufferRange( IndexBufferHandle_t hIndexBuffer, uint32 nOffset, uint32 nNumBytes, void **ppData ) = 0;
	virtual void UnlockIndexBufferRange( IndexBufferHandle_t hIndexBuffer, uint32 nOffset, uint32 nNumBytes, void **ppData ) = 0;
	// pDataRecycleDelegate is invoked with pData and pRecycleContext once the data has been consumed
	virtual void SetVertexBufferData( VertexBufferHandle_t hVertexBuffer, const void *pData, size_t nNumBytes, size_t nOffset, const DataRecycleDelegate_t *pDataRecycleDelegate, void *pRecycleContext ) = 0;
	virtual void SetVertexBufferData( VertexBufferHandle_t hVertexBuffer, const void *pData, size_t nNumBytes, size_t nOffset ) = 0;
	// The index size comes from the buffer
	virtual bool BindIndexBuffer( IndexBufferHandle_t hIndexBuffer, int nOffset ) = 0;
	// Used with the buffers of CreateDynamicIndexBuffer, forwards to BindIndexBuffer
	virtual bool BindDynamicIndexBuffer( IndexBufferHandle_t hIndexBuffer, int nOffset ) = 0;
	virtual bool BindIndexBuffer( RenderBufferHandle_t hBuffer, int nIndexSize, int nOffset ) = 0;
	virtual bool BindVertexBuffer( int nSlot, VertexBufferHandle_t hVertexBuffer, int nOffset, int nStride ) = 0;
	// Used with the buffers of CreateDynamicVertexBuffer, forwards to BindVertexBuffer
	virtual bool BindDynamicVertexBuffer( int nSlot, VertexBufferHandle_t hVertexBuffer, int nOffset, int nStride ) = 0;
	// Same body as the VertexBufferHandle_t overload
	virtual bool BindVertexBuffer( int nSlot, RenderBufferHandle_t hBuffer, int nOffset, int nStride ) = 0;
	// Recorded by IRenderDevice::AsyncSetTextureData with the same arguments
	virtual void AsyncSetTextureData( HRenderTexture hTexture, const CTextureDesc *pDataDesc, const void *pData, int nDataSize, bool bUnk, int nSpecificMipLevelToSet, const Rect3D_t *pSubRectToUpdate, uint32 nTextureUpdateFlags, const DataRecycleDelegate_t *pDataRecycleDelegate, uint32 nAsyncTextureUpdateFlags ) = 0;
	virtual void ReadTexturePixelsAsync( HRenderTexture hTexture, std::shared_ptr< IReadTexturePixelsCallback > pCallback, int nSlice, int nMipLevel, const Rect_t *pSrcRect ) = 0;
	// Records a command that makes the render thread retire the submissions whose fences have completed
	virtual void RetireCompletedSubmissions() = 0;
	virtual void ReadBuffer( RenderBufferHandle_t hBuffer, std::shared_ptr< IReadBufferCallback > pCallback, uint32 nOffset, uint32 nNumBytes ) = 0;
	virtual void *QueuePolledBufferReadback( RenderBufferHandle_t hBuffer, uint32 nOffset, uint32 nNumBytes ) = 0;
	virtual void ReadBufferIndirect( RenderBufferHandle_t hBuffer, uint32 nUnused, RenderBufferHandle_t hIndirectBuffer, uint32 nIndirectOffset, std::shared_ptr< IReadBufferCallback > pCallback ) = 0;
	virtual void GenerateMipmaps( HRenderTexture hTexture ) = 0;
	virtual void CopyTexture( HRenderTexture hSrcTexture, HRenderTexture hDstTexture, const Rect_t *pSrcRect, int nDstX, int nDstY, int nSrcMipLevel, int nSrcSlice, int nDstMipLevel, int nDstSlice ) = 0;
	virtual void CopyFromVolumeTexture( HRenderTexture hSrcTexture, HRenderTexture hDstTexture, const Rect3D_t *pSrcBox, int nDstX, int nDstY, int nDstZ, int nSrcMipLevel, int nSrcSlice, int nDstMipLevel, int nDstSlice ) = 0;
	// Forwards to the RenderBufferHandle_t overload
	virtual void CopyBuffer( VertexBufferHandle_t hDstBuffer, uint32 nDstOffset, VertexBufferHandle_t hSrcBuffer ) = 0;
	// Copies the whole of hSrcBuffer
	virtual void CopyBuffer( RenderBufferHandle_t hDstBuffer, uint32 nDstOffset, RenderBufferHandle_t hSrcBuffer ) = 0;
	virtual void CopyBuffer( RenderBufferHandle_t hDstBuffer, uint32 nDstOffset, RenderBufferHandle_t hSrcBuffer, uint32 nSrcOffset, uint64 nNumBytes ) = 0;
	virtual void FillFallbackTextures() = 0;
	virtual void Draw( int nPrimitiveType, int nFirstVertex, int nVertexCount ) = 0;
	virtual void DrawInstanced( int nPrimitiveType, int nFirstVertex, int nVertexCount, int nInstanceCount ) = 0;
	virtual void DrawIndexed( int nPrimitiveType, int nFirstIndex, int nIndexCount, int nUnk, int nBaseVertex ) = 0;
	virtual void DrawIndexedInstanced( int nPrimitiveType, int nFirstIndex, int nIndexCount, int nInstanceCount, int nUnk, int nBaseVertex, int nFirstInstance ) = 0;
	virtual void MultiDrawIndexed( int nPrimitiveType, int nDrawCount, const uint32 *pDraws, int nInstanceCount, int nFirstInstance ) = 0;
	virtual void DrawIndirect( int nPrimitiveType, RenderResourceHandle_t hBuffer, uint32 nOffset ) = 0;
	virtual void DrawIndexedIndirect( int nPrimitiveType, RenderResourceHandle_t hBuffer, uint32 nOffset ) = 0;
	virtual void MultiDrawIndexedIndirect( int nPrimitiveType, int nDrawCount, RenderResourceHandle_t hBuffer, uint32 nOffset ) = 0;
	virtual void DrawIndexedIndirectCount( int nPrimitiveType, RenderResourceHandle_t hBuffer, uint32 nOffset, int nMaxDrawCount, RenderResourceHandle_t hCountBuffer, uint32 nCountOffset ) = 0;
	virtual void Dispatch( int nThreadCountX, int nThreadCountY, int nThreadCountZ ) = 0;
	virtual void DispatchIndirect( RenderResourceHandle_t hBuffer, uint32 nOffset ) = 0;
	virtual void BuildBLAS( int nCount, const BLASBuildDesc_t *pBuildDescs ) = 0;
	// Queries the compacted size of the BLAS
	virtual void RequestBLASCompaction( RenderBLASHandle_t hBLAS ) = 0;
	virtual void CompactBLAS( RenderBLASHandle_t hBLAS ) = 0;
	// A NULL hScratchBuffer makes the build allocate its own scratch memory
	virtual void BuildTLAS( RenderTLASHandle_t hTLAS, const TLASInstance_t *pInstances, int nInstanceCount, RenderBufferHandle_t hScratchBuffer, uint32 nScratchOffset ) = 0;
	virtual void BindRayTracePipeline( RenderRayTracePipelineHandle_t hPipeline ) = 0;
	virtual void UploadRayTraceShaderBindingTable( IRaytraceShaderBindingTable *pSBT ) = 0;
	virtual void TraceRays( IRaytraceShaderBindingTable *pSBT, uint32 nWidth, uint32 nHeight, uint32 nDepth ) = 0;
	virtual void TraceRaysIndirect( IRaytraceShaderBindingTable *pSBT, RenderBufferHandle_t hArgsBuffer, uint32 nOffset ) = 0;
	virtual void BuildOpacityMicromaps( int nCount, const OpacityMicromapBuildDesc_t *pBuildDescs ) = 0;
	// Fills the whole buffer with nValue (vkCmdFillBuffer) between the given access and stage masks
	virtual void FillBuffer( RenderBufferHandle_t hBuffer, uint32 nValue, uint32 nSrcAccessMask, uint32 nSrcStageMask, uint32 nDstAccessMask, uint32 nDstStageMask ) = 0;
	virtual void SetIndexBufferData( IndexBufferHandle_t hIndexBuffer, const void *pData, size_t nNumBytes, size_t nOffset, const DataRecycleDelegate_t *pDataRecycleDelegate, void *pRecycleContext ) = 0;
	// Submits the display list and waits for the GPU
	virtual void ForceFlushGPU() = 0;
	// Flushes and waits for the GPU on a context of its own, then frees the resources pending deletion
	virtual void FlushGPUAndFreeDeletedResources() = 0;
	virtual void unk113() = 0;
	virtual IRenderDevice *GetRenderDevice() = 0;
	virtual void BeginPIXEvent( uint32 nColor, const char *pName ) = 0;
	virtual void BeginPIXEvent( const char *pName ) = 0;
	virtual void EndPIXEvent() = 0;
	virtual void SetPIXMarker( uint32 nColor, const char *pName ) = 0;
	virtual void SetPIXMarker( const char *pName ) = 0;
	// bRemove sends the keys with empty values, removing them
	virtual void SetRenderDocCommandAnnotations( const std::function< CUtlVector< RenderDocAnnotation_t >() > &fnGetAnnotations, bool bRemove ) {}
	virtual void SetRenderDocObjectAnnotations( void *pObject, const std::function< CUtlVector< RenderDocAnnotation_t >() > &fnGetAnnotations ) {}
	virtual void BeginQuery( RenderQueryHandle_t hQuery ) = 0;
	virtual void EndQuery( RenderQueryHandle_t hQuery ) = 0;
	virtual void SetDepthBoundsTestEnable( bool bEnable ) = 0;
	virtual void SetDepthBounds( float flMinDepth, float flMaxDepth ) = 0;
	virtual std::unique_ptr< IBarrierHelper > GetBarrierHelper() = 0;
	virtual void TransitionTextures( int nCount, const HRenderTexture *pTextures, const uint32 *pLayouts ) = 0;
	virtual void TextureBarriers( int nCount, const RenderTextureBarrier_t *pBarriers ) = 0;
	virtual void BufferBarriers( int nCount, const RenderBufferBarrier_t *pBarriers ) = 0;
	virtual void MemoryBarriers( int nCount, const RenderMemoryBarrier_t *pBarriers ) = 0;
	// Transitions the textures from the undefined layout, dropping their contents
	virtual void DiscardTextures( int nCount, const HRenderTexture *pTextures, const uint32 *pLayouts ) = 0;
	// Records the pending barriers of the textures before the following commands
	virtual void FlushTextureBarriers( int nCount, const HRenderTexture *pTextures ) = 0;
	virtual void SubmitBarrierBatch( const RenderBarrierBatch_t &batch ) = 0;
	// While the top of this stack is true, buffer copies and fills skip their automatic barriers
	virtual void PushManualBufferBarriers( bool bManual ) = 0;
	virtual void PopManualBufferBarriers() = 0;
	virtual void unk136() = 0;
	virtual void unk137() = 0;
	virtual void unk138() = 0;
	virtual const char *GetDebugName() = 0;
	// The render thread calls the callback when it reaches the command
	virtual void QueueCommandCallback( ICommandCallback *pCallback ) = 0;
	// Returns the semaphore the display list signals when it completes, created on first use
	virtual RenderSemaphoreHandle_t SemaphoreSignalAtEnd( const char *pDebugName ) = 0;
	virtual void SemaphoreWaitAtBegin( RenderSemaphoreHandle_t hSemaphore ) = 0;
	// Device mask of the following submissions; 23294 restores the previous mask and 4269 reuses it
	virtual void SetMultiGPUDeviceMask( uint32 nDeviceMask ) = 0;
	// Recorded for the render thread when multiple GPUs are linked: blits pSrcRect of hSrcTexture into pDstRect
	// of hDstTexture with device group submits on the given GPU indices; nUnused0/nUnused1 are recorded but not read
	virtual void BlitTextureAcrossGPUs( HRenderTexture hSrcTexture, HRenderTexture hDstTexture, int nSecondPassGPU, int nFirstPassGPU, int nUnused0, int nUnused1, const Rect_t *pSrcRect, const Rect_t *pDstRect, bool bFirstPass, bool bSecondPass ) = 0;
	// Returns the context's render attribute block; libmaterialsystem2 sets texture and integer attributes in the
	// containers at +0x110 and +0x180 of it
	virtual void *GetRenderAttributes() = 0;
	virtual bool BindPooledIndexBuffer( PooledIndexBufferHandle_t hIndexBuffer ) = 0;
	virtual bool BindPooledVertexBuffer( int nSlot, RenderResourceHandle_t hPooledVertexBuffer ) = 0;
	virtual void ExecuteGeneratedCommands( void *hIndirectCommandsLayout, void *hIndirectExecutionSet, RenderBufferHandle_t hIndirectBuffer, bool bIsPreprocessed, RenderBufferHandle_t hPreprocessBuffer, uint32 nMaxSequenceCount, RenderBufferHandle_t hSequenceCountBuffer, uint32 nMaxDrawCount ) = 0;
	virtual void PreprocessGeneratedCommands( void *hIndirectCommandsLayout, void *hIndirectExecutionSet, RenderBufferHandle_t hIndirectBuffer, RenderBufferHandle_t hPreprocessBuffer, uint32 nMaxSequenceCount, RenderBufferHandle_t hSequenceCountBuffer, uint32 nMaxDrawCount ) = 0;
	virtual void DrawMeshTasks( uint32 nGroupCountX, uint32 nGroupCountY, uint32 nGroupCountZ ) = 0;
	virtual void DrawMeshTasksIndirect( RenderResourceHandle_t hBuffer, uint32 nOffset, uint32 nDrawCount, uint32 nStride ) = 0;
	virtual void DrawMeshTasksIndirectCount( RenderResourceHandle_t hBuffer, uint32 nOffset, RenderResourceHandle_t hCountBuffer, uint32 nCountOffset, uint32 nMaxDrawCount ) = 0;
	virtual void BeginConditionalRendering( RenderResourceHandle_t hBuffer, uint32 nOffset ) = 0;
	virtual void EndConditionalRendering() = 0;
	virtual void unk155() = 0;
	virtual void SetFragmentShadingRate( uint32 nWidth, uint32 nHeight ) = 0;
	virtual ~IRenderContext() {}
	virtual bool BindIndexBuffer( IndexBufferHandle_t hIndexBuffer, int nOffset, int nIndexSize ) = 0;
};

#endif // IRENDERCONTEXT_H
