//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//


#include "mathlib/ssemath.h"
#include "mathlib/ssequaternion.h"
#include "mathlib/compressed_vector.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"

const fltx4 Four_PointFives={0.5,0.5,0.5,0.5};
const fltx4 Four_Zeros={0.0,0.0,0.0,0.0};
const fltx4 Four_Ones={1.0,1.0,1.0,1.0};
const fltx4 Four_Twos={2.0,2.0,2.0,2.0};
const fltx4 Four_Threes={3.0,3.0,3.0,3.0};
const fltx4 Four_Fours={4.0,4.0,4.0,4.0};
const fltx4 Four_Origin={0,0,0,1};
const fltx4 Four_NegativeOnes={-1,-1,-1,-1};

const fltx4 Four_2ToThe21s={ (float) (1<<21), (float) (1<<21), (float) (1<<21), (float)(1<<21) };
const fltx4 Four_2ToThe22s={ (float) (1<<22), (float) (1<<22), (float) (1<<22), (float)(1<<22) };
const fltx4 Four_2ToThe23s={ (float) (1<<23), (float) (1<<23), (float) (1<<23), (float)(1<<23) };
const fltx4 Four_2ToThe24s={ (float) (1<<24), (float) (1<<24), (float) (1<<24), (float)(1<<24) };
const fltx4 Four_Thirds={ 0.33333333, 0.33333333, 0.33333333, 0.33333333 };
const fltx4 Four_TwoThirds={ 0.66666666, 0.66666666, 0.66666666, 0.66666666 };
const fltx4 Four_Point225s={ .225, .225, .225, .225 };
const fltx4 Four_Epsilons={FLT_EPSILON,FLT_EPSILON,FLT_EPSILON,FLT_EPSILON};
const fltx4 Four_DegToRad= { ((float)(M_PI_F / 180.f)), ((float)(M_PI_F / 180.f)), ((float)(M_PI_F / 180.f)), ((float)(M_PI_F / 180.f))};

const fltx4 Four_FLT_MAX={FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX};
const fltx4 Four_Negative_FLT_MAX={-FLT_MAX,-FLT_MAX,-FLT_MAX,-FLT_MAX};
const fltx4 g_SIMD_0123 = { 0., 1., 2., 3. };

const fltx4 Four_LinearToGammaCoefficients_A = { -3.7295, -3.7295, -3.7295, -3.7295 };
const fltx4 Four_LinearToGammaCoefficients_B = { 8.9635,  8.9635,  8.9635,  8.9635 };
const fltx4 Four_LinearToGammaCoefficients_C = { -7.7397,  -7.7397,  -7.7397,  -7.7397 };
const fltx4 Four_LinearToGammaCoefficients_D = {3.443, 3.443, 3.443, 3.443 };
const fltx4 Four_LinearToGammaCoefficients_E = { 0.048, 0.048, 0.048, 0.048 };

const fltx4 Four_GammaToLinearCoefficients_A = { .1731, .1731, .1731, .1731 };
const fltx4 Four_GammaToLinearCoefficients_B = { .8717, .8717, .8717, .8717 };
const fltx4 Four_GammaToLinearCoefficients_C = { -.0452, -.0452, -.0452, -.0452 };
const fltx4 Four_GammaToLinearCoefficients_D = { .0012, .0012, .0012, .0012 };

const fltx4 g_QuatMultRowSign[4] =
{
	{  1.0f,  1.0f, -1.0f, 1.0f },
	{ -1.0f,  1.0f,  1.0f, 1.0f },
	{  1.0f, -1.0f,  1.0f, 1.0f },
	{ -1.0f, -1.0f, -1.0f, 1.0f }
};


const int32 ALIGN16 g_SIMD_clear_signmask[4]= {0x7fffffff,0x7fffffff,0x7fffffff,0x7fffffff};
const int32 ALIGN16 g_SIMD_signmask[4]= { static_cast<int32>(0x80000000), static_cast<int32>(0x80000000), static_cast<int32>(0x80000000), static_cast<int32>(0x80000000) };
const int32 ALIGN16 g_SIMD_lsbmask[4]= { static_cast<int32>(0xfffffffe), static_cast<int32>(0xfffffffe), static_cast<int32>(0xfffffffe), static_cast<int32>(0xfffffffe) };
const int32 ALIGN16 g_SIMD_clear_wmask[4]= { static_cast<int32>(0xffffffff), static_cast<int32>(0xffffffff), static_cast<int32>(0xffffffff), 0 };
const int32 ALIGN16 g_SIMD_AllOnesMask[4]= { static_cast<int32>(0xffffffff), static_cast<int32>(0xffffffff), static_cast<int32>(0xffffffff), static_cast<int32>(0xffffffff) }; // ~0,~0,~0,~0
const int32 ALIGN16 g_SIMD_Low16BitsMask[4]= { 0xffff, 0xffff, 0xffff, 0xffff }; // 0xffff x 4


