//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef COMPRESSED_VECTOR_H
#define COMPRESSED_VECTOR_H

#ifdef _WIN32
#pragma once
#endif

#include <math.h>
#include <float.h>

// For vec_t, put this somewhere else?
#include "basetypes.h"

// For rand(). We really need a library!
#include <stdlib.h>

#include "tier0/dbg.h"
#include "mathlib/vector.h"

#include "mathlib/mathlib.h"
#include "mathlib/ssemath.h"


#ifdef OSX
#pragma GCC diagnostic ignored "-Wtautological-compare"
#endif

class Quaternion48;


FORCEINLINE fltx4 UnpackQuaternion48SIMD( const Quaternion48 * RESTRICT pVec );


//=========================================================
// fit a 3D vector into 32 bits
//=========================================================

class Vector32
{
public:
	// Construction/destruction:
	Vector32(void); 
	Vector32(vec_t X, vec_t Y, vec_t Z);

	// assignment
	Vector32& operator=(const Vector &vOther);
	operator Vector ();

private:
	unsigned short x:10;
	unsigned short y:10;
	unsigned short z:10;
	unsigned short exp:2;
};

inline Vector32& Vector32::operator=(const Vector &vOther)	
{
	CHECK_VALID(vOther);

	static float expScale[4] = { 4.0f, 16.0f, 32.f, 64.f };

	float fmax = MAX( fabs( vOther.x ), fabs( vOther.y ) );
	fmax = fpmax( fmax, fabs( vOther.z ) );

	for (exp = 0; exp < 3; exp++)
	{
		if (fmax < expScale[exp])
			break;
	}
	Assert( fmax < expScale[exp] );

	float fexp = 512.0f / expScale[exp];

	x = clamp( (int)(vOther.x * fexp) + 512, 0, 1023 );
	y = clamp( (int)(vOther.y * fexp) + 512, 0, 1023 );
	z = clamp( (int)(vOther.z * fexp) + 512, 0, 1023 );
	return *this; 
}


inline Vector32::operator Vector ()
{
	Vector tmp;

	static float expScale[4] = { 4.0f, 16.0f, 32.f, 64.f };

	float fexp = expScale[exp] / 512.0f;

	tmp.x = (((int)x) - 512) * fexp;
	tmp.y = (((int)y) - 512) * fexp;
	tmp.z = (((int)z) - 512) * fexp; 
	return tmp; 
}


//=========================================================
// Fit a unit vector into 32 bits
//=========================================================

class Normal32
{
public:
	// Construction/destruction:
	Normal32(void); 
	Normal32(vec_t X, vec_t Y, vec_t Z);

	// assignment
	Normal32& operator=(const Vector &vOther);
	operator Vector ();

private:
	unsigned short x:15;
	unsigned short y:15;
	unsigned short zneg:1;
};


inline Normal32& Normal32::operator=(const Vector &vOther)	
{
	CHECK_VALID(vOther);

	x = clamp( (int)(vOther.x * 16384) + 16384, 0, 32767 );
	y = clamp( (int)(vOther.y * 16384) + 16384, 0, 32767 );
	zneg = (vOther.z < 0);
	//x = vOther.x; 
	//y = vOther.y; 
	//z = vOther.z; 
	return *this; 
}


inline Normal32::operator Vector ()
{
	Vector tmp;

	tmp.x = ((int)x - 16384) * (1 / 16384.0);
	tmp.y = ((int)y - 16384) * (1 / 16384.0);
	tmp.z = sqrt( 1 - tmp.x * tmp.x - tmp.y * tmp.y );
	if (zneg)
		tmp.z = -tmp.z;
	return tmp; 
}


//=========================================================
// 64 bit Quaternion
//=========================================================

class Quaternion64
{
public:
	// Construction/destruction:
	Quaternion64(void); 
	Quaternion64(vec_t X, vec_t Y, vec_t Z);

