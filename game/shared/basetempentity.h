#ifndef BASETEMPENTITY_H
#define BASETEMPENTITY_H

#pragma once

#include "tier0/platform.h"
#include "tier0/utlsignalslot.h"

class IRecipientFilter;

//-----------------------------------------------------------------------------
// The base of the temp entities (CTEBeamPoints, CTEEffectDispatch, CTEFireBullets, ...): a
// static object per kind, linked into one list by its constructor. 56 bytes on both platforms.
// Windows slots 0-3; on Linux the destructor takes two slots, so every slot after it is one
// further. CTEExplosion and CTEImpact add methods (te/te_explosion.h, te/te_impact.h); the
// other temp entities only override. Each derived class holds the network message it sends.
//-----------------------------------------------------------------------------
abstract_class CBaseTempEntity : public CUtlSlot
{
public:
	virtual ~CBaseTempEntity() = 0;

	virtual void Create( IRecipientFilter &filter, float delay = 0.0f ) = 0;

	// Both empty; CTEExplosion and CTEImpact override StaticInit with empty bodies
	virtual void StaticInit() = 0;
	virtual void StaticShutdown() = 0;

public:
	// The next temp entity of the list the constructors build (the previous head of the list)
	CBaseTempEntity *m_pNext;
};

#endif // BASETEMPENTITY_H
