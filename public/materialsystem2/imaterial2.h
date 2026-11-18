#ifndef IMATERIAL2_H
#define IMATERIAL2_H

#ifdef _WIN32
	#pragma once
#endif

#include <resourcesystem/resourcehandle.h>
#include <resourcesystem/resourcehandletypes.h>
#include <resourcefile/resourcetype.h>

#include <mathlib/vector4d.h>
#include <tier0/platform.h>
#include <tier0/utlstring.h>
#include <tier0/utlstringtoken.h>
#include <tier0/utlsymbollarge.h>
#include <tier0/utlvector.h>
#include "materialsystem2/material_resource_data.h"

class CBufferString;
class CRenderAttributes;
class IMaterialMode;
struct Vector2D;
struct VsInputSignature_t;

enum EBatchableInModeFlags_t : int;

//-----------------------------------------------------------------------------
// A loaded Source 2 material (CMaterial2 in materialsystem2).
//
// Method names from the s&box DWARF of IMaterial2 where the CS2 body does the
// same thing; the slots CS2 has and s&box does not are named by what they do.
// Slot 39 (Windows) / 39-40 (Linux) is the virtual destructor.
//
// The fields are the leading members of the game's CMaterial2 (2016 bytes in
// CS2, ctor in CMaterialSystem2's procedural material creators); only this
// prefix is declared, the class is never instantiated by the SDK.
//-----------------------------------------------------------------------------
abstract_class IMaterial2
{
public:
	struct TextureAttributeValue_t
	{
		CUtlStringToken m_name;
		int m_nSpecificMipLevelToBind;
		// A CWeakHandle< InfoForResourceTypeCTextureBase > (HRenderTexture) in the game
		ResourceHandle_t m_hTexture;
	};

	virtual const char *GetName() const = 0;

	// Builds "<name>_c" and resolves it through the file system into outName
	virtual void GetNameWithMod( CBufferString &outName ) const = 0;
	virtual const IMaterial2 *GetCopySource() const = 0;
	virtual uint64 GetSimilarityKey() const = 0;
	virtual bool IsLoaded() const = 0;
	virtual const IMaterialMode *GetMode( const CUtlStringToken &shaderMode ) const = 0;

	// Named by what it does: the material that owns the layer of the mode, this material
	// when it has no such mode
	virtual const IMaterial2 *GetMaterialForMode( const CUtlStringToken &shaderMode ) const = 0;
	virtual const CRenderAttributes *GetAttributes() const = 0;

	// Named by what it does: true when the attributes hold a value for the token
	virtual bool HasAttribute( CUtlStringToken name ) const = 0;

	virtual bool GetBoolAttribute( bool *pValue, CUtlStringToken name ) const = 0;
	virtual bool GetIntAttribute( int *pValue, CUtlStringToken name ) const = 0;
	virtual bool GetFloatAttribute( float *pValue, CUtlStringToken name ) const = 0;
	virtual bool GetStringAttribute( CUtlString *pValue, CUtlStringToken name ) const = 0;
	virtual bool GetVector2DAttribute( Vector2D *pValue, CUtlStringToken name ) const = 0;
	virtual bool GetVectorAttribute( Vector *pValue, CUtlStringToken name ) const = 0;
	virtual bool GetVector4DAttribute( Vector4D *pValue, CUtlStringToken name ) const = 0;
	virtual bool GetTextureAttribute( CWeakHandle< InfoForResourceTypeCTextureBase > *pValue, CUtlStringToken name ) const = 0;
	virtual CWeakHandle< InfoForResourceTypeCTextureBase > GetFirstTextureAttribute() const = 0;
	virtual void GetAllTextureAttributes( CUtlVector< TextureAttributeValue_t > &attrs ) const = 0;
	virtual const VsInputSignature_t *GetVertexShaderInputSignature() const = 0;
	virtual bool IsBatchableInMode( const CUtlStringToken &shaderMode, EBatchableInModeFlags_t nFlags ) const = 0;
	virtual void GetRepresentativeTextureSize( int *pWidth, int *pHeight, int nDefWidth, int nDefHeight, bool bClamp ) const = 0;

	// CS2 has one more int than s&box; it selects the texture attribute pair that is read
	virtual void GetWorldMappingSize( int *pWidth, int *pHeight, int nMode, int nDefWidth, int nDefHeight, bool bClamp ) const = 0;

	// Returns 1 in CS2
	virtual uint32 GetShaderStateOverriddenByMaterial() const = 0;

	// Sets a texture parameter; only between BeginParamUpdate and EndParamUpdate
	virtual bool Set( const char *pszParamName, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture ) = 0;

	// Named by what they do: the last dword of, and a pointer to, the 48-byte block at +64
	// that is copied from the copy source and cleared on unload
	virtual uint32 GetRenderStateOverrideFlags() const = 0;
	virtual const void *GetRenderStateOverrides() const = 0;

	// Named by what it does: the mode of the next layer after nLayer (up to 8 layers)
	virtual const IMaterialMode *GetModeForLayer( const CUtlStringToken &shaderMode, int nLayer ) const = 0;

	// Named by what it does: the resource kept at +96 (the shader reference of a raw material)
	virtual CWeakHandleBase GetShaderExtRef() const = 0;

	// Named by what they do: a nesting counter; the last EndParamUpdate rebuilds the
	// constant buffers of the modes when a parameter changed
	virtual void BeginParamUpdate() = 0;
	virtual void EndParamUpdate() = 0;

	virtual int GetInt( const char *pszName, int nDefault ) const = 0;
	virtual float GetFloat( const char *pszName, float flDefault ) const = 0;
	virtual const char *GetString( const char *pszName, const char *pszDefault ) const = 0;
	virtual CWeakHandle< InfoForResourceTypeCTextureBase > GetTexture( const char *pszName ) const = 0;
	virtual Vector4D GetVector4( const char *pszName, const Vector4D &vDefault ) const = 0;
	virtual bool IsEdited() const = 0;

	// Both ints are passed on to the rebuild of every layer of every mode
	virtual void RecreateAllStaticConstantAndCommandBuffers( int nRebuildArg1, int nRebuildArg2 ) = 0;

	// Named by what it does: true when a mode of the loaded material has its shaders ready
	virtual bool IsAnyModeReady() const = 0;

	virtual ~IMaterial2() {}

public:
	uint16 m_nReloadSerialNumber;
	uint8 m_nOutstandingIORequestCount;
	bool m_bIsLoaded : 1;
	bool m_bIsFallbackMaterial : 1;

	// Named by what it does: set by Set(), cleared by the last EndParamUpdate
	bool m_bConstantBuffersDirty : 1;

	// Named by what it does: set by every procedural material creator of CMaterialSystem2
	bool m_bIsProceduralMaterial : 1;
	bool m_bHasBeenEdited : 1;
	CUtlString m_name;
	CUtlVector< CMaterialParam > m_params;
};

class InfoForResourceTypeIMaterial2
{
public:
	using RuntimeClass_t = IMaterial2;
};

using HMaterialStrong = CStrongHandle< InfoForResourceTypeIMaterial2 >;
using HMaterialWeak = CWeakHandle< InfoForResourceTypeIMaterial2 >;

#endif // IMATERIAL2_H