	// assignment
	// Quaternion& operator=(const Quaternion64 &vOther);
	Quaternion64& operator=(const Quaternion &vOther);
	operator Quaternion () const;
	inline fltx4 LoadUnalignedSIMD() const; // load onto a SIMD register without assumptions of being on a 16byte boundary  

private:
	Quaternion64( uint64 xx, uint64 yy, uint64 zz, uint64 ww ) : x(xx), y(yy), z(zz), wneg(ww) {}; // stricly for static construction
	uint64 x:21;
	uint64 y:21;
	uint64 z:21;
	uint64 wneg:1;
};


inline Quaternion64::operator Quaternion ()	 const
{
	Quaternion tmp;

	// shift to -1048576, + 1048575, then round down slightly to -1.0 < x < 1.0
 	tmp.x = ((int)x - 1048576) * (1 / 1048576.5f);
 	tmp.y = ((int)y - 1048576) * (1 / 1048576.5f);
 	tmp.z = ((int)z - 1048576) * (1 / 1048576.5f);

	tmp.w = sqrt( 1 - tmp.x * tmp.x - tmp.y * tmp.y - tmp.z * tmp.z );
	if (wneg)
		tmp.w = -tmp.w;
	return tmp; 
}

inline Quaternion64& Quaternion64::operator=(const Quaternion &vOther)	
{
	CHECK_VALID(vOther);

	x = clamp( (int)(vOther.x * 1048576) + 1048576, 0, 2097151 );
	y = clamp( (int)(vOther.y * 1048576) + 1048576, 0, 2097151 );
	z = clamp( (int)(vOther.z * 1048576) + 1048576, 0, 2097151 );
	wneg = (vOther.w < 0);
	return *this; 
}

inline fltx4 Quaternion64::LoadUnalignedSIMD() const 
{
#if 0 // basic C implementation (which ends up being slower than writing the whole Q onto the stack and then reading it back at once)
	struct { float x; float y; float z; float w; } tmp;

	tmp.x = ((int)x - 1048576) * (1 / 1048576.5f);
	tmp.y = ((int)y - 1048576) * (1 / 1048576.5f);
	tmp.z = ((int)z - 1048576) * (1 / 1048576.5f);
	tmp.w = sqrt( 1 - tmp.x * tmp.x - tmp.y * tmp.y - tmp.z * tmp.z );
	if (wneg)
		tmp.w = -tmp.w;
	
	fltx4 ret = { tmp.x, tmp.y, tmp.z, tmp.w };
	return ret;
#else // naive implementation (which ends up being faster than the explicit c imp above)
	const QuaternionAligned q(Quaternion(*this)) ;
	return LoadAlignedSIMD( &q );
#endif
}

//=========================================================
// 48 bit Quaternion
//=========================================================

class Quaternion48
{
public:
	// Construction/destruction:
	Quaternion48(void); 
	Quaternion48(vec_t X, vec_t Y, vec_t Z);

	// assignment
	// Quaternion& operator=(const Quaternion48 &vOther);
	Quaternion48& operator=(const Quaternion &vOther);
	operator Quaternion () const;
	inline fltx4 LoadUnalignedSIMD() const; // load onto a SIMD register without assumptions of being on a 16byte boundary  

//private:
	unsigned short x:16;
	unsigned short y:16;
	unsigned short z:15;
	unsigned short wneg:1;
};


inline Quaternion48::operator Quaternion ()	const
{

	Quaternion tmp;

	tmp.x = ((int)x - 32768) * (1 / 32768.5);
	tmp.y = ((int)y - 32768) * (1 / 32768.5);
	tmp.z = ((int)z - 16384) * (1 / 16384.5);
	tmp.w = sqrt( 1 - tmp.x * tmp.x - tmp.y * tmp.y - tmp.z * tmp.z );
	if (wneg)
		tmp.w = -tmp.w;
	return tmp; 

}


