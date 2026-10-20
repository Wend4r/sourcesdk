#ifndef TIER0_QUANTIZEDFLOATENCODER_H
#define TIER0_QUANTIZEDFLOATENCODER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

template< class T, typename I, size_t N > class CUtlVectorMemory_Growable;
template< class T, typename I, class A > class CUtlVector;

// Flags of a quantized float
enum QuantizedFloatEncoderFlags_t
{
	QFE_ROUNDDOWN = ( 1 << 0 ),
	QFE_ROUNDUP = ( 1 << 1 ),
	QFE_ENCODE_ZERO_EXACTLY = ( 1 << 2 ),
	QFE_ENCODE_INTEGERS_EXACTLY = ( 1 << 3 ),
};

//-----------------------------------------------------------------------------
// The encoder of a range, shared by every field that declares the same definition
//-----------------------------------------------------------------------------
struct QuantizedFloatEncoder_t
{
	// The range, adjusted by the flags
	float m_flLowValue;
	float m_flHighValue;

	// Steps per unit, ( 2^bits - 1 ) / range, and its inverse per step, 1 / ( 2^bits - 1 )
	double m_flHighLowMul;
	double m_flDecodeMul;

	float m_flOffset;

	// 32 and m_bNoScale for a count outside [ 1, 31 ]
	uint8 m_nBitCount;

	// The flags in use after the validation and the flags as declared
	uint8 m_nFlags : 4;
	uint8 m_nDeclaredFlags : 4;

	uint8 m_bNoScale : 1;

	// Set by the setup of the definition
	uint8 m_bInitialized : 1;
	uint8 unk01e : 6;
};

//-----------------------------------------------------------------------------
// A definition of an encoder, given to aliases and their overrides
//-----------------------------------------------------------------------------
struct QuantizedFloatEncoderDef_t
{
	int m_nBits;
	int m_nFlags;
	float m_flLowValue;
	float m_flHighValue;
	bool m_bUnknown;
};

//-----------------------------------------------------------------------------
// The handle of a field to its encoder, usually a function static: the caller sets m_bRegistered
// after the first FindOrCreate
//-----------------------------------------------------------------------------
struct QuantizedFloatEncoderReg_t
{
	uint16 m_unEncoder;
	uint16 m_nFieldName : 15; // A symbol of the registry
	uint16 m_bRegistered : 1;
};

// Finds the encoder of the definition or creates it, and points pReg of the field at it
PLATFORM_INTERFACE void FindOrCreateQuantizedFloatEncoder( QuantizedFloatEncoderReg_t *pReg, bool bUnknown, const char *pszFieldName, int nBits, int nFlags, float flLowValue, float flHighValue );

// Same, with the definition of the alias; the first field to declare an alias defines it and the
// later ones that declare another definition are warned about
PLATFORM_INTERFACE void FindOrCreateQuantizedFloatEncoderAlias( QuantizedFloatEncoderReg_t *pReg, const char *pszFieldName, const char *pszAliasName, const QuantizedFloatEncoderDef_t *pDef );

// The definition of an alias for its new fields: an override wins over a tuning, the reset drops
// the overrides; an unusable definition is ignored with a warning
PLATFORM_INTERFACE void OverrideQuantizedFloatEncoderAlias( const char *pszAliasName, const QuantizedFloatEncoderDef_t *pDef );
PLATFORM_INTERFACE void RegisterQuantizedFloatEncoderAliasTuning( const char *pszAliasName, const QuantizedFloatEncoderDef_t *pDef );
PLATFORM_INTERFACE void ResetQuantizedFloatEncoderAliasOverrides();

PLATFORM_INTERFACE int GetQuantizedFloatEncoderAliasCount();

// False for an index out of range; ppszName receives "" for an alias without a name
PLATFORM_INTERFACE bool GetQuantizedFloatEncoderAlias( int nIndex, const char **ppszName, QuantizedFloatEncoderDef_t *pDef );

PLATFORM_INTERFACE const QuantizedFloatEncoder_t *GetQuantizedFloatEncoderByReg( const QuantizedFloatEncoderReg_t *pReg );
PLATFORM_INTERFACE const char *GetQuantizedFloatEncoderNameByReg( const QuantizedFloatEncoderReg_t *pReg );

// Appends the names of the fields that use the encoder of pReg
PLATFORM_INTERFACE void GetQuantizedFloatEncodersSharingReg( const QuantizedFloatEncoderReg_t *pReg, CUtlVector< const char *, int, CUtlVectorMemory_Growable< const char *, int, 0 > > *pFieldNames );

// The registration of a field as the callers do it, once per handle; returns the encoder.
// The first call for a handle and the registry itself are not synchronized
inline const QuantizedFloatEncoder_t *RegisterQuantizedFloatEncoder( QuantizedFloatEncoderReg_t *pReg, const char *pszFieldName, int nBits, int nFlags, float flLowValue, float flHighValue, bool bUnknown = true )
{
	if ( !pReg->m_bRegistered )
	{
		FindOrCreateQuantizedFloatEncoder( pReg, bUnknown, pszFieldName, nBits, nFlags, flLowValue, flHighValue );
		pReg->m_bRegistered = 1;
	}

	return GetQuantizedFloatEncoderByReg( pReg );
}

inline const QuantizedFloatEncoder_t *RegisterQuantizedFloatEncoderAlias( QuantizedFloatEncoderReg_t *pReg, const char *pszFieldName, const char *pszAliasName, const QuantizedFloatEncoderDef_t *pDef )
{
	if ( !pReg->m_bRegistered )
	{
		FindOrCreateQuantizedFloatEncoderAlias( pReg, pszFieldName, pszAliasName, pDef );
		pReg->m_bRegistered = 1;
	}

	return GetQuantizedFloatEncoderByReg( pReg );
}

// A registered handle, for the initialization of a function static
inline QuantizedFloatEncoderReg_t CreateQuantizedFloatEncoderReg( const char *pszFieldName, int nBits, int nFlags, float flLowValue, float flHighValue, bool bUnknown = true )
{
	QuantizedFloatEncoderReg_t reg = {};
	RegisterQuantizedFloatEncoder( &reg, pszFieldName, nBits, nFlags, flLowValue, flHighValue, bUnknown );

	return reg;
}

// The encoder of a field through a handle of its own at the place of the macro; the
// initialization of the static registers it once even when threads reach the place together
#define QUANTIZED_FLOAT_ENCODER( pszFieldName, nBits, nFlags, flLowValue, flHighValue ) \
	( [&]() -> const QuantizedFloatEncoder_t * { static const QuantizedFloatEncoderReg_t s_QuantizedFloatEncoderReg = CreateQuantizedFloatEncoderReg( pszFieldName, nBits, nFlags, flLowValue, flHighValue ); return GetQuantizedFloatEncoderByReg( &s_QuantizedFloatEncoderReg ); }() )

// The index of the alias and its definition, -1 when there is no such alias
inline int FindQuantizedFloatEncoderAlias( const char *pszAliasName, QuantizedFloatEncoderDef_t *pDef )
{
	const int nCount = GetQuantizedFloatEncoderAliasCount();

	for ( int i = 0; i < nCount; i++ )
	{
		const char *pszName = NULL;

		if ( GetQuantizedFloatEncoderAlias( i, &pszName, pDef ) && strcmp( pszName, pszAliasName ) == 0 )
			return i;
	}

	return -1;
}

#endif // TIER0_QUANTIZEDFLOATENCODER_H
