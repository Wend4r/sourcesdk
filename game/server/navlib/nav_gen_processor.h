//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The stages of the polygon processor of the navigation mesh generation
//          (CNavGenPolyProcessorStage1, CNavGenPolyProcessorStage2).
//
//=============================================================================//

#ifndef NAV_GEN_PROCESSOR_H
#define NAV_GEN_PROCESSOR_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"

//-----------------------------------------------------------------------------
// A named step of the polygon clipping pipeline of the nav mesh generation (the stages wrap a
// std::function member of their own). 24 bytes.
//-----------------------------------------------------------------------------
abstract_class CNavGenPolyProcessorStageBase
{
public:
	virtual ~CNavGenPolyProcessorStageBase() = 0;
	// Runs the stage; pure in the base. The pipeline runs a stage only while m_bEnabled is set
	// and times it
	virtual void Stub_2() = 0;

public:
	// The pipeline skips a disabled stage; initialised from the convar nav_gen_true. Named by
	// what it does
	bool m_bEnabled;
	// The pipeline records the output of the stage for the debug drawing ("(Output drawable)").
	// Named by what it does
	bool m_bOutputDrawable;
	// The stage name the pipeline prints (">>NAVGEN: stage %02d / %02d: ")
	CUtlString m_sName;
};

#endif // NAV_GEN_PROCESSOR_H