inline Quaternion48& Quaternion48::operator=(const Quaternion &vOther)	
{
	CHECK_VALID(vOther);

	x = clamp( (int)(vOther.x * 32768) + 32768, 0, 65535 );
	y = clamp( (int)(vOther.y * 32768) + 32768, 0, 65535 );
	z = clamp( (int)(vOther.z * 16384) + 16384, 0, 32767 );
	wneg = (vOther.w < 0);
	return *this; 
}

inline fltx4 Quaternion48::LoadUnalignedSIMD() const 
{
#if 0 // basic C implementation (which ends up being slower than writing the whole Q onto the stack and then reading it back at once)
	struct { float x; float y; float z; float w; } tmp;

	tmp.x = ((int)x - 32768) * (1 / 32768.5);
	tmp.y = ((int)y - 32768) * (1 / 32768.5);
	tmp.z = ((int)z - 16384) * (1 / 16384.5);
	tmp.w = sqrt( 1 - tmp.x * tmp.x - tmp.y * tmp.y - tmp.z * tmp.z );
	if (wneg)
		tmp.w = -tmp.w;
	
	fltx4 ret = { tmp.x, tmp.y, tmp.z, tmp.w };
	return ret;
#else // naive implementation (which ends up being faster than the explicit c imp above)
	const QuaternionAligned q(Quaternion(*this)) ;
	return LoadAlignedSIMD( &q );
#endif
}


//=========================================================
// 48 bit sorted Quaternion
//=========================================================


class Quaternion48S
{
public:
	// Construction/destruction:
	Quaternion48S(void); 
	Quaternion48S(vec_t X, vec_t Y, vec_t Z);

	// assignment
	// Quaternion& operator=(const Quaternion48 &vOther);
	Quaternion48S& operator=(const Quaternion &vOther);
	operator Quaternion () const;
	operator fltx4 () const RESTRICT ;
//private:
	// shift the quaternion so that the largest value is recreated by the sqrt()
	// abcd maps modulo into quaternion xyzw starting at "offset"
	// "offset" is split into two 1 bit fields so that the data packs into 6 bytes (3 shorts)
	unsigned short a:15;		// first of the 3 consecutive smallest quaternion elements 
	unsigned short offsetH:1;	// high bit of "offset"
	unsigned short b:15;
	unsigned short offsetL:1;	// low bit of "offset"
	unsigned short c:15;
	unsigned short dneg:1;		// sign of the largest quaternion element
};

#define SCALE48S 23168.0f		// needs to fit 2*sqrt(0.5) into 15 bits.
#define SHIFT48S 16384			// half of 2^15 bits.

inline Quaternion48S::operator Quaternion ()	const
{

	Quaternion tmp;

	COMPILE_TIME_ASSERT( sizeof( Quaternion48S ) == 6 );

	float *ptmp = &tmp.x;
	int ia = offsetL + offsetH * 2;
	int ib = ( ia + 1 ) % 4;
	int ic = ( ia + 2 ) % 4;
	int id = ( ia + 3 ) % 4;
	ptmp[ia] = ( (int)a - SHIFT48S ) * ( 1.0f / SCALE48S );
	ptmp[ib] = ( (int)b - SHIFT48S ) * ( 1.0f / SCALE48S );
	ptmp[ic] = ( (int)c - SHIFT48S ) * ( 1.0f / SCALE48S );
	ptmp[id] = sqrt( 1.0f - ptmp[ia] * ptmp[ia] - ptmp[ib] * ptmp[ib] - ptmp[ic] * ptmp[ic] );
	if (dneg)
		ptmp[id] = -ptmp[id];

	return tmp; 

}

