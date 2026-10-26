#ifndef RENDERSYSTEM_RENDERSYSTEMTYPES_H
#define RENDERSYSTEM_RENDERSYSTEMTYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/basetypes.h"
#include "tier0/utlvector.h"
#include "network_connection.pb.h"

#include "schemacompiler/schemaon.h"

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

schema enum RenderSlotType_t : int8
{
	RENDER_SLOT_INVALID = -1,
	RENDER_SLOT_PER_VERTEX = 0,
	RENDER_SLOT_PER_INSTANCE,
};

schema enum InputLayoutVariation_t : uint8
{
	INPUT_LAYOUT_VARIATION_DEFAULT = 0,
	INPUT_LAYOUT_VARIATION_STREAM1_INSTANCEID,
	INPUT_LAYOUT_VARIATION_STREAM1_INSTANCEID_MORPH_VERT_ID,
	INPUT_LAYOUT_VARIATION_MAX,
};

schema enum RenderPrimitiveType_t : int32
{
	RENDER_PRIM_POINTS = 0,
	RENDER_PRIM_LINES,
	RENDER_PRIM_LINES_WITH_ADJACENCY,
	RENDER_PRIM_LINE_STRIP,
	RENDER_PRIM_LINE_STRIP_WITH_ADJACENCY,
	RENDER_PRIM_TRIANGLES,
	RENDER_PRIM_TRIANGLES_WITH_ADJACENCY,
	RENDER_PRIM_TRIANGLE_STRIP,
	RENDER_PRIM_TRIANGLE_STRIP_WITH_ADJACENCY,
	RENDER_PRIM_INSTANCED_QUADS,
	RENDER_PRIM_HETEROGENOUS,
	RENDER_PRIM_COMPUTE_SHADER,
	RENDER_PRIM_MESH_SHADER,
	RENDER_PRIM_TYPE_COUNT,
};

// Usage flags of the vertex, index and GPU buffers
schema enum RenderBufferFlags_t : uint32
{
	RENDER_BUFFER_USAGE_NONE = 0,
	RENDER_BUFFER_USAGE_VERTEX_BUFFER = ( 1 << 0 ),
	RENDER_BUFFER_USAGE_INDEX_BUFFER = ( 1 << 1 ),
	RENDER_BUFFER_USAGE_SHADER_RESOURCE = ( 1 << 2 ),
	RENDER_BUFFER_USAGE_UNORDERED_ACCESS = ( 1 << 3 ),
	RENDER_BUFFER_BYTEADDRESS_BUFFER = ( 1 << 4 ),
	RENDER_BUFFER_STRUCTURED_BUFFER = ( 1 << 5 ),
	RENDER_BUFFER_UAV_DRAW_INDIRECT_ARGS = ( 1 << 8 ),
	RENDER_BUFFER_ACCELERATION_STRUCTURE = ( 1 << 9 ),
	RENDER_BUFFER_SHADER_BINDING_TABLE = ( 1 << 10 ),
	RENDER_BUFFER_POOL_ALLOCATED = ( 1 << 11 ),
	RENDER_BUFFER_USAGE_CONDITIONAL_RENDERING = ( 1 << 12 ),
	RENDER_BUFFER_IMMOVABLE_ALLOCATION = ( 1 << 13 ),
	RENDER_BUFFER_DYNAMIC_ZERO_COPY = ( 1 << 14 ),
};

// Comparison of the depth, stencil and sampler states; the closer/farther variants follow the depth direction
schema enum RsComparison_t : uint8
{
	RS_CMP_NEVER = 0,
	RS_CMP_LESS,
	RS_CMP_EQUAL,
	RS_CMP_LESS_EQUAL,
	RS_CMP_GREATER,
	RS_CMP_NOT_EQUAL,
	RS_CMP_GREATER_EQUAL,
	RS_CMP_ALWAYS,
	RS_CMP_FUNC_MASK = 7,
	RS_CMP_CLOSER_FARTHER_FLAG = 8,
	RS_CMP_CLOSER = 9,
	RS_CMP_CLOSER_EQUAL = 11,
	RS_CMP_FARTHER = 12,
	RS_CMP_FARTHER_EQUAL = 14,
};

schema enum UpscalerType_t : int32
{
	UPSCALER_NONE = 0,
	UPSCALER_AMD_FSR2,
	UPSCALER_AMD_FSR3,
	UPSCALER_NVIDIA_DLSS,
	UPSCALER_INTEL_XESS,
	UPSCALER_COUNT,
};

// One vertex attribute of an input layout
schema struct RenderInputLayoutField_t
{
	char m_pSemanticName[ 32 ];
	int8 m_nSemanticIndex;
	noschema uint32 m_Format;
	int16 m_nOffset;
	int8 m_nSlot;
	RenderSlotType_t m_nSlotType;
	char m_szShaderSemantic[ 32 ];
};

schema struct VsInputSignatureElement_t
{
	char m_pName[ 64 ];
	char m_pSemantic[ 64 ];
	char m_pD3DSemanticName[ 64 ];
	int32 m_nD3DSemanticIndex;
};

schema struct VsInputSignature_t
{
	CUtlVector< VsInputSignatureElement_t > m_elems;
	CUtlVector< VsInputSignatureElement_t > m_depth_elems;
};

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

#include "schemacompiler/schemaoff.h"

#endif // RENDERSYSTEM_RENDERSYSTEMTYPES_H
