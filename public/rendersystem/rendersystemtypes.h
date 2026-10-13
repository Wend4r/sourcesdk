#ifndef RENDERSYSTEM_RENDERSYSTEMTYPES_H
#define RENDERSYSTEM_RENDERSYSTEMTYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/basetypes.h"
#include "network_connection.pb.h"

FORWARD_DECLARE_HANDLE( RenderInputLayout_t );

DECLARE_POINTER_HANDLE( RenderResourceHandle_t );

FORWARD_DECLARE_HANDLE( SwapChainHandle_t );

DECLARE_DERIVED_POINTER_HANDLE( VertexBufferHandle_t, RenderResourceHandle_t );
DECLARE_DERIVED_POINTER_HANDLE( IndexBufferHandle_t, RenderResourceHandle_t );
DECLARE_DERIVED_POINTER_HANDLE( RenderBufferHandle_t, RenderResourceHandle_t );
DECLARE_DERIVED_POINTER_HANDLE( PooledIndexBufferHandle_t, RenderResourceHandle_t );
DECLARE_DERIVED_POINTER_HANDLE( ConstantBufferHandle_t, RenderResourceHandle_t );
DECLARE_DERIVED_POINTER_HANDLE( RenderShaderHandle_t, RenderResourceHandle_t );
DECLARE_DERIVED_POINTER_HANDLE( RenderSemaphoreHandle_t, RenderResourceHandle_t );
DECLARE_DERIVED_POINTER_HANDLE( RenderDescriptorSetHandle_t, RenderResourceHandle_t );

DECLARE_POINTER_HANDLE( SamplerStateHandle_t );

DECLARE_DERIVED_POINTER_HANDLE( RenderBLASHandle_t, RenderResourceHandle_t );
DECLARE_DERIVED_POINTER_HANDLE( RenderTLASHandle_t, RenderResourceHandle_t );
DECLARE_DERIVED_POINTER_HANDLE( RenderRayTracePipelineHandle_t, RenderResourceHandle_t );

enum RenderMultisampleType_t : uint8
{
	RENDER_MULTISAMPLE_INVALID = 0xFF,
	RENDER_MULTISAMPLE_NONE = 0,
	RENDER_MULTISAMPLE_2X,
	RENDER_MULTISAMPLE_4X,
	RENDER_MULTISAMPLE_6X,
	RENDER_MULTISAMPLE_8X,
	RENDER_MULTISAMPLE_16X,
	RENDER_MULTISAMPLE_TYPE_COUNT
};

struct RenderViewport_t
{
	int m_nVersion;
	int m_nTopLeftX;
	int m_nTopLeftY;
	int m_nWidth;
	int m_nHeight;
	float m_flMinZ;
	float m_flMaxZ;
};

#endif // RENDERSYSTEM_RENDERSYSTEMTYPES_H