inline Quaternion48S& Quaternion48S::operator=(const Quaternion &vOther)	
{
	CHECK_VALID(vOther);

	const float *ptmp = &vOther.x;

	// find largest field, make sure that one is recreated by the sqrt to minimize error
	int i = 0;
	if ( fabs( ptmp[i] ) < fabs( ptmp[1] ) )
	{
		i = 1;
	}
	if ( fabs( ptmp[i] ) < fabs( ptmp[2] ) )
	{
		i = 2;
	}
	if ( fabs( ptmp[i] ) < fabs( ptmp[3] ) )
	{
		i = 3;
	}

	int offset = ( i + 1 ) % 4; // make "a" so that "d" is the largest element
	offsetL = offset & 1;
	offsetH = offset > 1;
	a = clamp( (int)(ptmp[ offset ] * SCALE48S) + SHIFT48S, 0, (int)(SCALE48S * 2) );
	b = clamp( (int)(ptmp[ ( offset + 1 ) % 4 ] * SCALE48S) + SHIFT48S, 0, (int)(SCALE48S * 2) );
	c = clamp( (int)(ptmp[ ( offset + 2 ) % 4 ] * SCALE48S) + SHIFT48S, 0, (int)(SCALE48S * 2) );
	dneg = ( ptmp[ ( offset + 3 ) % 4 ] < 0.0f );

	return *this; 
}


// decode onto a SIMD register
inline Quaternion48S::operator fltx4 ()	const RESTRICT
{
	AssertMsg1( (((uintp) this) & 1) == 0, "Quaternion48S is unaligned at %p\n", this );
	AssertMsg( false, "Quaternion48S::operator fltx4  is slow on this platform and should not be used.\n" );
	QuaternionAligned q( (Quaternion) *this );
	return LoadAlignedSIMD( &q );
}


//=========================================================
// 32 bit Quaternion
//=========================================================

class Quaternion32
{
public:
	// Construction/destruction:
	Quaternion32(void); 
	Quaternion32(vec_t X, vec_t Y, vec_t Z);

	// assignment
	// Quaternion& operator=(const Quaternion48 &vOther);
	Quaternion32& operator=(const Quaternion &vOther);
	operator Quaternion ();
	inline fltx4 LoadUnalignedSIMD() const; // load onto a SIMD register without assumptions of being on a 16byte boundary  

private:
	unsigned int x:11;
	unsigned int y:10;
	unsigned int z:10;
	unsigned int wneg:1;
};


inline Quaternion32::operator Quaternion ()	
{

	Quaternion tmp;

	tmp.x = ((int)x - 1024) * (1 / 1024.0);
	tmp.y = ((int)y - 512) * (1 / 512.0);
	tmp.z = ((int)z - 512) * (1 / 512.0);
	tmp.w = sqrt( 1 - tmp.x * tmp.x - tmp.y * tmp.y - tmp.z * tmp.z );
	if (wneg)
		tmp.w = -tmp.w;
	return tmp; 

}

inline Quaternion32& Quaternion32::operator=(const Quaternion &vOther)	
{
	CHECK_VALID(vOther);

	x = clamp( (int)(vOther.x * 1024) + 1024, 0, 2047 );
	y = clamp( (int)(vOther.y * 512) + 512, 0, 1023 );
	z = clamp( (int)(vOther.z * 512) + 512, 0, 1023 );
	wneg = (vOther.w < 0);
	return *this; 
}



inline fltx4 Quaternion32::LoadUnalignedSIMD() const 
{

	struct { float x; float y; float z; float w; } tmp;

	tmp.x = ((int)x - 1024) * (1 / 1024.0);
	tmp.y = ((int)y - 512) * (1 / 512.0);
	tmp.z = ((int)z - 512) * (1 / 512.0);
	tmp.w = sqrt( 1 - tmp.x * tmp.x - tmp.y * tmp.y - tmp.z * tmp.z );
	if (wneg)
		tmp.w = -tmp.w;

	fltx4 ret = { tmp.x, tmp.y, tmp.z, tmp.w };
	return ret;

}


//=========================================================
// 16 bit float
//=========================================================


const int float32bias = 127;
const int float16bias = 15;

const float maxfloat16bits = 65504.0f;

class float16
{
public:
	// float16() {};
	//float16( float f ) { m_storage.rawWord = ConvertFloatTo16bits(f); }
	float16& operator=(const unsigned short &other)  { m_storage.rawWord = other; return *this; };