const int32 ALIGN16 g_SIMD_ComponentMask[4][4] ALIGN16_POST =
{
	{ static_cast<int32>(0xFFFFFFFF), 0, 0, 0 }, { 0, static_cast<int32>(0xFFFFFFFF), 0, 0 }, { 0, 0, static_cast<int32>(0xFFFFFFFF), 0 }, { 0, 0, 0, static_cast<int32>(0xFFFFFFFF) }
};

const fltx4 g_SIMD_Identity[4] =
{
	{ 1.0, 0, 0, 0 }, { 0, 1.0, 0, 0 }, { 0, 0, 1.0, 0 }, { 0, 0, 0, 1.0 }
};

const int32 ALIGN16 g_SIMD_SkipTailMask[4][4] ALIGN16_POST =
{
	{ static_cast<int32>(0xffffffff), static_cast<int32>(0xffffffff), static_cast<int32>(0xffffffff), static_cast<int32>(0xffffffff) },
	{ static_cast<int32>(0xffffffff), 0x00000000, 0x00000000, 0x00000000 },
	{ static_cast<int32>(0xffffffff), static_cast<int32>(0xffffffff), 0x00000000, 0x00000000 },
	{ static_cast<int32>(0xffffffff), static_cast<int32>(0xffffffff), static_cast<int32>(0xffffffff), 0x00000000 },
};

const int32 ALIGN16 g_SIMD_EveryOtherMask[4] = { 0, ~0, 0, ~0 };





	// FUNCTIONS
	// NOTE: WHY YOU **DO NOT** WANT TO PUT FUNCTIONS HERE
// Generally speaking, you want to make sure SIMD math functions
// are inlined, because that gives the compiler much more latitude
// in instruction scheduling. It's not that the overhead of calling
// the function is particularly great; rather, many of the SIMD 
// opcodes have long latencies, and if you have a sequence of 
// several dependent ones inside a function call, the latencies 
// stack up to create a big penalty. If the function is inlined,
// the compiler can interleave its operations with ones from the
// caller to better hide those latencies. Finally, on the 360,
// putting parameters or return values on the stack, and then 
// reading them back within the next forty cycles, is a very 
// severe penalty. So, as much as possible, you want to leave your
// data on the registers.

// That said, there are certain occasions where it is appropriate
// to call into functions -- particularly for very large blocks
// of code that will spill most of the registers anyway. Unless your
// function is more than one screen long, yours is probably not one
// of those occasions.


/// You can use this to rotate a long array of FourVectors all by the same
/// matrix. The first parameter is the head of the array. The second is the
/// number of vectors to rotate. The third is the matrix.
void FourVectors::RotateManyBy(FourVectors * RESTRICT pVectors, unsigned int numVectors, const matrix3x4_t& rotationMatrix )
{
	Assert(numVectors > 0);
	if ( numVectors == 0 )
		return;

	// Splat out each of the entries in the matrix to a fltx4. Do this
	// in the order that we will need them, to hide latency. I'm
	// avoiding making an array of them, so that they'll remain in 
	// registers.
	fltx4 matSplat00, matSplat01, matSplat02,
		matSplat10, matSplat11, matSplat12,
		matSplat20, matSplat21, matSplat22;

	{
		// Load the matrix into local vectors. Sadly, matrix3x4_ts are 
		// often unaligned. The w components will be the tranpose row of
		// the matrix, but we don't really care about that.
		fltx4 matCol0 = LoadUnalignedSIMD(rotationMatrix[0]);
		fltx4 matCol1 = LoadUnalignedSIMD(rotationMatrix[1]);
		fltx4 matCol2 = LoadUnalignedSIMD(rotationMatrix[2]);

		matSplat00 = SplatXSIMD(matCol0);
		matSplat01 = SplatYSIMD(matCol0);
		matSplat02 = SplatZSIMD(matCol0);

		matSplat10 = SplatXSIMD(matCol1);
		matSplat11 = SplatYSIMD(matCol1);
		matSplat12 = SplatZSIMD(matCol1);

		matSplat20 = SplatXSIMD(matCol2);
		matSplat21 = SplatYSIMD(matCol2);
		matSplat22 = SplatZSIMD(matCol2);
	}

	// PC does not benefit from the unroll/scheduling above
	fltx4 outX0, outY0, outZ0; // bank one of outputs


	// Because of instruction latencies and scheduling, it's actually faster to use adds and muls
	// rather than madds. (Empirically determined by timing.)
	const FourVectors * stop = pVectors + numVectors;

	// perform an even number of iterations through this loop.
	while (pVectors < stop)
	{
		outX0 = MaddSIMD( pVectors->z, matSplat02, AddSIMD( MulSIMD( pVectors->x, matSplat00 ), MulSIMD( pVectors->y, matSplat01 ) ) );
		outY0 = MaddSIMD( pVectors->z, matSplat12, AddSIMD( MulSIMD( pVectors->x, matSplat10 ), MulSIMD( pVectors->y, matSplat11 ) ) );
		outZ0 = MaddSIMD( pVectors->z, matSplat22, AddSIMD( MulSIMD( pVectors->x, matSplat20 ), MulSIMD( pVectors->y, matSplat21 ) ) );

		pVectors->x = outX0;
		pVectors->y = outY0;
		pVectors->z = outZ0;
		pVectors++;
	}
}

