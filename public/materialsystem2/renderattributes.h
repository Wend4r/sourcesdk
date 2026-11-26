//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A set of render attributes: the values a material or a view hands to the shaders,
// keyed by the token of the attribute name.
//
//=============================================================================//

#ifndef RENDERATTRIBUTES_H
#define RENDERATTRIBUTES_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/basetypes.h"
#include "tier0/utlattributelist.h"
#include "tier0/utlstring.h"
#include "mathlib/vector4d.h"
#include "mathlib/vmatrix.h"
#include "resourcefile/resourcetype.h"

FORWARD_DECLARE_HANDLE( ConstantBufferHandle_t );
FORWARD_DECLARE_HANDLE( RenderDescriptorSetHandle_t );

//-----------------------------------------------------------------------------
// 928 bytes on both platforms, 16-byte aligned. Each kind of value has its own attribute list (a
// fixed chunk of keys and values, then overflow chunks); a lookup that misses falls back to
// m_pParent. Only the members are declared; the methods live in the material system.
//-----------------------------------------------------------------------------
class CRenderAttributes
{
public:
	struct TextureAttr_t
	{
		// A CWeakHandle< InfoForResourceTypeCTextureBase > (HRenderTexture) in the game
		ResourceHandle_t texture;
		int32 nSpecificMipLevelToBind;
	};

	// How a texture value is stored: the same members, the mip level -1 when no specific one is bound
	struct TextureAttrForStorage_t
	{
		// A CWeakHandle< InfoForResourceTypeCTextureBase > (HRenderTexture) in the game
		ResourceHandle_t texture;
		int32 nSpecificMipLevelToBind;
	};

	struct ConstantBufferAttr_t
	{
		ConstantBufferHandle_t hBuffer;
		uint32 nView;
	};

	struct DescriptorSetAttr_t
	{
		RenderDescriptorSetHandle_t hDescriptorSet;
		// -1 when unused
		int32 nCbufferOffsets[ 4 ];
	};

	CUtlAttributeList< Vector4D, 8, Vector4D, 4 > m_vec4Attrs;
	CUtlAttributeList< VMatrix, 1, VMatrix, 2 > m_matrix4x4Attrs;
	CUtlAttributeList< int, 12, int, 12 > m_intAttrs;
	CUtlAttributeList< TextureAttr_t, 8, TextureAttrForStorage_t, 8 > m_textureAttrs;
	CUtlAttributeList< ConstantBufferAttr_t, 4, ConstantBufferAttr_t, 4 > m_constantBufferAttrs;
	CUtlAttributeList< DescriptorSetAttr_t, 4, DescriptorSetAttr_t, 4 > m_descriptorSetAttrs;
	CUtlAttributeList< void *, 4, void *, 4 > m_ptrAttrs;
	CUtlAttributeList< CUtlString, 2, CUtlString, 2 > m_stringAttrs;

	const CRenderAttributes *m_pParent;

	// Named by what it holds: a hash of the values, updated by every set; the lookups that cache
	// render state fold it with the hashes of the parents
	uint64 m_nHash;

	// Named by what it holds: 1 by default; the material system takes another shader lookup path
	// for a mode when it is 4
	int32 m_nShaderLookupMode;

	// Constructed, cleared and freed only; nothing reads its text
	CUtlString m_strReserved;
};

#endif // RENDERATTRIBUTES_H