	void Init() { m_storage.rawWord = 0; }
//	float16& operator=(const float16 &other) { m_storage.rawWord = other.m_storage.rawWord; return *this; }
	//	float16& operator=(const float &other) { m_storage.rawWord = ConvertFloatTo16bits(other); return *this; }
//	operator unsigned short () { return m_storage.rawWord; }
//	operator float () { return Convert16bitFloatTo32bits( m_storage.rawWord ); }
	unsigned short GetBits() const 
	{ 
		return m_storage.rawWord; 
	}
	float GetFloat() const 
	{ 
		return Convert16bitFloatTo32bits( m_storage.rawWord ); 
	}
	void SetFloat( float in ) 
	{ 
		m_storage.rawWord = ConvertFloatTo16bits( in ); 
	}

	bool IsInfinity() const
	{
		return m_storage.bits.biased_exponent == 31 && m_storage.bits.mantissa == 0;
	}
	bool IsNaN() const
	{
		return m_storage.bits.biased_exponent == 31 && m_storage.bits.mantissa != 0;
	}

	bool operator==(const float16 other) const { return m_storage.rawWord == other.m_storage.rawWord; }
	bool operator!=(const float16 other) const { return m_storage.rawWord != other.m_storage.rawWord; }
	
//	bool operator< (const float other) const	   { return GetFloat() < other; }
//	bool operator> (const float other) const	   { return GetFloat() > other; }

	template< bool BRANCHLESS > // allows you to force branchy/branchless implementation regardless of the current platform
	static unsigned short ConvertFloatTo16bitsNonDefault( float input );
	static float Convert16bitFloatTo32bits( unsigned short input );
	
	// a special case useful for the pixel writer: take four input float values, which are already in memory (not on registers),
	// convert them all at once and write them sequentially through the output pointer.
	static void ConvertFourFloatsTo16BitsAtOnce( float16 * RESTRICT pOut,
		const float *a, const float *b, const float *c, const float *d  );
	
	// unfortunately, function templates can't have default template parameters in 2010-era C++ 
	inline static unsigned short ConvertFloatTo16bits( float input )
	{	// default to branchless on ppc and branchy on x86
		return ConvertFloatTo16bitsNonDefault<false>(input);
	}	

protected:
	union float32bits
	{
		float rawFloat;
		uint32 rawAsInt;
		struct 
		{
			unsigned int mantissa : 23;
			unsigned int biased_exponent : 8;
			unsigned int sign : 1;
		} bits;
	};

	union float16bits
	{
		unsigned short rawWord;
		struct
		{
			unsigned short mantissa : 10;
			unsigned short biased_exponent : 5;
			unsigned short sign : 1;
		} bits;
	};

	static bool IsNaN( float16bits in )
	{
		return in.bits.biased_exponent == 31 && in.bits.mantissa != 0;
	}
	static bool IsInfinity( float16bits in )
	{
		return in.bits.biased_exponent == 31 && in.bits.mantissa == 0;
	}

	// 0x0001 - 0x03ff
	float16bits m_storage;
};

class float16_with_assign : public float16
{
public:
	float16_with_assign() {}
	float16_with_assign( float f ) { m_storage.rawWord = ConvertFloatTo16bits(f); }

	float16& operator=(const float16 &other) { m_storage.rawWord = ((float16_with_assign &)other).m_storage.rawWord; return *this; }
	float16& operator=(const float &other) { m_storage.rawWord = ConvertFloatTo16bits(other); return *this; }
//	operator unsigned short () const { return m_storage.rawWord; }
	operator float () const { return Convert16bitFloatTo32bits( m_storage.rawWord ); }
};

//=========================================================
// Fit a 3D vector in 48 bits
//=========================================================

class Vector48
{
public:
	// Construction/destruction:
	Vector48(void) {}
	Vector48(vec_t X, vec_t Y, vec_t Z) { x.SetFloat( X ); y.SetFloat( Y ); z.SetFloat( Z ); }

	// assignment
	Vector48& operator=(const Vector &vOther);
	operator Vector ();

