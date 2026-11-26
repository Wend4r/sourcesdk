//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The decoders of compressed animation channels.
//
//=============================================================================//

#ifndef COMPRESSEDANIM_H
#define COMPRESSEDANIM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"

enum fieldtype_t : uint8;
struct DecodeContainer_t;
class CAnimDecodeSwizzle;

//-----------------------------------------------------------------------------
// The decoder of one kind of compressed animation channel (full, static, delta, quantized, ...). No virtual
// destructor.
//-----------------------------------------------------------------------------
abstract_class ICompressorService
{
public:
	virtual const char *GetName() const = 0;
	virtual fieldtype_t GetFieldType() const = 0;
	virtual int GetFlags() const = 0;
	virtual bool DecodeFrame( const DecodeContainer_t *pContainer, int nFrame, void *pOut ) const = 0;
	virtual int DecodeSize( const DecodeContainer_t *pContainer ) const = 0;
	virtual void DeswizzleAndBlendContainer( const DecodeContainer_t *pContainer, const CAnimDecodeSwizzle *pSwizzle, void *pOut, float flWeight, const void *pFrom, const void *pTo ) const = 0;
	virtual void DeswizzleContainer( const DecodeContainer_t *pContainer, const CAnimDecodeSwizzle *pSwizzle, void *pOut, const void *pIn ) const = 0;
	virtual ICompressorService *Instantiate() const = 0;
};

//-----------------------------------------------------------------------------
// The typed base of the decoders: the CCompressedList / CCompressedStatic templates and the concrete decoders
// (CCompressedFullFloat, CCompressedStaticVector3, ...) override, none adds a virtual method or a member. The
// members hold the channel data while a container is built (CreateContainer sizes it from m_elements and the
// first value count of m_data).
//-----------------------------------------------------------------------------
template < class T >
abstract_class CCompressedAnim : public ICompressorService
{
public:
	virtual ~CCompressedAnim() {}
	virtual bool CanEncode( CUtlVector< T > &values, T &outValue ) = 0;
	virtual void *CreateContainer() = 0;
	virtual int GetSizeof() const = 0;

public:
	CUtlVector< int > m_elements;
	CUtlVector< CUtlVector< T > > m_data;
	CUtlVector< T > m_reference;
};

#endif // COMPRESSEDANIM_H
