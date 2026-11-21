//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef EDICT_H
#define EDICT_H

#ifdef _WIN32
#pragma once
#endif

#include "mathlib/vector.h"
#include "cmodel.h"
#include "const.h"
#include "globalvars_base.h"
#include "iservernetworkable.h"
#include "tier0/bitvec.h"
#include "tier0/convar.h"
#include "engine2/gameshareddefs.h"



inline CGlobalVars::CGlobalVars() : 
	CGlobalVarsBase()
{
	serverCount = 0;
}


// Remembers that the entity is carrying a "full change list" - all its properties
// might have changed their value.
#define FL_FULL_EDICT_CHANGED		(1<<1)

#define FL_EDICT_ALWAYS		(1<<2)	// always transmit this entity
#define FL_EDICT_DONTSEND	(1<<3)	// don't transmit this entity
#define FL_EDICT_PVSCHECK	(1<<4)	// always transmit entity, but cull against PVS


// Shared between engine and game DLL (IVEngineServer2::GetSharedEdictChangeInfo).
class CSharedEdictChangeInfo;

#endif // EDICT_H
