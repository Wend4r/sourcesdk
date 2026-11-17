//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Pose tasks the graph registers with the task system
//
//===========================================================================//

#ifndef ANIMPOSETASKS_H
#define ANIMPOSETASKS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlbinaryblock.h"
#include "tier0/bufferstring.h"
#include "tier0/globalsymbol.h"
#include "mathlib/vector.h"
#include "mathlib/vector2d.h"
#include "mathlib/transform.h"
#include "mathlib/piecewisecurve.h"
#include "resourcesystem/resourcehandle.h"
#include "schemasystem/schematypes.h"
#include "resourcefile/resourcetype.h"
#include "particles/iparticlesystemmgr.h"
#include "animlib/graph/animgraph_valuetypes.h"
#include "animlib/animlib_types.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

#endif // ANIMPOSETASKS_H
