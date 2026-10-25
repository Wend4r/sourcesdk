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

#include <memory>

class IBarrierHelper;

// Created by IRenderDevice::CreateRenderContext. Slots whose purpose is not known yet are
// placeholders named after their vtable index
abstract_class IRenderContext
{
public:
	virtual void unk000() = 0;
	virtual void unk001() = 0;
	virtual void unk002() = 0;
	virtual void unk003() = 0;
	virtual void unk004() = 0;
	virtual void ClearUnorderedAccessViewUint( HRenderTexture hTexture, int nUnk, const uint32 *pValues ) = 0;
	virtual void ClearUnorderedAccessViewFloat( HRenderTexture hTexture, int nUnk, const float *pValues ) = 0;
	virtual void unk007() = 0;
	virtual void unk008() = 0;
	virtual void SetViewport( void *pUnk, int nTopLeftX, int nTopLeftY, int nWidth, int nHeight ) = 0;
	// Ends the recording with EndDisplayList and hands the list to IRenderDevice::SubmitDisplayLists
	virtual void SubmitDisplayList() = 0;
	virtual CDisplayList *EndDisplayList() = 0;
	virtual void unk012() = 0;
	virtual void unk013() = 0;
	virtual void GetViewport( RenderViewport_t *pViewport, int nIndex ) = 0;
	virtual void SetViewports( int nCount, const RenderViewport_t *pViewports ) = 0;
	virtual void SetScissorRects( int nCount, const Rect_t *pRects ) = 0;
	virtual void unk017() = 0;
	virtual void unk018() = 0;
	virtual void unk019() = 0;
	virtual void unk020() = 0;
	virtual void unk021() = 0;
	virtual void unk022() = 0;
	virtual void unk023() = 0;
	virtual void unk024() = 0;
	virtual void unk025() = 0;
	virtual void unk026() = 0;
	virtual void unk027() = 0;
	virtual void unk028() = 0;
	virtual void unk029() = 0;
	virtual void unk030() = 0;
	virtual void unk031() = 0;
	virtual void *LockDynamicConstantBuffer( uint32 nNumBytes, void **ppData ) = 0;
	virtual void unk033() = 0;
	virtual void unk034() = 0;
	virtual void unk035() = 0;
	virtual void unk036() = 0;
	virtual void unk037() = 0;
	virtual void unk038() = 0;
	virtual RsRasterizerStateHandle_t FindOrCreateRasterizerState( const RsRasterizerStateDesc_t *pDesc ) = 0;
	virtual void SetRasterizerState( RsRasterizerStateHandle_t hState ) = 0;
	virtual void SetRasterizerStateOverride( RsRasterizerStateHandle_t hState ) = 0;
	virtual RsDepthStencilStateHandle_t FindOrCreateDepthStencilState( const RsDepthStencilStateDesc_t *pDesc ) = 0;
	virtual void SetDepthStencilState( RsDepthStencilStateHandle_t hState, uint32 nStencilRef ) = 0;
	virtual void SetDepthStencilStateOverride( RsDepthStencilStateHandle_t hState, uint32 nStencilRef ) = 0;
	virtual RsBlendStateHandle_t FindOrCreateBlendState( const RsBlendStateDesc_t *pDesc ) = 0;
	virtual void SetBlendState( RsBlendStateHandle_t hState, const float *pBlendFactor, uint32 nSampleMask ) = 0;
	virtual void SetBlendStateOverride( RsBlendStateHandle_t hState, const float *pBlendFactor, uint32 nSampleMask ) = 0;
	virtual void unk048() = 0;
	virtual void unk049() = 0;
	virtual void unk050() = 0;
	virtual void unk051() = 0;
	virtual void unk052() = 0;
	virtual void unk053() = 0;
	virtual void unk054() = 0;
	virtual void unk055() = 0;
	virtual void unk056() = 0;
	virtual void unk057() = 0;
	virtual void unk058() = 0;
	virtual void unk059() = 0;
	virtual void unk060() = 0;
	virtual void unk061() = 0;
	virtual void unk062() = 0;
	virtual void unk063() = 0;
	virtual void unk064() = 0;
	virtual void unk065() = 0;
	virtual void unk066() = 0;
	virtual void unk067() = 0;
	virtual void unk068() = 0;
	virtual void unk069() = 0;
	virtual void unk070() = 0;
	virtual void unk071() = 0;
	virtual void unk072() = 0;
	virtual bool BindVertexBuffer( int nSlot, VertexBufferHandle_t hVertexBuffer, int nOffset, int nStride ) = 0;
	virtual void unk074() = 0;
	virtual void unk075() = 0;
	// Recorded by IRenderDevice::AsyncSetTextureData with the same arguments
	virtual void AsyncSetTextureData( HRenderTexture hTexture, const CTextureDesc *pDataDesc, const void *pData, int nDataSize, bool bUnk, int nSpecificMipLevelToSet, const Rect3D_t *pSubRectToUpdate, uint32 nTextureUpdateFlags, const DataRecycleDelegate_t *pDataRecycleDelegate, uint32 nAsyncTextureUpdateFlags ) = 0;
	virtual void unk077() = 0;
	virtual void unk078() = 0;
	virtual void unk079() = 0;
	virtual void unk080() = 0;
	virtual void unk081() = 0;
	virtual void unk082() = 0;
	virtual void unk083() = 0;
	virtual void unk084() = 0;
	virtual void unk085() = 0;
	virtual void unk086() = 0;
	virtual void unk087() = 0;
	virtual void unk088() = 0;
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
	virtual void unk100() = 0;
	virtual void unk101() = 0;
	virtual void unk102() = 0;
	virtual void unk103() = 0;
	virtual void unk104() = 0;
	virtual void unk105() = 0;
	virtual void unk106() = 0;
	virtual void unk107() = 0;
	virtual void unk108() = 0;
	virtual void unk109() = 0;
	virtual void unk110() = 0;
	virtual void unk111() = 0;
	virtual void unk112() = 0;
	virtual void unk113() = 0;
	virtual IRenderDevice *GetRenderDevice() = 0;
	virtual void BeginPIXEvent( uint32 nColor, const char *pName ) = 0;
	virtual void BeginPIXEvent( const char *pName ) = 0;
	virtual void EndPIXEvent() = 0;
	virtual void SetPIXMarker( uint32 nColor, const char *pName ) = 0;
	virtual void SetPIXMarker( const char *pName ) = 0;
	virtual void unk120() {}
	virtual void unk121() {}
	virtual void BeginQuery( RenderQueryHandle_t hQuery ) = 0;
	virtual void EndQuery( RenderQueryHandle_t hQuery ) = 0;
	virtual void SetDepthBoundsTestEnable( bool bEnable ) = 0;
	virtual void SetDepthBounds( float flMinDepth, float flMaxDepth ) = 0;
	virtual std::unique_ptr< IBarrierHelper > GetBarrierHelper() = 0;
	virtual void unk127() = 0;
	virtual void unk128() = 0;
	virtual void unk129() = 0;
	virtual void unk130() = 0;
	virtual void unk131() = 0;
	virtual void unk132() = 0;
	virtual void unk133() = 0;
	virtual void unk134() = 0;
	virtual void unk135() = 0;
	virtual void unk136() = 0;
	virtual void unk137() = 0;
	virtual void unk138() = 0;
	virtual void unk139() = 0;
	virtual void unk140() = 0;
	virtual void unk141() = 0;
	virtual void unk142() = 0;
	virtual void unk143() = 0;
	virtual void unk144() = 0;
	virtual void unk145() = 0;
	virtual bool BindPooledIndexBuffer( PooledIndexBufferHandle_t hIndexBuffer ) = 0;
	virtual bool BindPooledVertexBuffer( int nSlot, RenderResourceHandle_t hPooledVertexBuffer ) = 0;
	virtual void unk148() = 0;
	virtual void unk149() = 0;
	virtual void DrawMeshTasks( uint32 nGroupCountX, uint32 nGroupCountY, uint32 nGroupCountZ ) = 0;
	virtual void DrawMeshTasksIndirect( RenderResourceHandle_t hBuffer, uint32 nOffset, uint32 nDrawCount, uint32 nStride ) = 0;
	virtual void DrawMeshTasksIndirectCount( RenderResourceHandle_t hBuffer, uint32 nOffset, RenderResourceHandle_t hCountBuffer, uint32 nCountOffset, uint32 nMaxDrawCount ) = 0;
	virtual void BeginConditionalRendering( RenderResourceHandle_t hBuffer, uint32 nOffset ) = 0;
	virtual void EndConditionalRendering() = 0;
	virtual void unk155() = 0;
	virtual void unk156() = 0;
	virtual ~IRenderContext() {}
	virtual bool BindIndexBuffer( IndexBufferHandle_t hIndexBuffer, int nOffset, int nIndexSize ) = 0;
};

#endif // IRENDERCONTEXT_H
