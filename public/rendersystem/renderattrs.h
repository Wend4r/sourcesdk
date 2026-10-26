//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: Named shader attributes (constants, textures, buffers) a draw binds
//
//===========================================================================//

#ifndef RENDERATTRS_H
#define RENDERATTRS_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlattributelist.h"
#include "tier0/utlstring.h"
#include "mathlib/vector4d.h"
#include "mathlib/vmatrix.h"
#include "resourcesystem/resourcehandle.h"

FORWARD_DECLARE_HANDLE( ConstantBufferHandle_t );
FORWARD_DECLARE_HANDLE( RenderDescriptorSetHandle_t );

class CRenderAttributes
{
public:
	struct TextureAttrForStorage_t
	{
		// HRenderTexture
		CWeakHandleBase texture;

		// -1 binds every mip
		int32 nSpecificMipLevelToBind;
	};

	struct TextureAttr_t
	{
		// HRenderTexture
		CWeakHandleBase texture;
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

	typedef CUtlAttributeList< TextureAttr_t, 8, TextureAttrForStorage_t, 8 > TextureAttrList_t;

	// Float, Vector2D, Vector and Vector4D attributes
	CUtlAttributeList< Vector4D, 8, Vector4D, 4 > m_vec4Attrs;
	CUtlAttributeList< VMatrix, 1, VMatrix, 2 > m_matrix4x4Attrs;

	// Bool and int attributes
	CUtlAttributeList< int, 12 > m_intAttrs;

	TextureAttrList_t m_textureAttrs;
	CUtlAttributeList< ConstantBufferAttr_t, 4 > m_constantBufferAttrs;
	CUtlAttributeList< DescriptorSetAttr_t, 4 > m_descriptorSetAttrs;
	CUtlAttributeList< void *, 4 > m_ptrAttrs;
	CUtlAttributeList< CUtlString, 2 > m_stringAttrs;

	// Searched when an attribute is not set here
	const CRenderAttributes *m_pParent;

	// Combined hash of the set attributes, updated as attributes are set
	uint64 m_nHash;

	// Shader type of the geometry stage the draws use: 1 (vertex shader) by default, 4 for a mesh shader, which
	// CMaterialSystem2::SetRenderStateForMode then requires from the material
	int m_nGeometryShaderType;

private:
	// Freed by the destructor, nothing in the shipped modules sets or reads it
	CUtlString m_unk398;
};

#endif // RENDERATTRS_H