// Get the closest point from P to the (infinite) line through vLineA and vLineB and
// calculate the shortest distance from P to the line.
// If you pass in a value for t, it will tell you the t for (A + (B-A)t) to get the closest point.
// If the closest point lies on the segment between A and B, then 0 <= t <= 1.
void FourVectors::CalcClosestPointOnLineSIMD( const FourVectors &P, const FourVectors &vLineA, const FourVectors &vLineB, FourVectors &vClosest, fltx4 *outT)
{
	FourVectors vDir;
	fltx4 t = CalcClosestPointToLineTSIMD( P, vLineA, vLineB, vDir );
	if ( outT ) *outT = t;
	vClosest =  vDir;
	vClosest *= t;
	vClosest += vLineA;
}

fltx4 FourVectors::CalcClosestPointToLineTSIMD( const FourVectors &P, const FourVectors &vLineA, const FourVectors &vLineB, FourVectors &vDir )
{
	Assert( s_bMathlibInitialized );
	vDir = vLineB;
	vDir -= vLineA;

	fltx4 div = vDir * vDir;
	bi32x4 Mask;
	fltx4 Compare = ReplicateX4( 0.00001f );
	fltx4 result;
	Mask = CmpLtSIMD( div, Compare );

	result = DivSIMD( SubSIMD( vDir * P, vDir * vLineA ), div );

	MaskedAssign( Mask, Four_Zeros, result );
	return result;
}

void FourVectors::RotateManyBy(FourVectors * RESTRICT pVectors, unsigned int numVectors, const matrix3x4_t& rotationMatrix, FourVectors * RESTRICT pOut )
{
	Assert(numVectors > 0);
	if ( numVectors == 0 )
		return;

	// Splat out each of the entries in the matrix to a fltx4. Do this
	// in the order that we will need them, to hide latency. I'm
	// avoiding making an array of them, so that they'll remain in 
	// registers.
	fltx4 matSplat00, matSplat01, matSplat02,
		matSplat10, matSplat11, matSplat12,
		matSplat20, matSplat21, matSplat22;

	{
		// Load the matrix into local vectors. Sadly, matrix3x4_ts are 
		// often unaligned. The w components will be the tranpose row of
		// the matrix, but we don't really care about that.
		fltx4 matCol0 = LoadUnalignedSIMD(rotationMatrix[0]);
		fltx4 matCol1 = LoadUnalignedSIMD(rotationMatrix[1]);
		fltx4 matCol2 = LoadUnalignedSIMD(rotationMatrix[2]);

		matSplat00 = SplatXSIMD(matCol0);
		matSplat01 = SplatYSIMD(matCol0);
		matSplat02 = SplatZSIMD(matCol0);

		matSplat10 = SplatXSIMD(matCol1);
		matSplat11 = SplatYSIMD(matCol1);
		matSplat12 = SplatZSIMD(matCol1);

		matSplat20 = SplatXSIMD(matCol2);
		matSplat21 = SplatYSIMD(matCol2);
		matSplat22 = SplatZSIMD(matCol2);
	}

	// PC does not benefit from the unroll/scheduling above
	fltx4 outX0, outY0, outZ0; // bank one of outputs


	// Because of instruction latencies and scheduling, it's actually faster to use adds and muls
	// rather than madds. (Empirically determined by timing.)
	const FourVectors * stop = pVectors + numVectors;

	// perform an even number of iterations through this loop.
	while (pVectors < stop)
	{
		outX0 = MaddSIMD( pVectors->z, matSplat02, AddSIMD( MulSIMD( pVectors->x, matSplat00 ), MulSIMD( pVectors->y, matSplat01 ) ) );
		outY0 = MaddSIMD( pVectors->z, matSplat12, AddSIMD( MulSIMD( pVectors->x, matSplat10 ), MulSIMD( pVectors->y, matSplat11 ) ) );
		outZ0 = MaddSIMD( pVectors->z, matSplat22, AddSIMD( MulSIMD( pVectors->x, matSplat20 ), MulSIMD( pVectors->y, matSplat21 ) ) );

		pOut->x = outX0;
		pOut->y = outY0;
		pOut->z = outZ0;
		pVectors++;
		pOut++;
	}
}


// Transform many (horizontal) points in-place by a 3x4 matrix,
// here already loaded onto three fltx4 registers but not transposed. 
// The points must be stored as 16-byte aligned. They are points
// and not vectors because we assume the w-component to be 1. 



