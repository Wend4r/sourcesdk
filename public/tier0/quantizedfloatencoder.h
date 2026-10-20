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
	uint8 unk01e : 7;
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

#endif // TIER0_QUANTIZEDFLOATENCODER_H
