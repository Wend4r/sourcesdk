//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef FRAME_H
#define FRAME_H

#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Frame variables used by shared code
//-----------------------------------------------------------------------------
struct CFrame
{
	float starttime_stddeviation;
	float time_stddeviation;
	float time_computationduration;
	float time_unbounded;
};

#endif // FRAME_H
