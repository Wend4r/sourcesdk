#ifndef CSTRIKE15_CS_CHICKENGRAPHCONTROLLER_H
#define CSTRIKE15_CS_CHICKENGRAPHCONTROLLER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/globalsymbol.h"
#include "mathlib/vector.h"
#include "../animgraphcontroller.h"

#include "schemacompiler/schemaon.h"

//-----------------------------------------------------------------------------
// Drives the graph of a chicken: its action, mode, life stage, idle, turn, look-at and flinch
// parameters. It overrides the virtual methods of its base and adds none.
//
// The game implements the virtual methods; the objects are only reached through pointers.
//-----------------------------------------------------------------------------
schema class CCS2ChickenGraphController : public CAnimGraphControllerBase
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_action;
	CAnimGraph2ParamAutoResetOptionalRef m_bActionReset;
	CAnimGraph2ParamOptionalRef< float32 > m_actionVariation;
	CAnimGraph2ParamOptionalRef< bool > m_bInWater;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_mode;
	CAnimGraph2ParamOptionalRef< CGlobalSymbol > m_lifeStage;
	CAnimGraph2ParamOptionalRef< float32 > m_idlePhase;
	CAnimGraph2ParamOptionalRef< float32 > m_turnAngle;
	CAnimGraph2ParamOptionalRef< bool > m_bHasLookatTarget;
	CAnimGraph2ParamOptionalRef< Vector > m_lookatTarget;
	CAnimGraph2ParamOptionalRef< bool > m_bFlinch;
	CAnimGraph2ParamOptionalRef< float32 > m_flinchVariation;
	bool m_bHasActionCompletedEvent;
};

#include "schemacompiler/schemaoff.h"

#endif // CSTRIKE15_CS_CHICKENGRAPHCONTROLLER_H
