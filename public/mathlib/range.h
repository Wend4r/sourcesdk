//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Ranges of two values, edited as a pair in the tools
//
//=============================================================================//

#ifndef RANGE_H
#define RANGE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

#include "schemacompiler/schemaon.h"

//-----------------------------------------------------------------------------
// Schema classes of the tier2 project (both are registered by every game module
// that links tier2). The game builds a range from one value by storing it twice;
// the two elements are the low and the high end.
//-----------------------------------------------------------------------------
schema class CRangeInt
{
public:
	TYPEMETA( MPropertyCustomEditor = "multi_int(2)" );

	int32 m_pValue[ 2 ];
};

schema class CRangeFloat
{
public:
	TYPEMETA( MPropertyCustomEditor = "multi_float(2)" );

	float32 m_pValue[ 2 ];
};

#include "schemacompiler/schemaoff.h"

#endif // RANGE_H
