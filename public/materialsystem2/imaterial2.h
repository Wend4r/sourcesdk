#ifndef IMATERIAL2_H
#define IMATERIAL2_H

#ifdef _WIN32
	#pragma once
#endif

#include <resourcesystem/resourcehandle.h>
#include <resourcefile/resourcetype.h>

#include <mathlib/vector4d.h>
#include <tier0/platform.h>
#include <tier0/utlsymbollarge.h>
#include <tier0/utlvector.h>

//-----------------------------------------------------------------------------
// A single shader parameter of a material. Textures are stored as a resource
// handle in m_hTexture and flagged with MATERIAL_PARAM_TEXTURE.
//-----------------------------------------------------------------------------
enum MaterialParamFlags_t : uint32
{
	MATERIAL_PARAM_TEXTURE = 0x10000,
};

struct MaterialParam_t
{
	Vector4D m_vValue;
	ResourceHandle_t m_hTexture;
	uint32 m_nValueHash;
	uint32 m_nFlags;
	void *m_pBlob;
	CUtlSymbolLarge m_symName;
	CUtlSymbolLarge m_symString;
	bool m_bShaderDefault;

private:
	uint8 m_nUnknown0039[7];
};

COMPILE_TIME_ASSERT( sizeof( MaterialParam_t ) == 64 );

//-----------------------------------------------------------------------------
// A loaded Source 2 material.
//
// Reconstructed layout. The vtable is declared flat: every slot is kept in its
// verified index, and slots whose purpose is unknown are placeholders named
// after their index. Do not reorder, remove or collapse them. There is no
// virtual destructor in slot 0 - GetName() owns it.
//
// Only the field offsets marked below are verified, and the total size of the
// class is NOT verified.
//-----------------------------------------------------------------------------
abstract_class IMaterial2
{
public:
	virtual const char *GetName() = 0;
	virtual void Unk_Slot1( void *p ) = 0;
	virtual bool IsLoaded() = 0;
	virtual void Unk_Slot3( void *p ) = 0;
	virtual void Unk_Slot4( void *p ) = 0;
	virtual void Unk_Slot5( void *p ) = 0;
	virtual void Unk_Slot6( void *p ) = 0;
	virtual void Unk_Slot7( void *p ) = 0;
	virtual void Unk_Slot8( void *p ) = 0;
	virtual void Unk_Slot9( void *p ) = 0;
	virtual void Unk_Slot10( void *p ) = 0;
	virtual void Unk_Slot11( void *p ) = 0;
	virtual void Unk_Slot12( void *p ) = 0;
	virtual void Unk_Slot13( void *p ) = 0;
	virtual void Unk_Slot14( void *p ) = 0;
	virtual void Unk_Slot15( void *p ) = 0;
	virtual void Unk_Slot16( void *p ) = 0;
	virtual void Unk_Slot17( void *p ) = 0;
	virtual void Unk_Slot18( void *p ) = 0;
	virtual void Unk_Slot19( void *p ) = 0;
	virtual void Unk_Slot20( void *p ) = 0;
	virtual void Unk_Slot21( void *p ) = 0;
	virtual void Unk_Slot22( void *p ) = 0;
	virtual void Unk_Slot23( void *p ) = 0;
	virtual void Unk_Slot24( void *p ) = 0;
	virtual void Unk_Slot25( void *p ) = 0;
	virtual void Unk_Slot26( void *p ) = 0;
	virtual void Unk_Slot27( void *p ) = 0;
	virtual void Unk_Slot28( void *p ) = 0;
	virtual void Unk_Slot29( void *p ) = 0;
	virtual void Unk_Slot30( void *p ) = 0;
	virtual int GetIntParam( const char *pName, int nFallback ) = 0;
	virtual float GetFloatParam( const char *pName, float flFallback ) = 0;
	virtual const char *GetStringParam( const char *pName, const char *pFallback ) = 0;
	virtual ResourceHandle_t *GetTextureParam( ResourceHandle_t *pOut, const char *pName ) = 0;
	virtual Vector4D *GetVecParam( Vector4D *pOut, const char *pName, const Vector4D *pFallback ) = 0;

private:
	uint8 m_nUnknown0008[8];

public:
	const char *m_pPath;
	CUtlVector< MaterialParam_t > m_Params;
};

class InfoForResourceTypeIMaterial2
{
public:
	using RuntimeClass_t = IMaterial2;
};

using HMaterialStrong = CStrongHandle< InfoForResourceTypeIMaterial2 >;
using HMaterialWeak = CWeakHandle< InfoForResourceTypeIMaterial2 >;

#endif // IMATERIAL2_H
