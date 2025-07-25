//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

#ifndef MATH_LIB_H
#define MATH_LIB_H

#include <math.h>
#include "tier0/basetypes.h"
#include "mathlib/vector.h"
#include "mathlib/vector2d.h"
#include "tier0/dbg.h"
#include "mathlib/math_pfns.h"
#include "mathlib/fltx4.h"

#ifndef ALIGN8_POST
#define ALIGN8_POST
#endif

#if defined(_PS3)

#if defined(__SPU__)
#include <spu_intrinsics.h>
#include <vmx2spu.h>
#include <vectormath/c/vectormath_soa.h>
#else
#include <ppu_intrinsics.h>
#include <altivec.h>
#include <vectormath/c/vectormath_soa.h>
#endif

#endif

// plane_t structure
// !!! if this is changed, it must be changed in asm code too !!!
// FIXME: does the asm code even exist anymore?
// FIXME: this should move to a different file
struct cplane_t
{
	Vector	normal;
	float	dist;
	byte	type;			// for fast side tests
	byte	signbits;		// signx + (signy<<1) + (signz<<1)
	byte	pad[2];

#ifdef VECTOR_NO_SLOW_OPERATIONS
	cplane_t() {}

private:
	// No copy constructors allowed if we're in optimal mode
	cplane_t(const cplane_t& vOther);
#endif
};

// structure offset for asm code
#define CPLANE_NORMAL_X			0
#define CPLANE_NORMAL_Y			4
#define CPLANE_NORMAL_Z			8
#define CPLANE_DIST				12
#define CPLANE_TYPE				16
#define CPLANE_SIGNBITS			17
#define CPLANE_PAD0				18
#define CPLANE_PAD1				19

// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2

// 3-5 are non-axial planes snapped to the nearest
#define	PLANE_ANYX		3
#define	PLANE_ANYY		4
#define	PLANE_ANYZ		5


//-----------------------------------------------------------------------------
// Frustum plane indices.
// WARNING: there is code that depends on these values
//-----------------------------------------------------------------------------

enum
{
	FRUSTUM_RIGHT		= 0,
	FRUSTUM_LEFT		= 1,
	FRUSTUM_TOP			= 2,
	FRUSTUM_BOTTOM		= 3,
	FRUSTUM_NEARZ		= 4,
	FRUSTUM_FARZ		= 5,
	FRUSTUM_NUMPLANES	= 6
};

extern int SignbitsForPlane( cplane_t *out );
class Frustum_t;

// Computes Y fov from an X fov and a screen aspect ratio + X from Y
float CalcFovY( float flFovX, float flScreenAspect );
float CalcFovX( float flFovY, float flScreenAspect );

// Generate a frustum based on perspective view parameters
// NOTE: FOV is specified in degrees, as the *full* view angle (not half-angle)
class VPlane;
void GeneratePerspectiveFrustum( const Vector& origin, const QAngle &angles, float flZNear, float flZFar, float flFovX, float flAspectRatio, Frustum_t &frustum );
void GeneratePerspectiveFrustum( const Vector& origin, const Vector &forward, const Vector &right, const Vector &up, float flZNear, float flZFar, float flFovX, float flFovY, VPlane *pPlanesOut );
// Cull the world-space bounding box to the specified frustum.
// bool R_CullBox( const Vector& mins, const Vector& maxs, const Frustum_t &frustum );
// bool R_CullBoxSkipNear( const Vector& mins, const Vector& maxs, const Frustum_t &frustum );
void GenerateOrthoFrustum( const Vector &origin, const Vector &forward, const Vector &right, const Vector &up, float flLeft, float flRight, float flBottom, float flTop, float flZNear, float flZFar, VPlane *pPlanesOut );

class CTransform;
class matrix3x4a_t;

struct matrix3x4_t
{
	matrix3x4_t() {}
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 )
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	/// Creates a matrix where the X axis = forward the Y axis = left, and the Z axis = up
	void InitXYZ( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin )
	{
		m_flMatVal[ 0 ][ 0 ] = xAxis.x; m_flMatVal[ 0 ][ 1 ] = yAxis.x; m_flMatVal[ 0 ][ 2 ] = zAxis.x; m_flMatVal[ 0 ][ 3 ] = vecOrigin.x;
		m_flMatVal[ 1 ][ 0 ] = xAxis.y; m_flMatVal[ 1 ][ 1 ] = yAxis.y; m_flMatVal[ 1 ][ 2 ] = zAxis.y; m_flMatVal[ 1 ][ 3 ] = vecOrigin.y;
		m_flMatVal[ 2 ][ 0 ] = xAxis.z; m_flMatVal[ 2 ][ 1 ] = yAxis.z; m_flMatVal[ 2 ][ 2 ] = zAxis.z; m_flMatVal[ 2 ][ 3 ] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	inline void Init( const Vector &xAxis, const Vector &yAxis, const Vector &zAxis, const Vector &vecOrigin );
	inline void Init( const QAngle &angs, const Vector &pos );
	inline void Init( const QAngle &angs );
	inline void Init( const RadianEuler &angs, const Vector &pos );
	inline void Init( const RadianEuler &angs );
	inline void Init( const Quaternion &quat, const Vector &pos );
	inline void Init( const Quaternion &quat );
	inline void Init( const Vector &fwd );
	inline void Init( const Vector &fwd, const Vector &pos );
	inline void Init( const Vector &axis_of_rotation, float degrees );
	inline void Init( const Vector &axis_of_rotation, float degrees, const Vector &pos );

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t( const Vector &xAxis, const Vector &yAxis, const Vector &zAxis, const Vector &vecOrigin )
	{
		Init( xAxis, yAxis, zAxis, vecOrigin );
	}

	inline void SetToIdentity();

	/// multiply the scale/rot part of the matrix by a constant. This doesn't init the matrix ,
	/// just scale in place. So if you want to construct a scaling matrix, init to identity and
	/// then call this.
	FORCEINLINE void ScaleUpper3x3Matrix( float flScale );

	inline void ConcatRotations( const matrix3x4_t &other );
	inline void ConcatTransforms( const matrix3x4_t &other );

	inline void Multiply( const matrix3x4_t &other );
	inline void Transpose();

	inline QAngle ToQAngle() const;
	inline CTransform ToTransform() const;
	inline RadianEuler ToRadianEuler() const;
	inline Quaternion ToQuaternion() const;

	inline float RowDotProduct( int row, const Vector &in ) const;
	inline float ColumnDotProduct( MatrixAxisType_t column, const Vector &in ) const;

	inline Vector GetColumn( MatrixAxisType_t column ) const;
	inline void SetColumn( const Vector &in, MatrixAxisType_t column );

	inline void SetOrigin( const Vector &pos ) { SetColumn( pos, ORIGIN ); }
	inline Vector GetOrigin() const { return GetColumn( ORIGIN ); }

	inline Vector GetForward() const { return GetColumn( FORWARD_AXIS ); }
	inline Vector GetLeft() const { return GetColumn( LEFT_AXIS ); }
	inline Vector GetRight() const { return -GetColumn( LEFT_AXIS ); }
	inline Vector GetUp() const { return GetColumn( UP_AXIS ); }
	
	inline Vector GetRow(int nRow) const { return *(Vector*)(m_flMatVal[nRow]); }
	inline void SetRow(int nRow, const Vector& vRow) { m_flMatVal[nRow][0] = vRow.x; m_flMatVal[nRow][1] = vRow.y; m_flMatVal[nRow][2] = vRow.z; }
	
	inline void GetVectors( Vector *fwd, Vector *right, Vector *up ) const;

	inline matrix3x4_t GetInverse() const;
	inline matrix3x4_t GetInverseTranspose() const;

	inline Vector Transform( const Vector &in ) const;
	inline Vector TransformByInverse( const Vector &in ) const;

	inline void TransformAABB( const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut ) const;
	inline void TransformAABBByInverse( const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut ) const;
	
	inline QAngle TransformToLocalSpace( const QAngle &angles ) const;
	inline QAngle TransformToWorldSpace( const QAngle &angles ) const;

	inline Vector Rotate( const Vector &in ) const;
	inline Vector RotateByInverse( const Vector &in ) const;

	inline void RotateAABB( const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut ) const;
	inline void RotateAABBByInverse( const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut ) const;

	inline bool IsEqualEnough( const matrix3x4_t &other, float flTolerance = 1e-5 ) const;

	inline void Invalidate( void )
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m_flMatVal[i][j] = VEC_T_NAN;
			}
		}
	}

		/// check all components for invalid floating point values
	inline bool IsValid( void ) const
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if ( !IsFinite( m_flMatVal[i][j] ) )
					return false;
			}
		}
		return true;
	}

	bool operator==( const matrix3x4_t &other ) const
	{
		return memcmp( this, &other, sizeof(matrix3x4_t) ) == 0;
	}

	bool operator!=( const matrix3x4_t &other ) const
	{
		return memcmp( this, &other, sizeof(matrix3x4_t) ) != 0;
	}

	inline bool IsEqualTo( const matrix3x4_t &other, float flTolerance = 1e-5f ) const;

	inline void GetBasisVectorsFLU( Vector *pForward, Vector *pLeft, Vector *pUp ) const;
	inline Vector TransformVector( const Vector &v0 ) const;
	inline Vector RotateVector( const Vector &v0 ) const;
	inline Vector TransformVectorByInverse( const Vector &v0 ) const;
	inline Vector RotateVectorByInverse( const Vector &v0 ) const;
	inline Vector RotateExtents( const Vector &vBoxExtents ) const; // these are extents and must remain positive/symmetric after rotation
	inline void TransformPlane( const cplane_t &inPlane, cplane_t &outPlane ) const;
	inline void TransformPlaneByInverse( const cplane_t &inPlane, cplane_t &outPlane ) const;
	inline float GetOrthogonalityError() const;
	inline float GetDeterminant( )const;
	inline float GetSylvestersCriterion()const; // for symmetrical matrices only: should be >0 iff it's a positive definite matrix

	inline void InverseTR( matrix3x4_t &out ) const;
	inline matrix3x4_t InverseTR() const;


	float *operator[]( int i )				{ Assert(( i >= 0 ) && ( i < 3 )); return m_flMatVal[i]; }
	const float *operator[]( int i ) const	{ Assert(( i >= 0 ) && ( i < 3 )); return m_flMatVal[i]; }
	float *Base()							{ return &m_flMatVal[0][0]; }
	const float *Base() const				{ return &m_flMatVal[0][0]; }

	float m_flMatVal[3][4];
};

class ALIGN16 matrix3x4a_t : public matrix3x4_t
{
public:
	/*
	matrix3x4a_t() { if (((size_t)Base()) % 16 != 0) { Error( "matrix3x4a_t missaligned" ); } }
	*/
	matrix3x4a_t( const matrix3x4_t& src ) { *this = src; };
	matrix3x4a_t& operator=( const matrix3x4_t& src ) { memcpy( Base(), src.Base(), sizeof( float ) * 3 * 4 ); return *this; };

	matrix3x4a_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 )
	{
		AssertDbg( ( ( size_t )Base() & 0xf ) == 0 );
		m_flMatVal[ 0 ][ 0 ] = m00;	m_flMatVal[ 0 ][ 1 ] = m01; m_flMatVal[ 0 ][ 2 ] = m02; m_flMatVal[ 0 ][ 3 ] = m03;
		m_flMatVal[ 1 ][ 0 ] = m10;	m_flMatVal[ 1 ][ 1 ] = m11; m_flMatVal[ 1 ][ 2 ] = m12; m_flMatVal[ 1 ][ 3 ] = m13;
		m_flMatVal[ 2 ][ 0 ] = m20;	m_flMatVal[ 2 ][ 1 ] = m21; m_flMatVal[ 2 ][ 2 ] = m22; m_flMatVal[ 2 ][ 3 ] = m23;
	}
	matrix3x4a_t(){}

	static FORCEINLINE bool TypeIsAlignedForSIMD( void ) { return true; }


	// raw data simd accessor
	FORCEINLINE fltx4 &SIMDRow( uint nIdx ) { AssertDbg( nIdx < 3 ); return *( ( fltx4 * )( &( m_flMatVal[ nIdx ] ) ) ); }
	FORCEINLINE const fltx4 &SIMDRow( uint nIdx ) const { AssertDbg( nIdx < 3 ); return *( ( const fltx4 * )( &( m_flMatVal[ nIdx ] ) ) ); }

} ALIGN16_POST;


FORCEINLINE void matrix3x4_t::ScaleUpper3x3Matrix( float flScale )
{
	for ( int i = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 3; j++ )
		{
			m_flMatVal[ i ][ j ] *= flScale;
		}
	}
}


#ifndef M_PI
	#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#ifndef M_PI_F
	#define M_PI_F		((float)(M_PI))
#endif

// NJS: Inlined to prevent floats from being autopromoted to doubles, as with the old system.
#ifndef RAD2DEG
	#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
	#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif

// Used to represent sides of things like planes.
#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2
#define SIDE_CROSS  -2      // necessary for polylib.c

// Use different side values (1, 2, 4) instead of (0, 1, 2) so we can '|' and '&' them, and quickly determine overall clipping
// without having to maintain counters and read / write memory.
enum Sides
{
	OR_SIDE_FRONT	=	1,
	OR_SIDE_BACK	=	2,
	OR_SIDE_ON		=	4,
};

#define ON_VIS_EPSILON  0.01    // necessary for vvis (flow.c) -- again look into moving later!
#define	EQUAL_EPSILON	0.001   // necessary for vbsp (faces.c) -- should look into moving it there?

extern bool s_bMathlibInitialized;

extern const matrix3x4a_t g_MatrixIdentity;
extern  const Vector vec3_origin;
extern  const QAngle vec3_angle;
extern	const Quaternion quat_identity;
extern const Vector vec3_invalid;
extern	const int nanmask;

#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)

