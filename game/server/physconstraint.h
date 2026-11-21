//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The list of constraint anchors (info_constraint_anchor) the
//          physics constraints look up by name.
//
//=============================================================================//

#ifndef PHYSCONSTRAINT_H
#define PHYSCONSTRAINT_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ehandle.h"
#include "iattachmentoverride.h"

#include <mathlib/vector.h>
#include <tier0/utlsymbollarge.h>
#include <tier0/utlvector.h>

class CBaseEntity;

//-----------------------------------------------------------------------------
// One anchor (40 bytes).
//-----------------------------------------------------------------------------
struct constraint_anchor_t
{
	// The anchor's origin in the space of its parent
	Vector localOrigin;
	// The anchor's parent entity, invalid when it has none
	CHandle< CBaseEntity > hEntity;
	// A byte: the attachment the anchor is parented to
	AttachmentHandle_t parentAttachment;
	// The anchor entity's name
	CUtlSymbolLarge name;
	// CInfoConstraintAnchor::m_massScale
	float massScale;
};

//-----------------------------------------------------------------------------
// Registered as "CAnchorList" (static factory). It adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CAnchorList : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	CAnchorList();
	virtual ~CAnchorList();

	// Purges m_list
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// CInfoConstraintAnchor appends itself on spawn; the constraints search the
	// list from the end by name
	CUtlVector< constraint_anchor_t > m_list;
};

#endif // PHYSCONSTRAINT_H
