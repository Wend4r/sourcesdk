#ifndef TE_EXPLOSION_H
#define TE_EXPLOSION_H

#pragma once

#include "../basetempentity.h"
#include "../te_messages.h"

class CParticleRenderIterator;

//-----------------------------------------------------------------------------
// The explosion temp entity, 200 bytes on both platforms. It adds one method to
// CBaseTempEntity: Windows slot 4, Linux 5. See basetempentity.h.
//-----------------------------------------------------------------------------
abstract_class CTEExplosion : public CBaseTempEntity
{
public:
	// Empty on both platforms
	virtual void RenderParticles( CParticleRenderIterator *pIterator ) = 0;

public:
	// The message the temp entity sends; named by what it holds
	TEExplosion_GameEvent_t m_Message;
	// Cleared by the constructor; nothing else touches these 8 bytes
	uint64 m_nReserved;
};

#endif // TE_EXPLOSION_H