	const float operator[]( int i ) const { return (((float16 *)this)[i]).GetFloat(); }

	float16 x;
	float16 y;
	float16 z;
};

// The uses of isel below are malformed because the first expression is unsigned and thus always >= 0,
// so this whole expression maps to a simple assignment. This was found through a noisy clang
// warning. I am preprocessing this out until it is needed.
#if 0
inline void float16::ConvertFourFloatsTo16BitsAtOnce( float16 * RESTRICT pOut,
											const float *a, const float *b, const float *c, const float *d  )
{
	COMPILE_TIME_ASSERT( sizeof(float) == 4 );
	// being meant for use on the PPC, this is tuned for that. 
	// it is mostly branchless, except for the large outer for loop,
	// since there's enough instructions inside that unrolling is
	// a bad idea. This fucntion is four-at-once to simplify SIMDifying in the
	// future should a convenient SIMD way to decimate emerge
	// Also, because this is only used for the special case of converting
	// float arrays into float16 GPU textures, this turns denorms into zeroes
	// and infinities into MAXFLTs, since the shader can't deal with nonfinite
	// numbers anyway.

	// alias the input floats onto a union giving their mantissa etc
	const float32bits * const inFloat[4] = {  
		reinterpret_cast<const float32bits *>(a),
		reinterpret_cast<const float32bits *>(b),
		reinterpret_cast<const float32bits *>(c),
		reinterpret_cast<const float32bits *>(d) };

	const static unsigned int maxfloat16bitsAsInt = 0x477FE000; // 65504.0f

	const static unsigned int SIGNBIT = 0x80000000;

	
	for ( int i = 0 ; i < 4 ; ++i ) // performs better not unrolled (less stack spilling)
	{	
		unsigned int onGPR = inFloat[i]->rawAsInt;

		// make a mask for each word; will be all 1's if the float is 
		// negative, all 0s if it is positive. Can do this just by
		// using arithmetic shift to smear out the sign bit.
		int isNegative = ((int) onGPR) >> 31;

		// clamp to be within -maxfloat16bits, maxfloat16bits
		// can't just use isel because IEEE754 floats are sign-magnitude, not two's comp. However,
		// positive IEEE754s can be compared as if they were ints. So, we need to do a little extra
		// work to test the negative case efficiently.
		// clamp to -maxfloat16
#error See above for explanation of why this and other uses of isel in this file are broken.
		int clampedNeg = isel( ((int)(onGPR & ~SIGNBIT)) -  maxfloat16bitsAsInt, // -in >= maxfloatbits so in <= -maxfloat
			maxfloat16bitsAsInt | SIGNBIT, // -65504.0f
			onGPR	);
		// clamp to +maxfloat16
		int clampedPos = isel( ((int)(onGPR)) -  maxfloat16bitsAsInt, // in >= maxfloatbits 
			maxfloat16bitsAsInt , // -65504.0f
			onGPR	);

		// take advantage of PPC's andc operator to effectively do a masked-move
		onGPR = ( clampedNeg & isNegative ) | ( clampedPos & ~isNegative );
	

		// fish out the input exponent and mantis fields directly (using the union induces an LHS)
		int inExponent = (onGPR & 0x7f800000) >> 23;
		unsigned int inMantissa = (onGPR & 0x007FFFFF);

		int exponent = inExponent - 127 + 15; // rebias the exponent
		unsigned int mantissa = isel( exponent, inMantissa >> 13, (unsigned) 0 ); // squash the mantissa to zero if the number is too small to represent (no denorms)

		float16bits output;
		// saturate the mantissa if rebiased exponent >= 31 (too big to store) 
		output.bits.mantissa = isel( exponent - 31, (unsigned) 0x3ff, mantissa );
		// clamp the exponent to 0..30
		output.bits.biased_exponent = isel( exponent, isel( exponent - 31, 30, exponent ), 0 );
		output.bits.sign = isNegative; //  this doesn't lhs, but instead issues the insrdi op to a word on GPR
		pOut[i].m_storage.rawWord = output.rawWord;
	}
}
#endif


