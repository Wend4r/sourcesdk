#ifndef TRAINS_H
#define TRAINS_H

#pragma once

#include "basetoggle.h"

//-----------------------------------------------------------------------------
// CBasePlatTrain adds 1 method to CBaseToggle: Linux slot 272, Windows 271.
//-----------------------------------------------------------------------------
template < class TBase = CBaseToggleVTable<> >
abstract_class CBasePlatTrainVTable : public TBase
{
public:
	virtual bool IsTogglePlat() = 0;
};

//-----------------------------------------------------------------------------
// CFuncPlat adds 4 methods to CBasePlatTrain: Linux slots 273-276, Windows 272-275.
//-----------------------------------------------------------------------------
template < class TBase = CBasePlatTrainVTable<> >
abstract_class CFuncPlatVTable : public TBase
{
public:
	// TS_GOING_UP, LinearMove( m_vecPosition1 )
	virtual void GoUp() = 0;
	virtual void GoDown() = 0;
	virtual void HitTop() = 0;
	virtual void HitBottom() = 0;
};

//-----------------------------------------------------------------------------
// CFuncTrackChange adds 1 method to CFuncPlatRot (through CFuncPlatRot, which adds no method): Linux slot 277, Windows 276.
//-----------------------------------------------------------------------------
template < class TBase = CFuncPlatVTable<> >
abstract_class CFuncTrackChangeVTable : public TBase
{
public:
	// CFuncTrackAuto overrides
	virtual void UpdateAutoTargets( int toggleState ) = 0;
};

#endif // TRAINS_H