FORCEINLINE vec_t DotProduct(const vec_t *v1, const vec_t *v2)
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}
FORCEINLINE void VectorSubtract(const vec_t *a, const vec_t *b, vec_t *c)
{
	c[0]=a[0]-b[0];
	c[1]=a[1]-b[1];
	c[2]=a[2]-b[2];
}
FORCEINLINE void VectorAdd(const vec_t *a, const vec_t *b, vec_t *c)
{
	c[0]=a[0]+b[0];
	c[1]=a[1]+b[1];
	c[2]=a[2]+b[2];
}
FORCEINLINE void VectorCopy(const vec_t *a, vec_t *b)
{
	b[0]=a[0];
	b[1]=a[1];
	b[2]=a[2];
}
FORCEINLINE void VectorClear(vec_t *a)
{
	a[0]=a[1]=a[2]=0;
}

FORCEINLINE float VectorMaximum(const vec_t *v)
{
	return MAX( v[0], MAX( v[1], v[2] ) );
}

FORCEINLINE float VectorMaximum(const Vector& v)
{
	return MAX( v.x, MAX( v.y, v.z ) );
}

FORCEINLINE void VectorScale (const float* in, vec_t scale, float* out)
{
	out[0] = in[0]*scale;
	out[1] = in[1]*scale;
	out[2] = in[2]*scale;
}


// Cannot be forceinline as they have overloads:
inline void VectorFill(vec_t *a, float b)
{
	a[0]=a[1]=a[2]=b;
}

inline void VectorNegate(vec_t *a)
{
	a[0]=-a[0];
	a[1]=-a[1];
	a[2]=-a[2];
}


//#define VectorMaximum(a)		( max( (a)[0], max( (a)[1], (a)[2] ) ) )
#define Vector2Clear(x)			{(x)[0]=(x)[1]=0;}
#define Vector2Negate(x)		{(x)[0]=-((x)[0]);(x)[1]=-((x)[1]);}
#define Vector2Copy(a,b)		{(b)[0]=(a)[0];(b)[1]=(a)[1];}
#define Vector2Subtract(a,b,c)	{(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];}
#define Vector2Add(a,b,c)		{(c)[0]=(a)[0]+(b)[0];(c)[1]=(a)[1]+(b)[1];}
#define Vector2Scale(a,b,c)		{(c)[0]=(b)*(a)[0];(c)[1]=(b)*(a)[1];}

// NJS: Some functions in VBSP still need to use these for dealing with mixing vec4's and shorts with vec_t's.
// remove when no longer needed.
#define VECTOR_COPY( A, B ) do { (B)[0] = (A)[0]; (B)[1] = (A)[1]; (B)[2]=(A)[2]; } while(0)
#define DOT_PRODUCT( A, B ) ( (A)[0]*(B)[0] + (A)[1]*(B)[1] + (A)[2]*(B)[2] )

FORCEINLINE void VectorMAInline( const float* start, float scale, const float* direction, float* dest )
{
	dest[0]=start[0]+direction[0]*scale;
	dest[1]=start[1]+direction[1]*scale;
	dest[2]=start[2]+direction[2]*scale;
}

FORCEINLINE void VectorMAInline( const Vector& start, float scale, const Vector& direction, Vector& dest )
{
	dest.x=start.x+direction.x*scale;
	dest.y=start.y+direction.y*scale;
	dest.z=start.z+direction.z*scale;
}

FORCEINLINE void VectorMA( const Vector& start, float scale, const Vector& direction, Vector& dest )
{
	VectorMAInline(start, scale, direction, dest);
}

FORCEINLINE void VectorMA( const float * start, float scale, const float *direction, float *dest )
{
	VectorMAInline(start, scale, direction, dest);
}


int VectorCompare (const float *v1, const float *v2);

inline float VectorLength(const float *v)
{
	return FastSqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + FLT_EPSILON );
}

void CrossProduct (const float *v1, const float *v2, float *cross);

inline float CrossProductX( const Vector & v1, const Vector& v2 )
{
	return v1.y * v2.z - v1.z * v2.y;
}

inline float CrossProductY( const Vector & v1, const Vector& v2 )
{
	return v1.z * v2.x - v1.x * v2.z;
}

inline float CrossProductZ( const Vector & v1, const Vector& v2 )
{
	return v1.x * v2.y - v1.y * v2.x;
}

qboolean VectorsEqual( const float *v1, const float *v2 );

inline vec_t RoundInt (vec_t in)
{
	return floor(in + 0.5f);
}

size_t Q_log2( unsigned int val );

// Math routines done in optimized assembly math package routines
void inline SinCos( float radians, float * RESTRICT sine, float * RESTRICT cosine )
{
#if defined( _X360 )
	XMScalarSinCos( sine, cosine, radians );
#elif defined( _PS3 )
#if ( __GNUC__ == 4 ) && ( __GNUC_MINOR__ == 1 ) && ( __GNUC_PATCHLEVEL__ == 1 )
	vector_float_union s;
	vector_float_union c;

	vec_float4 rad = vec_splats( radians );
	vec_float4 sin;
	vec_float4 cos;

	sincosf4( rad, &sin, &cos );

	vec_st( sin, 0, s.f );
	vec_st( cos, 0, c.f );

	*sine   = s.f[0];
	*cosine = c.f[0];
#else //__GNUC__ == 4 && __GNUC_MINOR__ == 1 && __GNUC_PATCHLEVEL__ == 1
	vector_float_union r;
	vector_float_union s;
	vector_float_union c;

	vec_float4 rad;
	vec_float4 sin;
	vec_float4 cos;

	r.f[0] = radians;
	rad = vec_ld( 0, r.f );

	sincosf4( rad, &sin, &cos );

	vec_st( sin, 0, s.f );
	vec_st( cos, 0, c.f );

	*sine   = s.f[0];
	*cosine = c.f[0];
#endif //__GNUC__ == 4 && __GNUC_MINOR__ == 1 && __GNUC_PATCHLEVEL__ == 1
#elif defined( COMPILER_MSVC32 )
	_asm
	{
		fld		DWORD PTR [radians]
		fsincos

		mov edx, DWORD PTR [cosine]
		mov eax, DWORD PTR [sine]

		fstp DWORD PTR [edx]
		fstp DWORD PTR [eax]
	}
#elif defined( GNUC )
	double __cosr, __sinr;
 	__asm __volatile__ ("fsincos" : "=t" (__cosr), "=u" (__sinr) : "0" (radians));

  	*sine = __sinr;
  	*cosine = __cosr;
#else
	*sine = sinf(radians);
	*cosine = cosf(radians);
#endif
}

#define SIN_TABLE_SIZE	256
#define FTOIBIAS		12582912.f
extern float SinCosTable[SIN_TABLE_SIZE];

inline float TableCos( float theta )
{
#if defined( LINUX )
	return cos(theta); // under the GCC compiler the float-represented-as-an-int causes an internal compiler error
#else

	union
	{
		int i;
		float f;
	} ftmp;

	// ideally, the following should compile down to: theta * constant + constant, changing any of these constants from defines sometimes fubars this.
	ftmp.f = theta * ( float )( SIN_TABLE_SIZE / ( 2.0f * M_PI ) ) + ( FTOIBIAS + ( SIN_TABLE_SIZE / 4 ) );
	return SinCosTable[ ftmp.i & ( SIN_TABLE_SIZE - 1 ) ];
#endif
}

inline float TableSin( float theta )
{
#if defined( LINUX )
	return sin(theta); // under the GCC compiler the float-represented-as-an-int causes an internal compiler error
#else
	union
	{
		int i;
		float f;
	} ftmp;

	// ideally, the following should compile down to: theta * constant + constant
	ftmp.f = theta * ( float )( SIN_TABLE_SIZE / ( 2.0f * M_PI ) ) + FTOIBIAS;
	return SinCosTable[ ftmp.i & ( SIN_TABLE_SIZE - 1 ) ];
#endif
}

template<class T>
FORCEINLINE T Square( T const &a )
{
	return a * a;
}

FORCEINLINE bool IsPowerOfTwo( uint x )
{
	return ( x & ( x - 1 ) ) == 0;
}

