#ifndef MATERIALSYSTEM2_MATERIAL_RESOURCE_DATA_H
#define MATERIALSYSTEM2_MATERIAL_RESOURCE_DATA_H

#ifdef _WIN32
#pragma once
#endif

#include <resourcefile/resourcetype.h>
#include <tier0/utlstringtoken.h>
#include <tier0/utlsymbollarge.h>

//-----------------------------------------------------------------------------
// A shader parameter value: a vector, a texture or a blob. CMaterial2's parameter
// code reads the texture flag at byte 30 bit 0, clears the buffer size as a 16-bit
// word at 28 and frees the buffer with delete[] at 32.
//-----------------------------------------------------------------------------
class CVfxParam
{
public:
	float m_flVec4[ 4 ];

	// A CStrongHandle< InfoForResourceTypeCTextureBase > in the game
	ResourceHandle_t m_textureHandle;
	CUtlStringToken m_stringToken;
	uint32 m_nBufferSize : 16;
	uint32 m_bIsTextureParam : 1;
	void *m_pBuffer;
};

//-----------------------------------------------------------------------------
// A named shader parameter of a loaded material (CMaterial2::m_params).
// The game compares m_name with V_stricmp_fast and reads the shader name from
// m_string of the first parameter. 64 bytes on both platforms.
//-----------------------------------------------------------------------------
class CMaterialParam : public CVfxParam
{
public:
	CUtlSymbolLarge m_name;
	CUtlSymbolLarge m_string;
	bool m_bIsDefaultValue;
};

#endif // MATERIALSYSTEM2_MATERIAL_RESOURCE_DATA_H
