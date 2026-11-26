//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The curve data of choreo samples.
//
//=============================================================================//

#ifndef EXPRESSIONSAMPLE_H
#define EXPRESSIONSAMPLE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"

class CChoreoEvent;

//-----------------------------------------------------------------------------
// What the curve evaluation of a choreo sample asks its owner (CFlexAnimationTrack, CTempCurveDataAccessor).
//-----------------------------------------------------------------------------
abstract_class ICurveDataAccessor
{
public:
	virtual float GetDuration() = 0;
	virtual bool CurveHasEndTime() = 0;
	virtual int GetDefaultCurveType() = 0;
};

//-----------------------------------------------------------------------------
// How a curve behaves before its first / after its last sample. 8 bytes.
//-----------------------------------------------------------------------------
struct EdgeInfo_t
{
	// "zero_value"
	float m_flZeroPos;
	// "type"
	unsigned short m_CurveType;
	bool m_bActive;
};

//-----------------------------------------------------------------------------
// One sample of a curve. 12 bytes.
//-----------------------------------------------------------------------------
struct CExpressionSample
{
	// "time"
	float time;
	// "value"
	float value;
	// "curvetype"
	unsigned short m_curvetype;
	// Named by what it holds: index of the sample's tangents in CCurveData::m_pBezierData
	// (0x7FFF for none)
	unsigned short m_nBezierIndex : 15;
	unsigned short selected : 1;
};

//-----------------------------------------------------------------------------
// "bezier": the tangents of a sample. Named by what it holds. 20 bytes.
//-----------------------------------------------------------------------------
struct ExpressionSampleBezier_t
{
	// "in": "deg", "weight"
	float m_flInDegrees;
	float m_flInWeight;
	// "out": "deg", "weight"
	float m_flOutDegrees;
	float m_flOutWeight;
	// "unified"
	bool m_bUnified;
	// "unweighted"
	bool m_bUnweighted;
};

//-----------------------------------------------------------------------------
// A curve made of samples, evaluated for its owner. 56 bytes.
//-----------------------------------------------------------------------------
class CCurveData
{
public:
	// Named by what it holds: the owner the curve asks for its duration and default curve type
	ICurveDataAccessor *m_pAccessor;
	CUtlVector< CExpressionSample > m_Ramp;
	// [ 0 ] the left edge, [ 1 ] the right edge
	EdgeInfo_t m_RampEdgeInfo[ 2 ];
	// Named by what it holds: the tangents of the samples that have them, allocated on demand
	CUtlVector< ExpressionSampleBezier_t > *m_pBezierData;
};

//-----------------------------------------------------------------------------
// A flex controller track of a CChoreoEvent_FLEXANIMATION event ("flexanimations"). 168 bytes.
//-----------------------------------------------------------------------------
abstract_class CFlexAnimationTrack : public ICurveDataAccessor
{
public:
	virtual ~CFlexAnimationTrack() = 0;

public:
	// "name": the flex controller
	CUtlString m_ControllerName;
	CChoreoEvent *m_pEvent;
	// "min" / "max": the range of the track (0 / 1 by default)
	float m_flMin;
	float m_flMax;
	// "samples" ([ 0 ], the magnitude) and "stereo" ([ 1 ], the left / right balance of a combo
	// track)
	CCurveData m_Samples[ 2 ];
	// -1 until looked up (see CChoreoEvent::m_bTrackLookupSet)
	int m_nFlexControllerIndex[ 2 ];
	int m_nFlexControllerIndexRaw[ 2 ];
	// Cleared for "disabled"
	bool m_bActive : 1;
	// "combo"
	bool m_bCombo : 1;
	// Copied with the track
	bool m_bServerSide : 1;
};

#endif // EXPRESSIONSAMPLE_H