template< bool BRANCHLESS >
inline unsigned short float16::ConvertFloatTo16bitsNonDefault( float input )
{ 
	float16bits output;
	float32bits inFloat;
	//if ( !BRANCHLESS ) // x86 code
	{
		if ( input > maxfloat16bits )
			input = maxfloat16bits;
		else if ( input < -maxfloat16bits )
			input = -maxfloat16bits;


		inFloat.rawFloat = input;

	}
	/*
	// The use of isel is incorrect because the first expression is unsigned and therefore always passes
	// the test.
	else // PPC code
	{
		// force the float onto the stack and then a GPR so we eat the LHS only once.
		// you can't just write to one union member and then read back another; 
		// the compiler is inconsistent about supporting that kind of type-punning. 
		// (ie, it will work in one file, but not another.)
		memcpy(&inFloat.rawFloat, &input, sizeof(inFloat.rawFloat));
		// inFloat.rawFloat = input;
		// clamp using the GPR
		{
			const unsigned int maxfloat16bitsAsInt = 0x477FE000; // 65504.0f
			// clamp to be <= maxfloat16bits
			uint32 &rawint = inFloat.rawAsInt; // <--- lhs
			if ( rawint & 0x80000000 ) // negative
			{
				// because floats are sign-magnitude, not two's comp, need to 
				// flip the int positive briefly to do the isel comparison
#error See above for explanation of why this and other uses of isel in this file are broken.
				rawint = isel( ((int)(rawint & ~0x80000000)) -  maxfloat16bitsAsInt, // -in >= maxfloatbits so in <= -maxfloat
					maxfloat16bitsAsInt | 0x80000000, // -65504.0f
					rawint	);
			}
			else // positive
			{
				rawint = isel( ((int)(rawint)) -  maxfloat16bitsAsInt, // in >= maxfloatbits 
					maxfloat16bitsAsInt , // -65504.0f
					rawint	);
			}
		}
	}
	*/
	output.bits.sign = inFloat.bits.sign;

	if ( !inFloat.bits.biased_exponent ) 
	{ 
		// zero and denorm both map to zero
		output.bits.mantissa = 0;
		output.bits.biased_exponent = 0;
	}
	else if ( inFloat.bits.biased_exponent == 0xff )
	{
		if ( !BRANCHLESS )
		{
			if ( !inFloat.bits.mantissa ) 
			{ 
				/*
				// infinity
				output.bits.mantissa = 0;
				output.bits.biased_exponent = 31;
				*/

				// infinity maps to maxfloat
				output.bits.mantissa = 0x3ff;
				output.bits.biased_exponent = 0x1e;
			}
			else if ( inFloat.bits.mantissa ) 
			{ 
				/*
				// NaN
				output.bits.mantissa = 1;
				output.bits.biased_exponent = 31;
				*/

				// NaN maps to zero
				output.bits.mantissa = 0;
				output.bits.biased_exponent = 0;
			}
		}
		else // branchless, only meant for PPC really bc needing the cntlzw op.
		{
			// else if ( inFloat.bits.biased_exponent==0xff )  // either infinity (biased_exponent is 0xff) or NaN.
			{
				int mantissamask = output.bits.mantissa ? -1 : 0;
				output.bits.mantissa		= isel( mantissamask, 0x3ff, 0 ); //infinity maps to maxfloat, NaN to zero
				output.bits.biased_exponent = isel( mantissamask, 0x1e, 0 );
				output.bits.sign = inFloat.bits.sign;
			}
		}
	}
	else 
	{ 
		// regular number
		int new_exp = inFloat.bits.biased_exponent-float32bias;
		// it's actually better to branch in these cases on PPC, 
		// because the variable bit shift is such a massive penalty 
		// that it's worth a branch penalty to avoid it.
		if (new_exp<-24) 
		{ 
			// this maps to 0
			output.bits.mantissa = 0;
			output.bits.biased_exponent = 0;
		}

		if (new_exp<-14) 
		{
			// this maps to a denorm
			output.bits.biased_exponent = 0;
			unsigned int exp_val = ( unsigned int )( -14 - new_exp );
			if( exp_val > 0 && exp_val < 11 )
			{
				output.bits.mantissa = ( 1 << ( 10 - exp_val ) ) + ( inFloat.bits.mantissa >> ( 13 + exp_val ) );
			}
		}
		else if (new_exp>15) 
		{ 
#if 0
			// map this value to infinity
			output.bits.mantissa = 0;
			output.bits.biased_exponent = 31;
#else
			// to big. . . maps to maxfloat
			output.bits.mantissa = 0x3ff;
			output.bits.biased_exponent = 0x1e;
#endif
		}
		else 
		{
			output.bits.biased_exponent = new_exp+15;
			output.bits.mantissa = (inFloat.bits.mantissa >> 13);
		}
		

	}
	return output.rawWord;
}

