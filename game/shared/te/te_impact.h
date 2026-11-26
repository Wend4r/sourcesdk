#ifndef TE_IMPACT_H
#define TE_IMPACT_H

#pragma once

#include "../basetempentity.h"
#include "../te_messages.h"
#include "mathlib/vector.h"

class CGameTrace;

//-----------------------------------------------------------------------------
// The impact temp entity, 152 bytes on both platforms. It adds two methods to
// CBaseTempEntity: Windows slots 4-5, Linux 5-6. See basetempentity.h.
//-----------------------------------------------------------------------------
abstract_class CTEImpact : public CBaseTempEntity
{
public:
	// Both empty on both platforms
	virtual void PlayImpactSound( CGameTrace &trace ) = 0;
	virtual void PerformCustomEffects( CGameTrace &trace, Vector &vecShotDir ) = 0;

public:
	// The message the temp entity sends; named by what it holds
	TEImpact_GameEvent_t m_Message;
};

#endif // TE_IMPACT_H