// return the smallest power of two >= x.
// returns 0 if x == 0 or x > 0x80000000 (ie numbers that would be negative if x was signed)
// NOTE: the old code took an int, and if you pass in an int of 0x80000000 casted to a uint,
//       you'll get 0x80000000, which is correct for uints, instead of 0, which was correct for ints
FORCEINLINE uint SmallestPowerOfTwoGreaterOrEqual( uint x )
{
	x -= 1;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

// return the largest power of two <= x. Will return 0 if passed 0
FORCEINLINE uint LargestPowerOfTwoLessThanOrEqual( uint x )
{
	if ( x >= 0x80000000 )
		return 0x80000000;

	return SmallestPowerOfTwoGreaterOrEqual( x + 1 ) >> 1;
}


// Math routines for optimizing division
void FloorDivMod (double numer, double denom, int *quotient, int *rem);
int GreatestCommonDivisor (int i1, int i2);

// Test for FPU denormal mode
bool IsDenormal( const float &val );

// MOVEMENT INFO
enum
{
	PITCH = 0,	// up / down
	YAW,		// left / right
	ROLL		// fall over
};

void MatrixVectorsFLU( const matrix3x4_t &matrix, Vector* pForward, Vector *pLeft, Vector *pUp );
void MatrixAngles( const matrix3x4_t & matrix, float *angles ); // !!!!
void MatrixVectors( const matrix3x4_t &matrix, Vector* pForward, Vector *pRight, Vector *pUp );
void VectorTransform (const float *in1, const matrix3x4_t & in2, float *out);
void VectorITransform (const float *in1, const matrix3x4_t & in2, float *out);
void VectorRotate( const float *in1, const matrix3x4_t & in2, float *out);
void VectorRotate( const Vector &in1, const QAngle &in2, Vector &out );
void VectorRotate( const Vector &in1, const Quaternion &in2, Vector &out );
void VectorIRotate( const float *in1, const matrix3x4_t & in2, float *out);

inline const Vector VectorRotate( const Vector &vIn1, const Quaternion &qIn2 )
{
	Vector out;
	VectorRotate( vIn1, qIn2, out );
	return out;
}


#ifndef VECTOR_NO_SLOW_OPERATIONS

QAngle TransformAnglesToLocalSpace( const QAngle &angles, const matrix3x4_t &parentMatrix );
QAngle TransformAnglesToWorldSpace( const QAngle &angles, const matrix3x4_t &parentMatrix );

#endif

void MatrixInitialize( matrix3x4_t &mat, const Vector &vecOrigin, const Vector &vecXAxis, const Vector &vecYAxis, const Vector &vecZAxis );
void MatrixCopy( const matrix3x4_t &in, matrix3x4_t &out );
void MatrixInvert( const matrix3x4_t &in, matrix3x4_t &out );

// Matrix equality test
bool MatricesAreEqual( const matrix3x4_t &src1, const matrix3x4_t &src2, float flTolerance = 1e-5 );

void MatrixGetColumn( const matrix3x4_t &in, int column, Vector &out );
void MatrixSetColumn( const Vector &in, int column, matrix3x4_t &out );

//void DecomposeRotation( const matrix3x4_t &mat, float *out );
void ConcatRotations (const matrix3x4_t &in1, const matrix3x4_t &in2, matrix3x4_t &out);
void ConcatTransforms (const matrix3x4_t &in1, const matrix3x4_t &in2, matrix3x4_t &out);
// faster version assumes m0, m1, out are 16-byte aligned addresses
void ConcatTransforms_Aligned( const matrix3x4a_t &m0, const matrix3x4a_t &m1, matrix3x4a_t &out );

// For identical interface w/ VMatrix
inline void MatrixMultiply ( const matrix3x4_t &in1, const matrix3x4_t &in2, matrix3x4_t &out )
{
	ConcatTransforms( in1, in2, out );
}

void QuaternionExp( const Quaternion &p, Quaternion &q );
void QuaternionLn( const Quaternion &p, Quaternion &q );
void QuaternionAverageExponential( Quaternion &q, int nCount, const Quaternion *pQuaternions, const float *pflWeights = NULL );
void QuaternionLookAt( const Vector &vecForward, const Vector &referenceUp, Quaternion &q );
void QuaternionSlerp( const Quaternion &p, const Quaternion &q, float t, Quaternion &qt );
void QuaternionSlerpNoAlign( const Quaternion &p, const Quaternion &q, float t, Quaternion &qt );
void QuaternionBlend( const Quaternion &p, const Quaternion &q, float t, Quaternion &qt );
void QuaternionBlendNoAlign( const Quaternion &p, const Quaternion &q, float t, Quaternion &qt );
void QuaternionIdentityBlend( const Quaternion &p, float t, Quaternion &qt );
float QuaternionAngleDiff( const Quaternion &p, const Quaternion &q );
void QuaternionScale( const Quaternion &p, float t, Quaternion &q );
void QuaternionAlign( const Quaternion &p, const Quaternion &q, Quaternion &qt );
float QuaternionDotProduct( const Quaternion &p, const Quaternion &q );
void QuaternionConjugate( const Quaternion &p, Quaternion &q );
void QuaternionInvert( const Quaternion &p, Quaternion &q );
float QuaternionNormalize( Quaternion &q );
void QuaternionMultiply( const Quaternion &q, const Vector &v, Vector &result );
void QuaternionAdd( const Quaternion &p, const Quaternion &q, Quaternion &qt );
void QuaternionMult( const Quaternion &p, const Quaternion &q, Quaternion &qt );
void QuaternionMatrix( const Quaternion &q, matrix3x4_t &matrix );
void QuaternionMatrix( const Quaternion &q, const Vector &pos, matrix3x4_t &matrix );
void QuaternionMatrix( const Quaternion &q, const Vector &pos, const Vector &vScale, matrix3x4_t& mat );
void QuaternionAngles( const Quaternion &q, QAngle &angles );
void AngleQuaternion( const QAngle& angles, Quaternion &qt );
void QuaternionAngles( const Quaternion &q, RadianEuler &angles );
void QuaternionVectorsFLU( Quaternion const &q, Vector *pForward, Vector *pLeft, Vector *pUp );
void QuaternionVectorsForward( const Quaternion& q, Vector *pForward );
void AngleQuaternion( RadianEuler const &angles, Quaternion &qt );
void QuaternionAxisAngle( const Quaternion &q, Vector &axis, float &angle );
void AxisAngleQuaternion( const Vector &axis, float angle, Quaternion &q );
void BasisToQuaternion( const Vector &vecForward, const Vector &vecRight, const Vector &vecUp, Quaternion &q );
void MatrixQuaternion( const matrix3x4_t &mat, Quaternion &q );


void MatrixQuaternionFast( const matrix3x4_t &mat, Quaternion &q );
void MatrixPosition( const matrix3x4_t &matrix, Vector &position );
Vector MatrixNormalize( const matrix3x4_t &in, matrix3x4_t &out );

inline void MatrixQuaternion( const matrix3x4_t &mat, Quaternion &q, Vector &o )
{
	MatrixQuaternion( mat, q );
	MatrixPosition( mat, o );
}



float MatrixQuaternionTest( uint );
float MatrixQuaternionTest2( uint );

/// qt = p + s * q
void QuaternionAccumulate( const Quaternion &p, float s, const Quaternion &q, Quaternion &qt );

/// qt = ( s * p ) * q
void QuaternionSM( float s, const Quaternion &p, const Quaternion &q, Quaternion &qt );

/// qt = p * ( s * q )
void QuaternionMA( const Quaternion &p, float s, const Quaternion &q, Quaternion &qt );

/*
//-----------------------------------------------------------------------------
// Quaternion equality with tolerance
//-----------------------------------------------------------------------------
inline bool QuaternionsAreEqualInternal( const Quaternion& src1, const Quaternion& src2, float flTolerance )
{
	if ( !FloatsAreEqual( src1.x, src2.x, flTolerance ) )
		return false;

	if ( !FloatsAreEqual( src1.y, src2.y, flTolerance ) )
		return false;

	if ( !FloatsAreEqual( src1.z, src2.z, flTolerance ) )
		return false;

	return FloatsAreEqual( src1.w, src2.w, flTolerance );
}

inline bool QuaternionsAreEqual( const Quaternion& src1, const Quaternion& src2, float flTolerance )
{
	if ( QuaternionsAreEqualInternal( src1, src2, flTolerance ) )
		return true;

	// negated quaternions are also 'equal'
	Quaternion src2neg( -src2.x, -src2.y, -src2.z, -src2.w );
	return QuaternionsAreEqualInternal( src1, src2neg, flTolerance );
}
*/
inline const Quaternion GetNormalized( const Quaternion & q )
{
	float flInv = 1.0f / sqrtf( q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w );
	return Quaternion( q.x * flInv, q.y * flInv, q.z * flInv, q.w * flInv );
}

inline const Quaternion AngleQuaternion( const QAngle& angles )
{
	Quaternion qt;
	AngleQuaternion( angles, qt );
	return qt;
}


inline const Quaternion AngleQuaternion( RadianEuler const &angles )
{
	Quaternion qt;
	AngleQuaternion( angles, qt );
	return qt;
}



inline Quaternion QuaternionFromPitchYawRoll( float flPitch, float flYaw, float flRoll )
{
	QAngle ang( flPitch, flYaw, flRoll );

	Quaternion q;
	AngleQuaternion( ang, q );
	return q;
}

inline Quaternion QuaternionAddPitch( const Quaternion &q, float flPitch )
{
	// FIXME: I know this can be made *tons* faster, but I just want to get something working quickly
	// that matches being able to add to the pitch of a QAngles so I can expose Quats to script/game code
	QAngle ang;
	QuaternionAngles( q, ang );
	ang[ PITCH ] += flPitch;

	Quaternion res;
	AngleQuaternion( ang, res );
	return res;
}

inline Quaternion QuaternionAddYaw( const Quaternion &q, float flYaw )
{
	// FIXME: I know this can be made *tons* faster, but I just want to get something working quickly
	// that matches being able to add to the yaw of a QAngles so I can expose Quats to script/game code
	QAngle ang;
	QuaternionAngles( q, ang );
	ang[ YAW ] += flYaw;

	Quaternion res;
	AngleQuaternion( ang, res );
	return res;
}

inline Quaternion QuaternionAddRoll( const Quaternion &q, float flRoll )
{
	// FIXME: I know this can be made *tons* faster, but I just want to get something working quickly
	// that matches being able to add to the roll of a QAngles so I can expose Quats to script/game code
	QAngle ang;
	QuaternionAngles( q, ang );
	ang[ ROLL ] += flRoll;

	Quaternion res;
	AngleQuaternion( ang, res );
	return res;
}

inline const Quaternion MatrixQuaternion( const matrix3x4_t &mat )
{
	Quaternion tmp;
	MatrixQuaternion( mat, tmp );
	return tmp;
}

inline const Quaternion MatrixQuaternionFast( const matrix3x4_t &mat )
{
	Quaternion tmp;
	MatrixQuaternionFast( mat, tmp );
	return tmp;
}

inline const matrix3x4_t QuaternionMatrix( const Quaternion &q )
{
	matrix3x4_t mat;
	QuaternionMatrix( q, mat );
	return mat;
}

inline const matrix3x4_t QuaternionMatrix( const Quaternion &q, const Vector &pos )
{
	matrix3x4_t mat;
	QuaternionMatrix( q, pos, mat );
	return mat;
}

//! Shortest-arc quaternion that rotates vector v1 into vector v2
const Quaternion RotateBetween( const Vector& v1, const Vector& v2 );

inline const Quaternion QuaternionConjugate( const Quaternion &p )
{
	Quaternion q;
	QuaternionConjugate( p, q );
	return q;
}

inline const Quaternion QuaternionInvert( const Quaternion &p )
{
	Quaternion q;
	QuaternionInvert( p, q );
	return q;
}





/// Actual quaternion multiplication; NOTE: QuaternionMult aligns quaternions first, so that q *
/// conjugate(q) may be -1 instead of 1!
inline const Quaternion operator * ( const Quaternion &p, const Quaternion &q )
{
	Quaternion qt;
	qt.x = p.x * q.w + p.y * q.z - p.z * q.y + p.w * q.x;
	qt.y = -p.x * q.z + p.y * q.w + p.z * q.x + p.w * q.y;
	qt.z = p.x * q.y - p.y * q.x + p.z * q.w + p.w * q.z;
	qt.w = -p.x * q.x - p.y * q.y - p.z * q.z + p.w * q.w;
	return qt;
}

inline Quaternion& operator *= ( Quaternion &p, const Quaternion &q )
{
	QuaternionMult( p, q, p );
	return p;
}

inline const matrix3x4_t ConcatTransforms( const matrix3x4_t &in1, const matrix3x4_t &in2 )
{
	matrix3x4_t out;
	ConcatTransforms( in1, in2, out );
	return out;
}

inline const matrix3x4_t operator *( const matrix3x4_t &in1, const matrix3x4_t &in2 )
{
	matrix3x4_t out;
	ConcatTransforms( in1, in2, out );
	return out;
}


inline const matrix3x4_t MatrixInvert( const matrix3x4_t &in )
{
	matrix3x4_t out;
	::MatrixInvert( in, out );
	return out;
}

inline const Vector MatrixGetColumn( const matrix3x4_t &in, MatrixAxisType_t nColumn )
{
	return in.GetColumn( nColumn );
}

// A couple methods to find the dot product of a vector with a matrix row or column...
inline float MatrixRowDotProduct( const matrix3x4_t &in1, int row, const Vector& in2 )
{
	Assert( (row >= 0) && (row < 3) );
	return DotProduct( in1[row], in2.Base() ); 
}

inline float MatrixColumnDotProduct( const matrix3x4_t &in1, int col, const Vector& in2 )
{
	Assert( (col >= 0) && (col < 4) );
	return in1[0][col] * in2[0] + in1[1][col] * in2[1] + in1[2][col] * in2[2]; 
}

int __cdecl BoxOnPlaneSide (const float *emins, const float *emaxs, const cplane_t *plane);

inline float anglemod(float a)
{
	a = (360.f/65536) * ((int)(a*(65536.f/360.0f)) & 65535);
	return a;
}

//// CLAMP
#if defined(__cplusplus) && defined(PLATFORM_PPC)

#ifdef _X360
#define __fsels __fsel
#endif

template< >
inline double clamp( double const &val, double const &minVal, double const &maxVal )
{
	float diffmin = val - minVal;
	float diffmax = maxVal - val;
	float r;
	r = __fsel(diffmin, val, minVal);
	r = __fsel(diffmax, r, maxVal);
	return r;
}

template< >
inline double clamp( double const &val, float const &minVal, float const &maxVal )
{
	// these typecasts are actually free since all FPU regs are 64 bit on PPC anyway
	return clamp ( val, (double) minVal, (double) maxVal );
}
template< >
inline double clamp( double const &val, float const &minVal, double const &maxVal )
{
	return clamp ( val, (double) minVal, (double) maxVal );
}
template< >
inline double clamp( double const &val, double const &minVal, float const &maxVal )
{
	return clamp ( val, (double) minVal, (double) maxVal );
}

template< >
inline float clamp( float const &val, float const &minVal, float const &maxVal )
{
	float diffmin = val - minVal;
	float diffmax = maxVal - val;
	float r;
	r = __fsels(diffmin, val, minVal);
	r = __fsels(diffmax, r, maxVal);
	return r;
}

template< >
inline float clamp( float const &val, double const &minVal, double const &maxVal )
{
	float diffmin = val - minVal;
	float diffmax = maxVal - val;
	float r;
	r = __fsels(diffmin, val, minVal);
	r = __fsels(diffmax, r, maxVal);
	return r;
}
template< >
inline float clamp( float const &val, double const &minVal, float const &maxVal )
{
	return clamp ( val, (float) minVal, maxVal );
}
template< >
inline float clamp( float const &val, float const &minVal, double const &maxVal )
{
	return clamp ( val, minVal, (float) maxVal );
}

#endif

// Remap a value in the range [A,B] to [C,D].
inline float RemapVal( float val, float A, float B, float C, float D)
{
	if ( A == B )
		return fsel( val - B , D , C );
	return C + (D - C) * (val - A) / (B - A);
}

inline float RemapValClamped( float val, float A, float B, float C, float D)
{
	if ( A == B )
		return fsel( val - B , D , C );
	float cVal = (val - A) / (B - A);
	cVal = clamp<float>( cVal, 0.0f, 1.0f );

	return C + (D - C) * cVal;
}

// Returns A + (B-A)*flPercent.
// float Lerp( float flPercent, float A, float B );
template <class T>
FORCEINLINE T Lerp( float flPercent, T const &A, T const &B )
{
	return A + (B - A) * flPercent;
}

FORCEINLINE float Sqr( float f )
{
	return f*f;
}

// 5-argument floating point linear interpolation.
// FLerp(f1,f2,i1,i2,x)=
//    f1 at x=i1
//    f2 at x=i2
//   smooth lerp between f1 and f2 at x>i1 and x<i2
//   extrapolation for x<i1 or x>i2
//
//   If you know a function f(x)'s value (f1) at position i1, and its value (f2) at position i2,
//   the function can be linearly interpolated with FLerp(f1,f2,i1,i2,x)
//    i2=i1 will cause a divide by zero.
static inline float FLerp(float f1, float f2, float i1, float i2, float x)
{
  return f1+(f2-f1)*(x-i1)/(i2-i1);
}


#ifndef VECTOR_NO_SLOW_OPERATIONS

// YWB:  Specialization for interpolating euler angles via quaternions...
template<> FORCEINLINE QAngle Lerp<QAngle>( float flPercent, const QAngle& q1, const QAngle& q2 )
{
	// Avoid precision errors
	if ( q1 == q2 )
		return q1;

	Quaternion src, dest;

	// Convert to quaternions
	AngleQuaternion( q1, src );
	AngleQuaternion( q2, dest );

	Quaternion result;

	// Slerp
	QuaternionSlerp( src, dest, flPercent, result );

	// Convert to euler
	QAngle output;
	QuaternionAngles( result, output );
	return output;
}

#else

#pragma error

// NOTE NOTE: I haven't tested this!! It may not work! Check out interpolatedvar.cpp in the client dll to try it
template<> FORCEINLINE QAngleByValue Lerp<QAngleByValue>( float flPercent, const QAngleByValue& q1, const QAngleByValue& q2 )
{
	// Avoid precision errors
	if ( q1 == q2 )
		return q1;

	Quaternion src, dest;

	// Convert to quaternions
	AngleQuaternion( q1, src );
	AngleQuaternion( q2, dest );

	Quaternion result;

	// Slerp
	QuaternionSlerp( src, dest, flPercent, result );

	// Convert to euler
	QAngleByValue output;
	QuaternionAngles( result, output );
	return output;
}

#endif // VECTOR_NO_SLOW_OPERATIONS


// Swap two of anything.
template <class T> 
FORCEINLINE void V_swap( T& x, T& y )
{
	T temp = x;
	x = y;
	y = temp;
}

template <class T> FORCEINLINE T AVG(T a, T b)
{
	return (a+b)/2;
}

// number of elements in an array of static size
#define NELEMS(x) ((sizeof(x))/sizeof(x[0]))

// XYZ macro, for printf type functions - ex printf("%f %f %f",XYZ(myvector));
#define XYZ(v) (v).x,(v).y,(v).z




inline float Sign( float x )
{
	return fsel( x, 1.0f, -1.0f ); // x >= 0 ? 1.0f : -1.0f
	//return (x <0.0f) ? -1.0f : 1.0f;
}

//
// Clamps the input integer to the given array bounds.
// Equivalent to the following, but without using any branches:
//
// if( n < 0 ) return 0;
// else if ( n > maxindex ) return maxindex;
// else return n;
//
// This is not always a clear performance win, but when you have situations where a clamped 
// value is thrashing against a boundary this is a big win. (ie, valid, invalid, valid, invalid, ...)
//
// Note: This code has been run against all possible integers.
//
inline int ClampArrayBounds( int n, unsigned maxindex )
{
	// mask is 0 if less than 4096, 0xFFFFFFFF if greater than
	unsigned int inrangemask = 0xFFFFFFFF + (((unsigned) n) > maxindex );
	unsigned int lessthan0mask = 0xFFFFFFFF + ( n >= 0 );
	
	// If the result was valid, set the result, (otherwise sets zero)
	int result = (inrangemask & n);

	// if the result was out of range or zero.
	result |= ((~inrangemask) & (~lessthan0mask)) & maxindex;

	return result;
}



// Turn a number "inside out". 
// See Recording Animation in Binary Order for Progressive Temporal Refinement
// by Paul Heckbert from "Graphics Gems".
//
// If you want to iterate something from 0 to n, you can use this to iterate non-sequentially, in
// such a way that you will start with widely separated values and then refine the gaps between
// them, as you would for progressive refinement. This works with non-power of two ranges.
int InsideOut( int nTotal, int nCounter );

#define BOX_ON_PLANE_SIDE(emins, emaxs, p)	\
	(((p)->type < 3)?						\
	(										\
		((p)->dist <= (emins)[(p)->type])?	\
			1								\
		:									\
		(									\
			((p)->dist >= (emaxs)[(p)->type])?\
				2							\
			:								\
				3							\
		)									\
	)										\
	:										\
		BoxOnPlaneSide( (emins), (emaxs), (p)))

//-----------------------------------------------------------------------------
// FIXME: Vector versions.... the float versions will go away hopefully soon!
//-----------------------------------------------------------------------------

void AngleVectors (const QAngle& angles, Vector *forward);
void AngleVectors (const QAngle& angles, Vector *forward, Vector *right, Vector *up);
void AngleVectorsTranspose (const QAngle& angles, Vector *forward, Vector *right, Vector *up);
void AngleVectorsFLU( const QAngle& angles, Vector *pForward, Vector *pLeft, Vector *pUp );
void AngleMatrix (const QAngle &angles, matrix3x4_t &mat );
void AngleMatrix( const QAngle &angles, const Vector &position, matrix3x4_t &mat );
void AngleMatrix (const RadianEuler &angles, matrix3x4_t &mat );
void AngleMatrix( RadianEuler const &angles, const Vector &position, matrix3x4_t &mat );
void AngleIMatrix (const QAngle &angles, matrix3x4_t &mat );
void AngleIMatrix (const QAngle &angles, const Vector &position, matrix3x4_t &mat );
void AngleIMatrix (const RadianEuler &angles, matrix3x4_t &mat );
void VectorAngles( const Vector &forward, QAngle &angles );
void VectorAngles( const Vector &forward, const Vector &pseudoup, QAngle &angles );
void VectorMatrix( const Vector &forward, matrix3x4_t &mat );
void VectorVectors( const Vector &forward, Vector &right, Vector &up );
void SetIdentityMatrix( matrix3x4_t &mat );
void SetScaleMatrix( float x, float y, float z, matrix3x4_t &dst );
void MatrixBuildRotationAboutAxis( const Vector &vAxisOfRot, float angleDegrees, matrix3x4_t &dst );

inline bool MatrixIsIdentity( const matrix3x4_t &m )
{
	return 
		m.m_flMatVal[0][0] == 1.0f && m.m_flMatVal[0][1] == 0.0f && m.m_flMatVal[0][2] == 0.0f && m.m_flMatVal[0][3] == 0.0f &&
		m.m_flMatVal[1][0] == 0.0f && m.m_flMatVal[1][1] == 1.0f && m.m_flMatVal[1][2] == 0.0f && m.m_flMatVal[1][3] == 0.0f &&
		m.m_flMatVal[2][0] == 0.0f && m.m_flMatVal[2][1] == 0.0f && m.m_flMatVal[2][2] == 1.0f && m.m_flMatVal[2][3] == 0.0f;
}


inline void SetScaleMatrix( float flScale, matrix3x4_t &dst )
{
	SetScaleMatrix( flScale, flScale, flScale, dst );
}

inline void SetScaleMatrix( const Vector& scale, matrix3x4_t &dst )
{
	SetScaleMatrix( scale.x, scale.y, scale.z, dst );
}

// Computes the inverse transpose
void MatrixTranspose( matrix3x4_t& mat );
void MatrixTranspose( const matrix3x4_t& src, matrix3x4_t& dst );
void MatrixInverseTranspose( const matrix3x4_t& src, matrix3x4_t& dst );

inline void PositionMatrix( const Vector &position, matrix3x4_t &mat )
{
	MatrixSetColumn( position, 3, mat );
}

inline void MatrixPosition( const matrix3x4_t &matrix, Vector &position )
{
	position[0] = matrix[0][3];
	position[1] = matrix[1][3];
	position[2] = matrix[2][3];
}

inline void VectorRotate( const Vector& in1, const matrix3x4_t &in2, Vector &out)
{
	VectorRotate( &in1.x, in2, &out.x );
}

inline void VectorIRotate( const Vector& in1, const matrix3x4_t &in2, Vector &out)
{
	VectorIRotate( &in1.x, in2, &out.x );
}

inline void MatrixAngles( const matrix3x4_t &matrix, QAngle &angles )
{
	MatrixAngles( matrix, &angles.x );
}

inline void MatrixAngles( const matrix3x4_t &matrix, QAngle &angles, Vector &position )
{
	MatrixAngles( matrix, angles );
	MatrixPosition( matrix, position );
}

inline void MatrixAngles( const matrix3x4_t &matrix, RadianEuler &angles )
{
	MatrixAngles( matrix, &angles.x );

	angles.Init( DEG2RAD( angles.z ), DEG2RAD( angles.x ), DEG2RAD( angles.y ) );
}

void MatrixAngles( const matrix3x4_t &mat, RadianEuler &angles, Vector &position );

void MatrixAngles( const matrix3x4_t &mat, Quaternion &q, Vector &position );

inline int VectorCompare (const Vector& v1, const Vector& v2)
{
	return v1 == v2;
}

inline void VectorTransform (const Vector& in1, const matrix3x4_t &in2, Vector &out)
{
	VectorTransform( &in1.x, in2, &out.x );
}

// MSVC folds the return value nicely and creates no temporaries on the stack,
//    we need more experiments with different compilers and in different circumstances
inline const Vector VectorTransform( const Vector& in1, const matrix3x4_t &in2 )
{
	Vector out;
	VectorTransform( in1, in2, out );
	return out;
}

inline const Vector VectorRotate( const Vector& in1, const matrix3x4_t &in2 )
{
	Vector out;
	VectorRotate( in1, in2, out );
	return out;
}



inline void VectorITransform (const Vector& in1, const matrix3x4_t &in2, Vector &out)
{
	VectorITransform( &in1.x, in2, &out.x );
}

inline const Vector VectorITransform( const Vector& in1, const matrix3x4_t &in2 )
{
	Vector out;
	VectorITransform( in1, in2, out );
	return out;
}

/*
inline void DecomposeRotation( const matrix3x4_t &mat, Vector &out )
{
	DecomposeRotation( mat, &out.x );
}
*/

inline int BoxOnPlaneSide (const Vector& emins, const Vector& emaxs, const cplane_t *plane )
{
	return BoxOnPlaneSide( &emins.x, &emaxs.x, plane );
}

inline void VectorFill(Vector& a, float b)
{
	a[0]=a[1]=a[2]=b;
}

inline void VectorNegate(Vector& a)
{
	a[0] = -a[0];
	a[1] = -a[1];
	a[2] = -a[2];
}

inline vec_t VectorAvg(Vector& a)
{
	return ( a[0] + a[1] + a[2] ) / 3;
}

//-----------------------------------------------------------------------------
// Box/plane test (slow version)
//-----------------------------------------------------------------------------
inline int FASTCALL BoxOnPlaneSide2 (const Vector& emins, const Vector& emaxs, const cplane_t *p, float tolerance = 0.f )
{
	Vector	corners[2];

	if (p->normal[0] < 0)
	{
		corners[0][0] = emins[0];
		corners[1][0] = emaxs[0];
	}
	else
	{
		corners[1][0] = emins[0];
		corners[0][0] = emaxs[0];
	}

	if (p->normal[1] < 0)
	{
		corners[0][1] = emins[1];
		corners[1][1] = emaxs[1];
	}
	else
	{
		corners[1][1] = emins[1];
		corners[0][1] = emaxs[1];
	}

	if (p->normal[2] < 0)
	{
		corners[0][2] = emins[2];
		corners[1][2] = emaxs[2];
	}
	else
	{
		corners[1][2] = emins[2];
		corners[0][2] = emaxs[2];
	}

	int sides = 0;

	float dist1 = DotProduct (p->normal, corners[0]) - p->dist;
	if (dist1 >= tolerance)
		sides = 1;

	float dist2 = DotProduct (p->normal, corners[1]) - p->dist;
	if (dist2 < -tolerance)
		sides |= 2;

	return sides;
}

//-----------------------------------------------------------------------------
// Helpers for bounding box construction
//-----------------------------------------------------------------------------

void ClearBounds (Vector& mins, Vector& maxs);
void AddPointToBounds (const Vector& v, Vector& mins, Vector& maxs);

//-----------------------------------------------------------------------------
// Ensures that the min and max bounds values are valid. 
// (ClearBounds() sets min > max, which is clearly invalid.)
//-----------------------------------------------------------------------------
bool AreBoundsValid( const Vector &vMin, const Vector &vMax );

//-----------------------------------------------------------------------------
// Returns true if the provided point is in the AABB defined by vMin
// at the lower corner and vMax at the upper corner.
//-----------------------------------------------------------------------------
bool IsPointInBounds( const Vector &vPoint, const Vector &vMin, const Vector &vMax );

//
// COLORSPACE/GAMMA CONVERSION STUFF
//
void BuildGammaTable( float gamma, float texGamma, float brightness, int overbright );

// convert texture to linear 0..1 value
inline float TexLightToLinear( int c, int exponent )
{
	// On VS 2013 LTCG builds it is required that the array declaration be annotated with
	// the same alignment requirements as the array definition.
	extern ALIGN128 float power2_n[256]; 
	Assert( exponent >= -128 && exponent <= 127 );
	return ( float )c * power2_n[exponent+128];
}


// convert texture to linear 0..1 value
int LinearToTexture( float f );
// converts 0..1 linear value to screen gamma (0..255)
int LinearToScreenGamma( float f );
float TextureToLinear( int c );

// compressed color format 
struct ColorRGBExp32
{
	byte r, g, b;
	signed char exponent;
};

void ColorRGBExp32ToVector( const ColorRGBExp32& in, Vector& out );
void VectorToColorRGBExp32( const Vector& v, ColorRGBExp32 &c );

// solve for "x" where "a x^2 + b x + c = 0", return true if solution exists
bool SolveQuadratic( float a, float b, float c, float &root1, float &root2 );

// solves for "a, b, c" where "a x^2 + b x + c = y", return true if solution exists
bool SolveInverseQuadratic( float x1, float y1, float x2, float y2, float x3, float y3, float &a, float &b, float &c );

// solves for a,b,c specified as above, except that it always creates a monotonically increasing or
// decreasing curve if the data is monotonically increasing or decreasing. In order to enforce the
// monoticity condition, it is possible that the resulting quadratic will only approximate the data
// instead of interpolating it. This code is not especially fast.
bool SolveInverseQuadraticMonotonic( float x1, float y1, float x2, float y2, 
									 float x3, float y3, float &a, float &b, float &c );




// solves for "a, b, c" where "1/(a x^2 + b x + c ) = y", return true if solution exists
bool SolveInverseReciprocalQuadratic( float x1, float y1, float x2, float y2, float x3, float y3, float &a, float &b, float &c );

// rotate a vector around the Z axis (YAW)
void VectorYawRotate( const Vector& in, float flYaw, Vector &out);


// Bias takes an X value between 0 and 1 and returns another value between 0 and 1
// The curve is biased towards 0 or 1 based on biasAmt, which is between 0 and 1.
// Lower values of biasAmt bias the curve towards 0 and higher values bias it towards 1.
//
// For example, with biasAmt = 0.2, the curve looks like this:
//
// 1
// |				  *
// |				  *
// |			     *
// |			   **
// |			 **
// |	  	 ****
// |*********
// |___________________
// 0                   1
//
//
// With biasAmt = 0.8, the curve looks like this:
//
// 1
// | 	**************
// |  **
// | * 
// | *
// |* 
// |* 
// |*  
// |___________________
// 0                   1
//
// With a biasAmt of 0.5, Bias returns X.
float Bias( float x, float biasAmt );


// Gain is similar to Bias, but biasAmt biases towards or away from 0.5.
// Lower bias values bias towards 0.5 and higher bias values bias away from it.
//
// For example, with biasAmt = 0.2, the curve looks like this:
//
// 1
// | 				  *
// | 				 *
// | 				**
// |  ***************
// | **
// | *
// |*
// |___________________
// 0                   1
//
//
// With biasAmt = 0.8, the curve looks like this:
//
// 1
// |  		    *****
// |  		 ***
// |  		*
// | 		*
// | 		*
// |   	 ***
// |*****
// |___________________
// 0                   1
float Gain( float x, float biasAmt );


// SmoothCurve maps a 0-1 value into another 0-1 value based on a cosine wave
// where the derivatives of the function at 0 and 1 (and 0.5) are 0. This is useful for
// any fadein/fadeout effect where it should start and end smoothly.
//
// The curve looks like this:
//
// 1
// |  		**
// | 	   *  *
// | 	  *	   *
// | 	  *	   *
// | 	 *		*
// |   **		 **
// |***			   ***
// |___________________
// 0                   1
//
float SmoothCurve( float x );


// This works like SmoothCurve, with two changes:
//
// 1. Instead of the curve peaking at 0.5, it will peak at flPeakPos.
//    (So if you specify flPeakPos=0.2, then the peak will slide to the left).
//
// 2. flPeakSharpness is a 0-1 value controlling the sharpness of the peak.
//    Low values blunt the peak and high values sharpen the peak.
float SmoothCurve_Tweak( float x, float flPeakPos=0.5, float flPeakSharpness=0.5 );


//float ExponentialDecay( float halflife, float dt );
//float ExponentialDecay( float decayTo, float decayTime, float dt );

// halflife is time for value to reach 50%
inline float ExponentialDecay( float halflife, float dt )
{
	// log(0.5) == -0.69314718055994530941723212145818
	return expf( -0.69314718f / halflife * dt);
}

// decayTo is factor the value should decay to in decayTime
inline float ExponentialDecay( float decayTo, float decayTime, float dt )
{
	return expf( logf( decayTo ) / decayTime * dt);
}

// Get the integrated distanced traveled
// decayTo is factor the value should decay to in decayTime
// dt is the time relative to the last velocity update
inline float ExponentialDecayIntegral( float decayTo, float decayTime, float dt  )
{
	return (powf( decayTo, dt / decayTime) * decayTime - decayTime) / logf( decayTo );
}

// hermite basis function for smooth interpolation
// Similar to Gain() above, but very cheap to call
// value should be between 0 & 1 inclusive
inline float SimpleSpline( float value )
{
	float valueSquared = value * value;

	// Nice little ease-in, ease-out spline-like curve
	return (3 * valueSquared - 2 * valueSquared * value);
}

// remaps a value in [startInterval, startInterval+rangeInterval] from linear to
// spline using SimpleSpline
inline float SimpleSplineRemapVal( float val, float A, float B, float C, float D)
{
	if ( A == B )
		return val >= B ? D : C;
	float cVal = (val - A) / (B - A);
	return C + (D - C) * SimpleSpline( cVal );
}

// remaps a value in [startInterval, startInterval+rangeInterval] from linear to
// spline using SimpleSpline
inline float SimpleSplineRemapValClamped( float val, float A, float B, float C, float D )
{
	if ( A == B )
		return val >= B ? D : C;
	float cVal = (val - A) / (B - A);
	cVal = clamp( cVal, 0.0f, 1.0f );
	return C + (D - C) * SimpleSpline( cVal );
}


FORCEINLINE int RoundFloatToInt(float f)
{
#if defined( _X360 )
#ifdef Assert
	Assert( IsFPUControlWordSet() );
#endif
	union
	{
		double flResult;
		int pResult[2];
	};
	flResult = __fctiw( f );
	return pResult[1];
#elif defined ( _PS3 )
#if defined(__SPU__)
	int nResult;
	nResult = static_cast<int>(f);
	return nResult;
#else
	return  __fctiw( f );
#endif
#else // !X360
	int nResult;
#if defined( COMPILER_MSVC32 )
	__asm
	{
		fld f
		fistp nResult
	}
#elif GNUC
	__asm __volatile__ (
		"fistpl %0;": "=m" (nResult): "t" (f) : "st"
	);
#else
	nResult = static_cast<int>(f);
#endif
	return nResult;
#endif
}

FORCEINLINE unsigned char RoundFloatToByte(float f)
{
#if defined( _X360 )
#ifdef Assert
	Assert( IsFPUControlWordSet() );
#endif
	union
	{
		double flResult;
		int pIntResult[2];
		unsigned char pResult[8];
	};
	flResult = __fctiw( f );
#ifdef Assert
	Assert( pIntResult[1] >= 0 && pIntResult[1] <= 255 );
#endif
	return pResult[7];

#elif defined ( _PS3 )
#if defined(__SPU__)
	int nResult;
	nResult = static_cast<unsigned int> (f) & 0xff;
	return nResult;
#else
	return __fctiw( f );
#endif
#else // !X360
	
	int nResult;

#if defined( COMPILER_MSVC32 )
	__asm
	{
		fld f
		fistp nResult
	}
#elif GNUC
	__asm __volatile__ (
		"fistpl %0;": "=m" (nResult): "t" (f) : "st"
	);
#else
	nResult = static_cast<unsigned int> (f) & 0xff;
#endif

#ifdef Assert
	Assert( nResult >= 0 && nResult <= 255 );
#endif 
	return nResult;

#endif
}

FORCEINLINE unsigned long RoundFloatToUnsignedLong(float f)
{
#if defined( _X360 )
#ifdef Assert
	Assert( IsFPUControlWordSet() );
#endif
	union
	{
		double flResult;
		int pIntResult[2];
		unsigned long pResult[2];
	};
	flResult = __fctiw( f );
	Assert( pIntResult[1] >= 0 );
	return pResult[1];
#elif defined ( _PS3 )
#if defined(__SPU__)
	return static_cast<unsigned long>(f);
#else
	return __fctiw( f );
#endif
#else  // !X360
	
#if defined( COMPILER_MSVC32 )
	unsigned char nResult[8];
	__asm
	{
		fld f
		fistp       qword ptr nResult
	}
	return *((unsigned long*)nResult);
#elif defined( COMPILER_GCC )
	unsigned char nResult[8];
	__asm __volatile__ (
		"fistpl %0;": "=m" (nResult): "t" (f) : "st"
	);
	return *((unsigned long*)nResult);
#else
	return static_cast<unsigned long>(f);
#endif

#endif
}

FORCEINLINE bool IsIntegralValue( float flValue, float flTolerance = 0.001f )
{
	return fabs( RoundFloatToInt( flValue ) - flValue ) < flTolerance;
}

// Fast, accurate ftol:
FORCEINLINE int Float2Int( float a )
{
#if defined( _X360 )
	union
	{
		double flResult;
		int pResult[2];
	};
	flResult = __fctiwz( a );
	return pResult[1];
#elif defined ( _PS3 )
#if defined(__SPU__)
	int RetVal;
	RetVal = static_cast<int>( a );
	return RetVal;
#else
	return __fctiwz( a );
#endif
#else  // !X360
	
	int RetVal;

#if defined( COMPILER_MSVC32 )
	int CtrlwdHolder;
	int CtrlwdSetter;
	__asm 
	{
		fld    a					// push 'a' onto the FP stack
		fnstcw CtrlwdHolder		// store FPU control word
		movzx  eax, CtrlwdHolder	// move and zero extend word into eax
		and    eax, 0xFFFFF3FF	// set all bits except rounding bits to 1
		or     eax, 0x00000C00	// set rounding mode bits to round towards zero
		mov    CtrlwdSetter, eax	// Prepare to set the rounding mode -- prepare to enter plaid!
		fldcw  CtrlwdSetter		// Entering plaid!
		fistp  RetVal				// Store and converted (to int) result
		fldcw  CtrlwdHolder		// Restore control word
	}
#else
	RetVal = static_cast<int>( a );
#endif

	return RetVal;
#endif
}



// Over 15x faster than: (int)floor(value)
inline int Floor2Int( float a )
{
   int RetVal;

#if defined( PLATFORM_PPC )
	RetVal = (int)floor( a );
#elif defined( COMPILER_MSVC32 )
   int CtrlwdHolder;
   int CtrlwdSetter;
   __asm 
   {
      fld    a					// push 'a' onto the FP stack
      fnstcw CtrlwdHolder		// store FPU control word
      movzx  eax, CtrlwdHolder	// move and zero extend word into eax
      and    eax, 0xFFFFF3FF	// set all bits except rounding bits to 1
      or     eax, 0x00000400	// set rounding mode bits to round down
      mov    CtrlwdSetter, eax	// Prepare to set the rounding mode -- prepare to enter plaid!
      fldcw  CtrlwdSetter		// Entering plaid!
      fistp  RetVal				// Store floored and converted (to int) result
      fldcw  CtrlwdHolder		// Restore control word
   }
#else
	RetVal = static_cast<int>( floor(a) );
#endif

	return RetVal;
}

//-----------------------------------------------------------------------------
// Fast color conversion from float to unsigned char
//-----------------------------------------------------------------------------
FORCEINLINE unsigned char FastFToC( float c )
{
	volatile float dc;

	// ieee trick
	dc = c * 255.0f + (float)(1 << 23);
	
	// return the lsb
#if defined( _X360 ) || defined( _PS3 )
	return ((unsigned char*)&dc)[3];
#else
	return *(unsigned char*)&dc;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Bound input float to .001 (millisecond) boundary
// Input  : in - 
// Output : inline float
//-----------------------------------------------------------------------------
inline float ClampToMsec( float in )
{
	int msec = Floor2Int( in * 1000.0f + 0.5f );
	return msec / 1000.0f;
}

// Over 15x faster than: (int)ceil(value)
inline int Ceil2Int( float a )
{
   int RetVal;

#if defined( PLATFORM_PPC )
	RetVal = (int)ceil( a );
#elif defined( COMPILER_MSVC32 )
   int CtrlwdHolder;
   int CtrlwdSetter;
   __asm 
   {
      fld    a					// push 'a' onto the FP stack
      fnstcw CtrlwdHolder		// store FPU control word
      movzx  eax, CtrlwdHolder	// move and zero extend word into eax
      and    eax, 0xFFFFF3FF	// set all bits except rounding bits to 1
      or     eax, 0x00000800	// set rounding mode bits to round down
      mov    CtrlwdSetter, eax	// Prepare to set the rounding mode -- prepare to enter plaid!
      fldcw  CtrlwdSetter		// Entering plaid!
      fistp  RetVal				// Store floored and converted (to int) result
      fldcw  CtrlwdHolder		// Restore control word
   }
#else
	RetVal = static_cast<int>( ceil(a) );
#endif

	return RetVal;
}


// Regular signed area of triangle
#define TriArea2D( A, B, C ) \
	( 0.5f * ( ( B.x - A.x ) * ( C.y - A.y ) - ( B.y - A.y ) * ( C.x - A.x ) ) )

// This version doesn't premultiply by 0.5f, so it's the area of the rectangle instead
#define TriArea2DTimesTwo( A, B, C ) \
	( ( ( B.x - A.x ) * ( C.y - A.y ) - ( B.y - A.y ) * ( C.x - A.x ) ) )


// Get the barycentric coordinates of "pt" in triangle [A,B,C].
inline void GetBarycentricCoords2D( 
	Vector2D const &A,
	Vector2D const &B,
	Vector2D const &C,
	Vector2D const &pt,
	float bcCoords[3] )
{
	// Note, because to top and bottom are both x2, the issue washes out in the composite
	float invTriArea = 1.0f / TriArea2DTimesTwo( A, B, C );

	// NOTE: We assume here that the lightmap coordinate vertices go counterclockwise.
	// If not, TriArea2D() is negated so this works out right.
	bcCoords[0] = TriArea2DTimesTwo( B, C, pt ) * invTriArea;
	bcCoords[1] = TriArea2DTimesTwo( C, A, pt ) * invTriArea;
	bcCoords[2] = TriArea2DTimesTwo( A, B, pt ) * invTriArea;
}


// Return true of the sphere might touch the box (the sphere is actually treated
// like a box itself, so this may return true if the sphere's bounding box touches
// a corner of the box but the sphere itself doesn't).
inline bool QuickBoxSphereTest( 
	const Vector& vOrigin,
	float flRadius,
	const Vector& bbMin,
	const Vector& bbMax )
{
	return vOrigin.x - flRadius < bbMax.x && vOrigin.x + flRadius > bbMin.x &&
		vOrigin.y - flRadius < bbMax.y && vOrigin.y + flRadius > bbMin.y && 
		vOrigin.z - flRadius < bbMax.z && vOrigin.z + flRadius > bbMin.z;
}


// Return true of the boxes intersect (but not if they just touch).
inline bool QuickBoxIntersectTest( 
	const Vector& vBox1Min,
	const Vector& vBox1Max,
	const Vector& vBox2Min,
	const Vector& vBox2Max )
{
	return 
		vBox1Min.x < vBox2Max.x && vBox1Max.x > vBox2Min.x &&
		vBox1Min.y < vBox2Max.y && vBox1Max.y > vBox2Min.y && 
		vBox1Min.z < vBox2Max.z && vBox1Max.z > vBox2Min.z;
}


extern float GammaToLinearFullRange( float gamma );
extern float LinearToGammaFullRange( float linear );
extern float GammaToLinear( float gamma );
extern float LinearToGamma( float linear );

extern float SrgbGammaToLinear( float flSrgbGammaValue );
extern float SrgbLinearToGamma( float flLinearValue );
extern float X360GammaToLinear( float fl360GammaValue );
extern float X360LinearToGamma( float flLinearValue );
extern float SrgbGammaTo360Gamma( float flSrgbGammaValue );

// linear (0..4) to screen corrected vertex space (0..1?)
FORCEINLINE float LinearToVertexLight( float f )
{
	extern float lineartovertex[4096];	

	// Gotta clamp before the multiply; could overflow...
	// assume 0..4 range
	int i = RoundFloatToInt( f * 1024.f );

	// Presumably the comman case will be not to clamp, so check that first:
	if( (unsigned)i > 4095 )
	{
		if ( i < 0 )
			i = 0;		// Compare to zero instead of 4095 to save 4 bytes in the instruction stream
		else
			i = 4095;
	}

	return lineartovertex[i];
}


FORCEINLINE unsigned char LinearToLightmap( float f )
{
	extern unsigned char lineartolightmap[4096];	

	// Gotta clamp before the multiply; could overflow...
	int i = RoundFloatToInt( f * 1024.f );	// assume 0..4 range

	// Presumably the comman case will be not to clamp, so check that first:
	if ( (unsigned)i > 4095 )
	{
		if ( i < 0 )
			i = 0;		// Compare to zero instead of 4095 to save 4 bytes in the instruction stream
		else
			i = 4095;
	}

	return lineartolightmap[i];
}

FORCEINLINE void ColorClamp( Vector& color )
{
	float maxc = MAX( color.x, MAX( color.y, color.z ) );
	if ( maxc > 1.0f )
	{
		float ooMax = 1.0f / maxc;
		color.x *= ooMax;
		color.y *= ooMax;
		color.z *= ooMax;
	}

	if ( color[0] < 0.f ) color[0] = 0.f;
	if ( color[1] < 0.f ) color[1] = 0.f;
	if ( color[2] < 0.f ) color[2] = 0.f;
}

inline void ColorClampTruncate( Vector& color )
{
	if (color[0] > 1.0f) color[0] = 1.0f; else if (color[0] < 0.0f) color[0] = 0.0f;
	if (color[1] > 1.0f) color[1] = 1.0f; else if (color[1] < 0.0f) color[1] = 0.0f;
	if (color[2] > 1.0f) color[2] = 1.0f; else if (color[2] < 0.0f) color[2] = 0.0f;
}

// Interpolate a Catmull-Rom spline.
// t is a [0,1] value and interpolates a curve between p2 and p3.
void Catmull_Rom_Spline(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector &output );

// Interpolate a Catmull-Rom spline.
// Returns the tangent of the point at t of the spline
void Catmull_Rom_Spline_Tangent( 
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector &output );

// area under the curve [0..t]
void Catmull_Rom_Spline_Integral( 
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// area under the curve [0..1]
void Catmull_Rom_Spline_Integral( 
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	Vector& output );

// Interpolate a Catmull-Rom spline.
// Normalize p2->p1 and p3->p4 to be the same length as p2->p3
void Catmull_Rom_Spline_Normalize(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector &output );

// area under the curve [0..t]
// Normalize p2->p1 and p3->p4 to be the same length as p2->p3
void Catmull_Rom_Spline_Integral_Normalize(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// Interpolate a Catmull-Rom spline.
// Normalize p2.x->p1.x and p3.x->p4.x to be the same length as p2.x->p3.x
void Catmull_Rom_Spline_NormalizeX(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector &output );

// area under the curve [0..t]
void Catmull_Rom_Spline_NormalizeX(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// Interpolate a Hermite spline.
// t is a [0,1] value and interpolates a curve between p1 and p2 with the deltas d1 and d2.
void Hermite_Spline(
	const Vector &p1,
	const Vector &p2,
	const Vector &d1,
	const Vector &d2,
	float t, 
	Vector& output );

float Hermite_Spline(
	float p1,
	float p2,
	float d1,
	float d2,
	float t );

// t is a [0,1] value and interpolates a curve between p1 and p2 with the slopes p0->p1 and p1->p2
void Hermite_Spline(
	const Vector &p0,
	const Vector &p1,
	const Vector &p2,
	float t, 
	Vector& output );

float Hermite_Spline(
	float p0,
	float p1,
	float p2,
	float t );


void Hermite_SplineBasis( float t, float basis[4] );

void Hermite_Spline( 
	const Quaternion &q0, 
	const Quaternion &q1, 
	const Quaternion &q2, 
	float t, 
	Quaternion &output );


// See http://en.wikipedia.org/wiki/Kochanek-Bartels_curves
// 
// Tension:  -1 = Round -> 1 = Tight
// Bias:     -1 = Pre-shoot (bias left) -> 1 = Post-shoot (bias right)
// Continuity: -1 = Box corners -> 1 = Inverted corners
//
// If T=B=C=0 it's the same matrix as Catmull-Rom.
// If T=1 & B=C=0 it's the same as Cubic.
// If T=B=0 & C=-1 it's just linear interpolation
// 
// See http://news.povray.org/povray.binaries.tutorials/attachment/%3CXns91B880592482seed7@povray.org%3E/Splines.bas.txt
// for example code and descriptions of various spline types...
// 
void Kochanek_Bartels_Spline(
	float tension, 
	float bias, 
	float continuity,
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

void Kochanek_Bartels_Spline_NormalizeX(
	float tension, 
	float bias, 
	float continuity,
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// See link at Kochanek_Bartels_Spline for info on the basis matrix used
void Cubic_Spline(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

void Cubic_Spline_NormalizeX(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// See link at Kochanek_Bartels_Spline for info on the basis matrix used
void BSpline(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

void BSpline_NormalizeX(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// See link at Kochanek_Bartels_Spline for info on the basis matrix used
void Parabolic_Spline(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

void Parabolic_Spline_NormalizeX(
	const Vector &p1,
	const Vector &p2,
	const Vector &p3,
	const Vector &p4,
	float t, 
	Vector& output );

// Evaluate the cubic Bernstein basis for the input parametric coordinate.
// Output is the coefficient for that basis polynomial.
float CubicBasis0( float t );
float CubicBasis1( float t );
float CubicBasis2( float t );
float CubicBasis3( float t );

// quintic interpolating polynomial from Perlin.
// 0->0, 1->1, smooth-in between with smooth tangents
inline float QuinticInterpolatingPolynomial(float t)
{
	// 6t^5-15t^4+10t^3
	return t * t * t *( t * ( t* 6.0 - 15.0 ) + 10.0 );
}

// given a table of sorted tabulated positions, return the two indices and blendfactor to linear
// interpolate. Does a search. Can be used to find the blend value to interpolate between
// keyframes.
void GetInterpolationData( float const *pKnotPositions, 
						   float const *pKnotValues,
						   int nNumValuesinList,
						   int nInterpolationRange,
						   float flPositionToInterpolateAt,
						   bool bWrap,
						   float *pValueA, 
						   float *pValueB,
						   float *pInterpolationValue);
float RangeCompressor( float flValue, float flMin, float flMax, float flBase );

// Get the minimum distance from vOrigin to the bounding box defined by [mins,maxs]
// using voronoi regions.
// 0 is returned if the origin is inside the box.
float CalcSqrDistanceToAABB( const Vector &mins, const Vector &maxs, const Vector &point );
void CalcClosestPointOnAABB( const Vector &mins, const Vector &maxs, const Vector &point, Vector &closestOut );
void CalcSqrDistAndClosestPointOnAABB( const Vector &mins, const Vector &maxs, const Vector &point, Vector &closestOut, float &distSqrOut );

inline float CalcDistanceToAABB( const Vector &mins, const Vector &maxs, const Vector &point )
{
	float flDistSqr = CalcSqrDistanceToAABB( mins, maxs, point );
	return sqrt(flDistSqr);
}

// Get the closest point from P to the (infinite) line through vLineA and vLineB and
// calculate the shortest distance from P to the line.
// If you pass in a value for t, it will tell you the t for (A + (B-A)t) to get the closest point.
// If the closest point lies on the segment between A and B, then 0 <= t <= 1.
void  CalcClosestPointOnLine( const Vector &P, const Vector &vLineA, const Vector &vLineB, Vector &vClosest, float *t=0 );
float CalcDistanceToLine( const Vector &P, const Vector &vLineA, const Vector &vLineB, float *t=0 );
float CalcDistanceSqrToLine( const Vector &P, const Vector &vLineA, const Vector &vLineB, float *t=0 );

// The same three functions as above, except now the line is closed between A and B.
void  CalcClosestPointOnLineSegment( const Vector &P, const Vector &vLineA, const Vector &vLineB, Vector &vClosest, float *t=0 );
float CalcDistanceToLineSegment( const Vector &P, const Vector &vLineA, const Vector &vLineB, float *t=0 );
float CalcDistanceSqrToLineSegment( const Vector &P, const Vector &vLineA, const Vector &vLineB, float *t=0 );

// A function to compute the closes line segment connnection two lines (or false if the lines are parallel, etc.)
bool CalcLineToLineIntersectionSegment(
   const Vector& p1,const Vector& p2,const Vector& p3,const Vector& p4,Vector *s1,Vector *s2,
   float *t1, float *t2 );

// The above functions in 2D
void  CalcClosestPointOnLine2D( Vector2D const &P, Vector2D const &vLineA, Vector2D const &vLineB, Vector2D &vClosest, float *t=0 );
float CalcDistanceToLine2D( Vector2D const &P, Vector2D const &vLineA, Vector2D const &vLineB, float *t=0 );
float CalcDistanceSqrToLine2D( Vector2D const &P, Vector2D const &vLineA, Vector2D const &vLineB, float *t=0 );
void  CalcClosestPointOnLineSegment2D( Vector2D const &P, Vector2D const &vLineA, Vector2D const &vLineB, Vector2D &vClosest, float *t=0 );
float CalcDistanceToLineSegment2D( Vector2D const &P, Vector2D const &vLineA, Vector2D const &vLineB, float *t=0 );
float CalcDistanceSqrToLineSegment2D( Vector2D const &P, Vector2D const &vLineA, Vector2D const &vLineB, float *t=0 );

// Init the mathlib
void MathLib_Init( float gamma = 2.2f, float texGamma = 2.2f, float brightness = 0.0f, int overbright = 2.0f, bool bAllow3DNow = true, bool bAllowSSE = true, bool bAllowSSE2 = true, bool bAllowMMX = true );
bool MathLib_MMXEnabled( void );
bool MathLib_SSEEnabled( void );
bool MathLib_SSE2Enabled( void );

inline float Approach( float target, float value, float speed );
float ApproachAngle( float target, float value, float speed );
float AngleDiff( float destAngle, float srcAngle );
float AngleDistance( float next, float cur );
float AngleNormalize( float angle );

// ensure that 0 <= angle <= 360
float AngleNormalizePositive( float angle );

bool AnglesAreEqual( float a, float b, float tolerance = 0.0f );


void RotationDeltaAxisAngle( const QAngle &srcAngles, const QAngle &destAngles, Vector &deltaAxis, float &deltaAngle );
void RotationDelta( const QAngle &srcAngles, const QAngle &destAngles, QAngle *out );

//-----------------------------------------------------------------------------
// Clips a line segment such that only the portion in the positive half-space
// of the plane remains.  If the segment is entirely clipped, the vectors
// are set to vec3_invalid (all components are FLT_MAX).
//
// flBias is added to the dot product with the normal.  A positive bias 
// results in a more inclusive positive half-space, while a negative bias
// results in a more exclusive positive half-space.
//-----------------------------------------------------------------------------
void ClipLineSegmentToPlane( const Vector &vNormal, const Vector &vPlanePoint, Vector *p1, Vector *p2, float flBias = 0.0f );

void ComputeTrianglePlane( const Vector& v1, const Vector& v2, const Vector& v3, Vector& normal, float& intercept );
int PolyFromPlane( Vector *pOutVerts, const Vector& normal, float dist, float fHalfScale = 9000.0f );
void PolyFromPlane_SIMD( fltx4 *pOutVerts, const fltx4 & plane, float fHalfScale = 9000.0f );
int ClipPolyToPlane( Vector *inVerts, int vertCount, Vector *outVerts, const Vector& normal, float dist, float fOnPlaneEpsilon = 0.1f );
int ClipPolyToPlane_SIMD( fltx4 *pInVerts, int vertCount, fltx4 *pOutVerts, const fltx4& plane, float fOnPlaneEpsilon = 0.1f );
int ClipPolyToPlane_Precise( double *inVerts, int vertCount, double *outVerts, const double *normal, double dist, double fOnPlaneEpsilon = 0.1 );
float TetrahedronVolume( const Vector &p0, const Vector &p1, const Vector &p2, const Vector &p3 );
float TriangleArea( const Vector &p0, const Vector &p1, const Vector &p2 );

/// return surface area of an AABB
FORCEINLINE float BoxSurfaceArea( Vector const &vecBoxMin, Vector const &vecBoxMax )
{
	Vector boxdim = vecBoxMax - vecBoxMin;
	return 2.0 * ( ( boxdim[0] * boxdim[2] ) + ( boxdim[0] * boxdim[1] ) + ( boxdim[1] * boxdim[2] ) );
}

//-----------------------------------------------------------------------------
// Computes a reasonable tangent space for a triangle
//-----------------------------------------------------------------------------
void CalcTriangleTangentSpace( const Vector &p0, const Vector &p1, const Vector &p2,
							  const Vector2D &t0, const Vector2D &t1, const Vector2D& t2,
							  Vector &sVect, Vector &tVect );

//-----------------------------------------------------------------------------
// Transforms a AABB into another space; which will inherently grow the box.
//-----------------------------------------------------------------------------
void TransformAABB( const matrix3x4_t &in1, const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut );

//-----------------------------------------------------------------------------
// Uses the inverse transform of in1
//-----------------------------------------------------------------------------
void ITransformAABB( const matrix3x4_t &in1, const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut );

//-----------------------------------------------------------------------------
// Rotates a AABB into another space; which will inherently grow the box. 
// (same as TransformAABB, but doesn't take the translation into account)
//-----------------------------------------------------------------------------
void RotateAABB( const matrix3x4_t &in1, const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut );

//-----------------------------------------------------------------------------
// Uses the inverse transform of in1
//-----------------------------------------------------------------------------
void IRotateAABB( const matrix3x4_t &in1, const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut );

//-----------------------------------------------------------------------------
// Transform a plane
//-----------------------------------------------------------------------------
inline void MatrixTransformPlane( const matrix3x4_t &src, const cplane_t &inPlane, cplane_t &outPlane )
{
	// What we want to do is the following:
	// 1) transform the normal into the new space.
	// 2) Determine a point on the old plane given by plane dist * plane normal
	// 3) Transform that point into the new space
	// 4) Plane dist = DotProduct( new normal, new point )

	// An optimized version, which works if the plane is orthogonal.
	// 1) Transform the normal into the new space
	// 2) Realize that transforming the old plane point into the new space
	// is given by [ d * n'x + Tx, d * n'y + Ty, d * n'z + Tz ]
	// where d = old plane dist, n' = transformed normal, Tn = translational component of transform
	// 3) Compute the new plane dist using the dot product of the normal result of #2

	// For a correct result, this should be an inverse-transpose matrix
	// but that only matters if there are nonuniform scale or skew factors in this matrix.
	VectorRotate( inPlane.normal, src, outPlane.normal );
	outPlane.dist = inPlane.dist * DotProduct( outPlane.normal, outPlane.normal );
	outPlane.dist += outPlane.normal.x * src[0][3] + outPlane.normal.y * src[1][3] + outPlane.normal.z * src[2][3];
}

inline void MatrixITransformPlane( const matrix3x4_t &src, const cplane_t &inPlane, cplane_t &outPlane )
{
	// The trick here is that Tn = translational component of transform,
	// but for an inverse transform, Tn = - R^-1 * T
	Vector vecTranslation;
	MatrixGetColumn( src, 3, vecTranslation );

	Vector vecInvTranslation;
	VectorIRotate( vecTranslation, src, vecInvTranslation );

	VectorIRotate( inPlane.normal, src, outPlane.normal );
	outPlane.dist = inPlane.dist * DotProduct( outPlane.normal, outPlane.normal );
	outPlane.dist -= outPlane.normal.x * vecInvTranslation[0] + outPlane.normal.y * vecInvTranslation[1] + outPlane.normal.z * vecInvTranslation[2];
}

int CeilPow2( int in );
int FloorPow2( int in );

FORCEINLINE float * UnpackNormal_HEND3N( const unsigned int *pPackedNormal, float *pNormal )
{
	int temp[3];
	temp[0] = ((*pPackedNormal >> 0L) & 0x7ff);
	if ( temp[0] & 0x400 )
	{
		temp[0] = 2048 - temp[0];
	}
	temp[1] = ((*pPackedNormal >> 11L) & 0x7ff);
	if ( temp[1] & 0x400 )
	{
		temp[1] = 2048 - temp[1];
	}
	temp[2] = ((*pPackedNormal >> 22L) & 0x3ff);
	if ( temp[2] & 0x200 )
	{
		temp[2] = 1024 - temp[2];
	}
	pNormal[0] = (float)temp[0] * 1.0f/1023.0f;
	pNormal[1] = (float)temp[1] * 1.0f/1023.0f;
	pNormal[2] = (float)temp[2] * 1.0f/511.0f;
	return pNormal;
}


FORCEINLINE unsigned int * PackNormal_HEND3N( const float *pNormal, unsigned int *pPackedNormal )
{
	int temp[3];

	temp[0] = Float2Int( pNormal[0] * 1023.0f );
	temp[1] = Float2Int( pNormal[1] * 1023.0f );
	temp[2] = Float2Int( pNormal[2] * 511.0f );

	// the normal is out of bounds, determine the source and fix
	// clamping would be even more of a slowdown here
	Assert( temp[0] >= -1023 && temp[0] <= 1023 );
	Assert( temp[1] >= -1023 && temp[1] <= 1023 );
	Assert( temp[2] >= -511 && temp[2] <= 511 );
	
	*pPackedNormal = ( ( temp[2] & 0x3ff ) << 22L ) |
                     ( ( temp[1] & 0x7ff ) << 11L ) |
                     ( ( temp[0] & 0x7ff ) << 0L );
	return pPackedNormal;
}


FORCEINLINE unsigned int * PackNormal_HEND3N( float nx, float ny, float nz, unsigned int *pPackedNormal )
{
	int temp[3];

	temp[0] = Float2Int( nx * 1023.0f );
	temp[1] = Float2Int( ny * 1023.0f );
	temp[2] = Float2Int( nz * 511.0f );

	// the normal is out of bounds, determine the source and fix
	// clamping would be even more of a slowdown here
	Assert( temp[0] >= -1023 && temp[0] <= 1023 );
	Assert( temp[1] >= -1023 && temp[1] <= 1023 );
	Assert( temp[2] >= -511 && temp[2] <= 511 );
	
	*pPackedNormal = ( ( temp[2] & 0x3ff ) << 22L ) |
                     ( ( temp[1] & 0x7ff ) << 11L ) |
                     ( ( temp[0] & 0x7ff ) << 0L );
	return pPackedNormal;
}



FORCEINLINE float * UnpackNormal_SHORT2( const unsigned int *pPackedNormal, float *pNormal, bool bIsTangent = FALSE )
{
	// Unpacks from Jason's 2-short format (fills in a 4th binormal-sign (+1/-1) value, if this is a tangent vector)

	// FIXME: short math is slow on 360 - use ints here instead (bit-twiddle to deal w/ the sign bits)
	short iX = (*pPackedNormal & 0x0000FFFF);
	short iY = (*pPackedNormal & 0xFFFF0000) >> 16;

	float zSign = +1;
	if ( iX < 0 )
	{
		zSign = -1;
		iX    = -iX;
	}
	float tSign = +1;
	if ( iY < 0 )
	{
		tSign = -1;
		iY    = -iY;
	}

	pNormal[0] = ( iX - 16384.0f ) / 16384.0f;
	pNormal[1] = ( iY - 16384.0f ) / 16384.0f;
	float mag = ( pNormal[0]*pNormal[0] + pNormal[1]*pNormal[1] );
	if ( mag > 1.0f )
	{
		mag = 1.0f;
	}
	pNormal[2] = zSign*sqrtf( 1.0f - mag );
	if ( bIsTangent )
	{
		pNormal[3] = tSign;
	}

	return pNormal;
}

FORCEINLINE unsigned int * PackNormal_SHORT2( float nx, float ny, float nz, unsigned int *pPackedNormal, float binormalSign = +1.0f )
{
	// Pack a vector (ASSUMED TO BE NORMALIZED) into Jason's 4-byte (SHORT2) format.
	// This simply reconstructs Z from X & Y. It uses the sign bits of the X & Y coords
	// to reconstruct the sign of Z and, if this is a tangent vector, the sign of the
	// binormal (this is needed because tangent/binormal vectors are supposed to follow
	// UV gradients, but shaders reconstruct the binormal from the tangent and normal
	// assuming that they form a right-handed basis).

	nx += 1;					// [-1,+1] -> [0,2]
	ny += 1;
	nx *= 16384.0f;				// [ 0, 2] -> [0,32768]
	ny *= 16384.0f;

	// '0' and '32768' values are invalid encodings
	nx = MAX( nx, 1.0f );		// Make sure there are no zero values
	ny = MAX( ny, 1.0f );
	nx = MIN( nx, 32767.0f );	// Make sure there are no 32768 values
	ny = MIN( ny, 32767.0f );

	if ( nz < 0.0f )
		nx = -nx;				// Set the sign bit for z

	ny *= binormalSign;			// Set the sign bit for the binormal (use when encoding a tangent vector)

	// FIXME: short math is slow on 360 - use ints here instead (bit-twiddle to deal w/ the sign bits), also use Float2Int()
	short sX = (short)nx;		// signed short [1,32767]
	short sY = (short)ny;

	*pPackedNormal = ( sX & 0x0000FFFF ) | ( sY << 16 ); // NOTE: The mask is necessary (if sX is negative and cast to an int...)

	return pPackedNormal;
}

FORCEINLINE unsigned int * PackNormal_SHORT2( const float *pNormal, unsigned int *pPackedNormal, float binormalSign = +1.0f )
{
	return PackNormal_SHORT2( pNormal[0], pNormal[1], pNormal[2], pPackedNormal, binormalSign );
}

// Unpacks a UBYTE4 normal (for a tangent, the result's fourth component receives the binormal 'sign')
FORCEINLINE float * UnpackNormal_UBYTE4( const unsigned int *pPackedNormal, float *pNormal, bool bIsTangent = FALSE )
{
	unsigned char cX, cY;
	if ( bIsTangent )
	{
		cX = *pPackedNormal >> 16;					// Unpack Z
		cY = *pPackedNormal >> 24;					// Unpack W
	}
	else
	{
		cX = *pPackedNormal >>  0;					// Unpack X
		cY = *pPackedNormal >>  8;					// Unpack Y
	}

	float x = cX - 128.0f;
	float y = cY - 128.0f;
	float z;

	float zSignBit = x < 0 ? 1.0f : 0.0f;			// z and t negative bits (like slt asm instruction)
	float tSignBit = y < 0 ? 1.0f : 0.0f;
	float zSign    = -( 2*zSignBit - 1 );			// z and t signs
	float tSign    = -( 2*tSignBit - 1 );

	x = x*zSign - zSignBit;							// 0..127
	y = y*tSign - tSignBit;
	x = x - 64;										// -64..63
	y = y - 64;

	float xSignBit = x < 0 ? 1.0f : 0.0f;	// x and y negative bits (like slt asm instruction)
	float ySignBit = y < 0 ? 1.0f : 0.0f;
	float xSign    = -( 2*xSignBit - 1 );			// x and y signs
	float ySign    = -( 2*ySignBit - 1 );

	x = ( x*xSign - xSignBit ) / 63.0f;				// 0..1 range
	y = ( y*ySign - ySignBit ) / 63.0f;
	z = 1.0f - x - y;

	float oolen	 = 1.0f / sqrt( x*x + y*y + z*z );	// Normalize and
	x			*= oolen * xSign;					// Recover signs
	y			*= oolen * ySign;
	z			*= oolen * zSign;

	pNormal[0] = x;
	pNormal[1] = y;
	pNormal[2] = z;
	if ( bIsTangent )
	{
		pNormal[3] = tSign;
	}

	return pNormal;
}

//////////////////////////////////////////////////////////////////////////////
// See: http://www.oroboro.com/rafael/docserv.php/index/programming/article/unitv2
//
// UBYTE4 encoding, using per-octant projection onto x+y+z=1
// Assume input vector is already unit length
//
// binormalSign specifies 'sign' of binormal, stored in t sign bit of tangent
// (lets the shader know whether norm/tan/bin form a right-handed basis)
//
// bIsTangent is used to specify which WORD of the output to store the data
// The expected usage is to call once with the normal and once with
// the tangent and binormal sign flag, bitwise OR'ing the returned DWORDs
FORCEINLINE unsigned int * PackNormal_UBYTE4( float nx, float ny, float nz, unsigned int *pPackedNormal, bool bIsTangent = false, float binormalSign = +1.0f )
{
	float xSign = nx < 0.0f ? -1.0f : 1.0f;			// -1 or 1 sign
	float ySign = ny < 0.0f ? -1.0f : 1.0f;
	float zSign = nz < 0.0f ? -1.0f : 1.0f;
	float tSign = binormalSign;
	Assert( ( binormalSign == +1.0f ) || ( binormalSign == -1.0f ) );

	float xSignBit = 0.5f*( 1 - xSign );			// [-1,+1] -> [1,0]
	float ySignBit = 0.5f*( 1 - ySign );			// 1 is negative bit (like slt instruction)
	float zSignBit = 0.5f*( 1 - zSign );
	float tSignBit = 0.5f*( 1 - binormalSign );		

	float absX = xSign*nx;							// 0..1 range (abs)
	float absY = ySign*ny;
	float absZ = zSign*nz;

	float xbits = absX / ( absX + absY + absZ );	// Project onto x+y+z=1 plane
	float ybits = absY / ( absX + absY + absZ );

	xbits *= 63;									// 0..63
	ybits *= 63;

	xbits  = xbits * xSign - xSignBit;				// -64..63 range
	ybits  = ybits * ySign - ySignBit;
	xbits += 64.0f;									// 0..127 range
	ybits += 64.0f;

	xbits  = xbits * zSign - zSignBit;				// Negate based on z and t
	ybits  = ybits * tSign - tSignBit;				// -128..127 range

	xbits += 128.0f;								// 0..255 range
	ybits += 128.0f;

	unsigned char cX = (unsigned char) xbits;
	unsigned char cY = (unsigned char) ybits;

	if ( !bIsTangent )
		*pPackedNormal = (cX <<  0) | (cY <<  8);	// xy for normal
	else						   
		*pPackedNormal = (cX << 16) | (cY << 24);	// zw for tangent

	return pPackedNormal;
}

FORCEINLINE unsigned int * PackNormal_UBYTE4( const float *pNormal, unsigned int *pPackedNormal, bool bIsTangent = false, float binormalSign = +1.0f )
{
	return PackNormal_UBYTE4( pNormal[0], pNormal[1], pNormal[2], pPackedNormal, bIsTangent, binormalSign );
}

FORCEINLINE void RGB2YUV( int &nR, int &nG, int &nB, float &fY, float &fU, float &fV, bool bApplySaturationCurve )
{
	// YUV conversion:
	//  |Y|   |  0.299f     0.587f     0.114f   |   |R|
	//  |U| = | -0.14713f  -0.28886f   0.436f   | x |G|
	//  |V|   |  0.615f    -0.51499f  -0.10001f |   |B|
	//
	// The coefficients in the first row sum to one, whereas the 2nd and 3rd rows each sum to zero (UV (0,0) means greyscale).
	// Ranges are Y [0,1], U [-0.436,+0.436] and V [-0.615,+0.615].
	// We scale and offset to [0,1] and allow the caller to round as they please.

	fY = (  0.29900f*nR +  0.58700f*nG +  0.11400f*nB ) / 255;
	fU = ( -0.14713f*nR + -0.28886f*nG +  0.43600f*nB )*( 0.5f / 0.436f ) / 255 + 0.5f;
	fV = (  0.61500f*nR + -0.51499f*nG + -0.10001f*nB )*( 0.5f / 0.615f ) / 255 + 0.5f;

	if ( bApplySaturationCurve )
	{
		// Apply a curve to saturation, and snap-to-grey for low saturations
		const float SNAP_TO_GREY = 0;//0.0125f; Disabled, saturation curve seems sufficient
		float dX, dY, sat, scale;
		dX    = 2*( fU - 0.5f );
		dY    = 2*( fV - 0.5f );
		sat   = sqrtf( dX*dX + dY*dY );
		sat   = clamp( ( sat*( 1 + SNAP_TO_GREY ) - SNAP_TO_GREY ), 0, 1 );
		scale = ( sat == 0 ) ? 0 : MIN( ( sqrtf( sat ) / sat ), 4.0f );
		fU    = 0.5f + scale*( fU - 0.5f );
		fV    = 0.5f + scale*( fV - 0.5f );
	}
}

#ifdef _X360
// Used for direct CPU access to VB data on 360 (used by shaderapi, studiorender and engine)
struct VBCPU_AccessInfo_t
{
	// Points to the GPU data pointer in the CVertexBuffer struct (VB data can be relocated during level transitions)
	const byte **ppBaseAddress;
	// pBaseAddress should be computed from ppBaseAddress immediately before use
	const byte  *pBaseAddress;
	int          nStride;
	int          nPositionOffset;
	int          nTexCoord0_Offset;
	int          nNormalOffset;
	int          nBoneIndexOffset;
	int          nBoneWeightOffset;
	int          nCompressionType;
	// TODO: if needed, add colour and tangents
};
#endif

//-----------------------------------------------------------------------------
// Convert RGB to HSV
//-----------------------------------------------------------------------------
void RGBtoHSV( const Vector &rgb, Vector &hsv );


//-----------------------------------------------------------------------------
// Convert HSV to RGB
//-----------------------------------------------------------------------------
void HSVtoRGB( const Vector &hsv, Vector &rgb );


//-----------------------------------------------------------------------------
// Fast version of pow and log
//-----------------------------------------------------------------------------
#ifndef _PS3 // these actually aren't fast (or correct) on the PS3
float FastLog2(float i);			// log2( i )
float FastPow2(float i);			// 2^i
float FastPow(float a, float b);	// a^b
float FastPow10( float i );			// 10^i
#else
inline float FastLog2(float i) {return logbf(i);}			// log2( i )
inline float FastPow2(float i) {return exp2f(i);}			// 2^i
inline float FastPow(float a, float b) {return powf(a,b);}	// a^b
#define LOGBASE2OF10 3.3219280948873623478703194294893901758648313930
inline float FastPow10( float i ) { return exp2f( i * LOGBASE2OF10 ); }			// 10^i, transform to base two, so log2(10^y) = y log2(10) . log2(10) = 3.3219280948873623478703194294893901758648313930
#endif

//-----------------------------------------------------------------------------
// For testing float equality
//-----------------------------------------------------------------------------

inline bool CloseEnough( float a, float b, float epsilon = EQUAL_EPSILON )
{
	return fabs( a - b ) <= epsilon;
}

inline bool CloseEnough( const Vector &a, const Vector &b, float epsilon = EQUAL_EPSILON )
{
	return fabs( a.x - b.x ) <= epsilon &&
		fabs( a.y - b.y ) <= epsilon &&
		fabs( a.z - b.z ) <= epsilon;
}

// Fast compare
// maxUlps is the maximum error in terms of Units in the Last Place. This 
// specifies how big an error we are willing to accept in terms of the value
// of the least significant digit of the floating point numbers 
// representation. maxUlps can also be interpreted in terms of how many 
// representable floats we are willing to accept between A and B. 
// This function will allow maxUlps-1 floats between A and B.
bool AlmostEqual(float a, float b, int maxUlps = 10);

inline bool AlmostEqual( const Vector &a, const Vector &b, int maxUlps = 10)
{
	return AlmostEqual( a.x, b.x, maxUlps ) &&
		AlmostEqual( a.y, b.y, maxUlps ) &&
		AlmostEqual( a.z, b.z, maxUlps );
}

inline Vector Approach( Vector target, Vector value, float speed )
{
	Vector diff = (target - value);
	float delta = diff.Length();

	if ( delta > speed )
		value += diff.Normalized() * speed;
	else if ( delta < -speed )
		value -= diff.Normalized() * speed;
	else 
		value = target;
		
	return value;
}

inline float Approach( float target, float value, float speed )
{
	float delta = target - value;

#if defined(_X360) || defined( _PS3 ) // use conditional move for speed on 360

	return fsel( delta-speed,	// delta >= speed ?
				 value + speed,	// if delta == speed, then value + speed == value + delta == target  
				 fsel( (-speed) - delta, // delta <= -speed
						value - speed,
						target )
				);  // delta < speed && delta > -speed

#else

	if ( delta > speed )
		value += speed;
	else if ( delta < -speed )
		value -= speed;
	else 
		value = target;
		
	return value;

#endif
}

inline void matrix3x4_t::Init( const Vector &xAxis, const Vector &yAxis, const Vector &zAxis, const Vector &vecOrigin )
{
	MatrixInitialize( *this, vecOrigin, xAxis, yAxis, zAxis );
}

inline void matrix3x4_t::Init( const QAngle &angs, const Vector &pos )
{
	AngleMatrix( angs, pos, *this );
}

inline void matrix3x4_t::Init( const QAngle &angs )
{
	AngleMatrix( angs, *this );
}

inline void matrix3x4_t::Init( const RadianEuler &angs, const Vector &pos )
{
	AngleMatrix( angs, pos, *this );
}

inline void matrix3x4_t::Init( const RadianEuler &angs )
{
	AngleMatrix( angs, *this );
}

inline void matrix3x4_t::Init( const Quaternion &quat, const Vector &pos )
{
	QuaternionMatrix( quat, pos, *this );
}

inline void matrix3x4_t::Init( const Quaternion &quat )
{
	QuaternionMatrix( quat, *this );
}

inline void matrix3x4_t::Init( const Vector &fwd )
{
	VectorMatrix( fwd, *this );
}

inline void matrix3x4_t::Init( const Vector &fwd, const Vector &pos )
{
	VectorMatrix( fwd, *this );
	SetOrigin( pos );
}

inline void matrix3x4_t::Init( const Vector &axis_of_rotation, float degrees )
{
	MatrixBuildRotationAboutAxis( axis_of_rotation, degrees, *this );
}

inline void matrix3x4_t::Init( const Vector &axis_of_rotation, float degrees, const Vector &pos )
{
	MatrixBuildRotationAboutAxis( axis_of_rotation, degrees, *this );
	SetOrigin( pos );
}

inline QAngle matrix3x4_t::ToQAngle() const
{
	QAngle out;
	MatrixAngles( *this, out );
	return out;
}

// matrix3x4_t::ToTransform() : see "transform.h"

inline RadianEuler matrix3x4_t::ToRadianEuler() const
{
	RadianEuler out;
	MatrixAngles( *this, out );
	return out;
}

inline Quaternion matrix3x4_t::ToQuaternion() const
{
	Quaternion out;
	MatrixQuaternion( *this, out );
	return out;
}

inline void matrix3x4_t::SetToIdentity()
{
	SetIdentityMatrix( *this );
}

inline bool matrix3x4_t::IsEqualTo( const matrix3x4_t &other, float flTolerance ) const
{
	return MatricesAreEqual( *this, other, flTolerance );
}

inline void matrix3x4_t::GetBasisVectorsFLU( Vector *pForward, Vector *pLeft, Vector *pUp ) const
{
	return MatrixVectorsFLU( *this, pForward, pLeft, pUp );
}

inline Vector matrix3x4_t::TransformVector( const Vector &v0 ) const
{
	return VectorTransform( v0, *this );
}

inline Vector matrix3x4_t::RotateVector( const Vector &v0 ) const
{
	return VectorRotate( v0, *this );
}

inline Vector matrix3x4_t::TransformVectorByInverse( const Vector &v0 ) const
{
	return VectorITransform( v0, *this );
}

inline Vector matrix3x4_t::RotateVectorByInverse( const Vector &v0 ) const
{
	Vector tmp;
	VectorIRotate( v0, *this, tmp );
	return tmp;
}

inline Vector matrix3x4_t::RotateExtents( const Vector &vBoxExtents ) const
{
	return Vector( DotProductAbs( vBoxExtents, m_flMatVal[ 0 ] ), DotProductAbs( vBoxExtents, m_flMatVal[ 1 ] ), DotProductAbs( vBoxExtents, m_flMatVal[ 2 ] ) );
}

inline Vector matrix3x4_t::GetColumn( MatrixAxisType_t nColumn ) const
{
	return Vector( m_flMatVal[ 0 ][ nColumn ], m_flMatVal[ 1 ][ nColumn ], m_flMatVal[ 2 ][ nColumn ] );
}

inline void matrix3x4_t::SetColumn( const Vector &vColumn, MatrixAxisType_t nColumn )
{
	m_flMatVal[ 0 ][ nColumn ] = vColumn.x;
	m_flMatVal[ 1 ][ nColumn ] = vColumn.y;
	m_flMatVal[ 2 ][ nColumn ] = vColumn.z;
}

inline void matrix3x4_t::InverseTR( matrix3x4_t &out ) const
{
	::MatrixInvert( *this, out );
}

inline matrix3x4_t matrix3x4_t::InverseTR() const
{
	matrix3x4_t out;
	::MatrixInvert( *this, out );
	return out;
}

inline void matrix3x4_t::TransformAABB( const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut ) const
{
	::TransformAABB( *this, vecMinsIn, vecMaxsIn, vecMinsOut, vecMaxsOut );
}

inline void matrix3x4_t::TransformAABBByInverse( const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut ) const
{
	::ITransformAABB( *this, vecMinsIn, vecMaxsIn, vecMinsOut, vecMaxsOut );
}

inline QAngle matrix3x4_t::TransformToLocalSpace( const QAngle &angles ) const
{
	return TransformAnglesToLocalSpace( angles, *this );
}

inline QAngle matrix3x4_t::TransformToWorldSpace( const QAngle &angles ) const
{
	return TransformAnglesToWorldSpace( angles, *this );
}

inline Vector matrix3x4_t::Rotate( const Vector &in ) const
{
	Vector out;
	VectorRotate( in, *this, out );
	return out;
}

inline Vector matrix3x4_t::RotateByInverse( const Vector &in ) const
{
	Vector out;
	VectorIRotate( in, *this, out );
	return out;
}

inline void matrix3x4_t::RotateAABB( const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut ) const
{
	::RotateAABB( *this, vecMinsIn, vecMaxsIn, vecMinsOut, vecMaxsOut );
}

inline void matrix3x4_t::RotateAABBByInverse( const Vector &vecMinsIn, const Vector &vecMaxsIn, Vector &vecMinsOut, Vector &vecMaxsOut ) const
{
	::IRotateAABB( *this, vecMinsIn, vecMaxsIn, vecMinsOut, vecMaxsOut );
}

inline bool matrix3x4_t::IsEqualEnough( const matrix3x4_t &other, float flTolerance ) const
{
	return MatricesAreEqual( *this, other, flTolerance );
}

inline void matrix3x4_t::TransformPlane( const cplane_t &inPlane, cplane_t &outPlane ) const
{
	::MatrixTransformPlane( *this, inPlane, outPlane );
}
inline void matrix3x4_t::TransformPlaneByInverse( const cplane_t &inPlane, cplane_t &outPlane ) const
{
	::MatrixITransformPlane( *this, inPlane, outPlane );
}

inline float matrix3x4_t::GetOrthogonalityError() const
{
	return
		fabsf( m_flMatVal[ 0 ][ 0 ] * m_flMatVal[ 0 ][ 1 ] + m_flMatVal[ 1 ][ 0 ] * m_flMatVal[ 1 ][ 1 ] + m_flMatVal[ 2 ][ 0 ] * m_flMatVal[ 2 ][ 1 ] ) +
		fabsf( m_flMatVal[ 0 ][ 1 ] * m_flMatVal[ 0 ][ 2 ] + m_flMatVal[ 1 ][ 1 ] * m_flMatVal[ 1 ][ 2 ] + m_flMatVal[ 2 ][ 1 ] * m_flMatVal[ 2 ][ 2 ] ) +
		fabsf( m_flMatVal[ 0 ][ 2 ] * m_flMatVal[ 0 ][ 0 ] + m_flMatVal[ 1 ][ 2 ] * m_flMatVal[ 1 ][ 0 ] + m_flMatVal[ 2 ][ 2 ] * m_flMatVal[ 2 ][ 0 ] );
}

inline matrix3x4_t Quaternion::ToMatrix() const
{
	matrix3x4_t mat;
	mat.Init( *this );
	return mat;
}

inline matrix3x4_t QAngle::ToMatrix() const
{
	matrix3x4_t mat;
	AngleMatrix( *this, mat );
	return mat;
}

inline Quaternion QAngle::ToQuaternion() const
{
	return AngleQuaternion( *this );
}

inline float matrix3x4_t::GetDeterminant() const
{
	return
		m_flMatVal[ 0 ][ 0 ] * ( m_flMatVal[ 1 ][ 1 ] * m_flMatVal[ 2 ][ 2 ] - m_flMatVal[ 2 ][ 1 ] * m_flMatVal[ 1 ][ 2 ] )
		- m_flMatVal[ 0 ][ 1 ] * ( m_flMatVal[ 1 ][ 0 ] * m_flMatVal[ 2 ][ 2 ] - m_flMatVal[ 1 ][ 2 ] * m_flMatVal[ 2 ][ 0 ] )
		+ m_flMatVal[ 0 ][ 2 ] * ( m_flMatVal[ 1 ][ 0 ] * m_flMatVal[ 2 ][ 1 ] - m_flMatVal[ 1 ][ 1 ] * m_flMatVal[ 2 ][ 0 ] );
}

inline float GetRelativeDifferenceSqr( const Vector &a, const Vector &b )
{
	return ( a - b ).LengthSqr() / Max( 1.0f, Max( a.LengthSqr(), b.LengthSqr() ) );
}


inline float GetRelativeDifference( const Vector &a, const Vector &b )
{
	return sqrtf( GetRelativeDifferenceSqr( a, b ) );
}


// a good measure of relative error between two TR matrices, perhaps with a reasonable scale
inline float GetRelativeDifference( const matrix3x4_t &a, const matrix3x4_t &b )
{
	return sqrtf( Max( Max( GetRelativeDifferenceSqr( a.GetColumn( X_AXIS ), b.GetColumn( X_AXIS ) ),
		GetRelativeDifferenceSqr( a.GetColumn( Y_AXIS ), b.GetColumn( Y_AXIS ) ) ),
		Max( GetRelativeDifferenceSqr( a.GetColumn( Z_AXIS ), b.GetColumn( Z_AXIS ) ),
		GetRelativeDifferenceSqr( a.GetOrigin(), b.GetOrigin() ) )
		)
		);
}



inline float matrix3x4_t::GetSylvestersCriterion()const
{
	// http://en.wikipedia.org/wiki/Sylvester%27s_criterion
	float flDet1 = m_flMatVal[ 0 ][ 0 ];
	float flDet2 = m_flMatVal[ 0 ][ 0 ] * m_flMatVal[ 1 ][ 1 ] - m_flMatVal[ 1 ][ 0 ] * m_flMatVal[ 0 ][ 1 ];
	float flDet3 = GetDeterminant();
	return MIN( MIN( flDet1, flDet2 ), flDet3 );
}



// Generate the corner points of a box:
// +y       _+z
// ^        /|
// |       /
// |  3---7   
//   /|  /|
//  / | / |
// 2---6  |
// |  1|--5
// | / | /
// |/  |/
// 0---4   --> +x
//
void PointsFromBox( const Vector &mins, const Vector &maxs, Vector *points );
void BuildTransformedBox( Vector *v2, Vector const &bbmin, Vector const &bbmax, const matrix3x4_t& m );



#endif	// MATH_BASE_H