inline float float16::Convert16bitFloatTo32bits( unsigned short input )
{
	float32bits output;
	const float16bits &inFloat = *((float16bits *)&input);

	if( IsInfinity( inFloat ) )
	{
		return maxfloat16bits * ( ( inFloat.bits.sign == 1 ) ? -1.0f : 1.0f );
	}
	if( IsNaN( inFloat ) )
	{
		return 0.0;
	}
	if( !inFloat.bits.biased_exponent && inFloat.bits.mantissa )
	{
		// denorm
		const float half_denorm = (1.0f/16384.0f); // 2^-14
		float mantissa = ((float)(inFloat.bits.mantissa)) / 1024.0f;
		float sgn = (inFloat.bits.sign)? -1.0f :1.0f;
		output.rawFloat = sgn*mantissa*half_denorm;
	}
	else
	{
		// regular number
		unsigned mantissa = inFloat.bits.mantissa;
		unsigned biased_exponent = inFloat.bits.biased_exponent;
		unsigned sign = ((unsigned)inFloat.bits.sign) << 31;
		biased_exponent = ( (biased_exponent - float16bias + float32bias) * (biased_exponent != 0) ) << 23;
		mantissa <<= (23-10);

		output.rawFloat = BitsToFloat( mantissa | biased_exponent | sign );
	}

	return output.rawFloat;
}




inline Vector48& Vector48::operator=(const Vector &vOther)	
{
	CHECK_VALID(vOther);

	x.SetFloat( vOther.x );
	y.SetFloat( vOther.y );
	z.SetFloat( vOther.z );
	return *this; 
}


inline Vector48::operator Vector ()
{
	Vector tmp;

	tmp.x = x.GetFloat();
	tmp.y = y.GetFloat();
	tmp.z = z.GetFloat(); 

	return tmp;
}

//=========================================================
// Fit a 2D vector in 32 bits
//=========================================================

class Vector2d32
{
public:
	// Construction/destruction:
	Vector2d32(void) {}
	Vector2d32(vec_t X, vec_t Y) { x.SetFloat( X ); y.SetFloat( Y ); }

	// assignment
	Vector2d32& operator=(const Vector &vOther);
	Vector2d32& operator=(const Vector2D &vOther);

	operator Vector2D ();

	void Init( vec_t ix = 0.f, vec_t iy = 0.f);

	float16_with_assign x;
	float16_with_assign y;
};

inline Vector2d32& Vector2d32::operator=(const Vector2D &vOther)	
{
	x.SetFloat( vOther.x );
	y.SetFloat( vOther.y );
	return *this; 
}

inline Vector2d32::operator Vector2D ()
{
	Vector2D tmp;

	tmp.x = x.GetFloat();
	tmp.y = y.GetFloat();

	return tmp;
}

inline void Vector2d32::Init( vec_t ix, vec_t iy )
{
	x.SetFloat(ix);
	y.SetFloat(iy);
}





//=========================================================
//      FAST SIMD BATCH OPERATIONS
//=========================================================









#endif

