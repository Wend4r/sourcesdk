//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Random number generator
//
// $Workfile: $
// $NoKeywords: $
//===========================================================================//

#ifndef VSTDLIB_RANDOM_H
#define VSTDLIB_RANDOM_H

#include "platform.h"
#include "tier0/basetypes.h"
#include "tier0/threadtools.h"

#define NTAB 32

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning( disable:4251 )
#endif

//-----------------------------------------------------------------------------
// A generator of uniformly distributed random numbers
//-----------------------------------------------------------------------------
class IUniformRandomStream
{
public:
	// Sets the seed of the random number generator
	virtual void	SetSeed( int iSeed ) = 0;

	// Generates random numbers
	virtual float	RandomFloat( float flMinVal = 0.0f, float flMaxVal = 1.0f ) = 0;
	virtual int		RandomInt( int iMinVal, int iMaxVal ) = 0;
	virtual float	RandomFloatExp( float flMinVal = 0.0f, float flMaxVal = 1.0f, float flExponent = 1.0f ) = 0;
};

//-----------------------------------------------------------------------------
// The uniform generator tier0 actually exports, templated on the locking policy:
// only the CThreadNullMutex, CAtomicMutex and CThreadNullVerificationMutex instantiations exist.
//-----------------------------------------------------------------------------
template < class TMutex >
class DLL_CLASS_IMPORT CUniformRandomStreamImpl
{
public:
	CUniformRandomStreamImpl();
	explicit CUniformRandomStreamImpl( int iSeed );

	void	SetSeed( int iSeed );

	float	RandomFloat( float flMinVal = 0.0f, float flMaxVal = 1.0f );
	int		RandomInt( int iMinVal, int iMaxVal );
	float	RandomFloatExp( float flMinVal = 0.0f, float flMaxVal = 1.0f, float flExponent = 1.0f );
	float	RandomGaussianFloat( float flMean = 0.0f, float flStdDev = 1.0f );
	void	RandomizeBits( void *pOut, size_t nBytes );

private:
	int		GenerateRandomNumber_Locked();
	void	SetSeed_Locked( int iSeed );

	int m_idum;
	int m_iy;
	int m_iv[NTAB];

	TMutex m_mutex;
};

//-----------------------------------------------------------------------------
// The standard generator of uniformly distributed random numbers
//-----------------------------------------------------------------------------
using CUniformRandomStream = CUniformRandomStreamImpl< CThreadNullMutex >;

//-----------------------------------------------------------------------------
// A couple of convenience functions to access the library's global uniform stream
//-----------------------------------------------------------------------------
DLL_IMPORT void		RandomSeed( int iSeed );
DLL_IMPORT float	RandomFloat( float flMinVal = 0.0f, float flMaxVal = 1.0f );
DLL_IMPORT float	RandomFloatExp( float flMinVal = 0.0f, float flMaxVal = 1.0f, float flExponent = 1.0f );
DLL_IMPORT int		RandomInt( int iMinVal, int iMaxVal );
DLL_IMPORT float	RandomGaussianFloat( float flMean = 0.0f, float flStdDev = 1.0f );

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // VSTDLIB_RANDOM_H
