#ifndef SOUND_EVENT_ENTITY_H
#define SOUND_EVENT_ENTITY_H

#pragma once

#include "baseentity.h"

struct SoundEventGuid_t;

//-----------------------------------------------------------------------------
// CSoundEventEntity adds 2 methods to CBaseEntity: Linux slots 239-240, Windows 238-239.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CSoundEventEntityVTable : public TBase
{
public:
	// Named by what it does
	virtual void StartSound() = 0;
	// Starts the sound event (position/attachment params) and schedules CSoundEventEntity::SoundFinishedThink. Named by what it does
	virtual void StartSoundWithGuid( SoundEventGuid_t *pGuid ) = 0;
};

//-----------------------------------------------------------------------------
// CSoundEventMultiPointEntity adds 2 methods to CSoundEventEntity: Linux slots 241-242, Windows 240-241.
//-----------------------------------------------------------------------------
template < class TBase = CSoundEventEntityVTable<> >
abstract_class CSoundEventMultiPointEntityVTable : public TBase
{
public:
	// Clears m_bPlaying. Named by what it does
	virtual void StopPlaying() = 0;
	// Pure; the box / path corner implementations walk their points (+2296 boxes, +2176 path corners), take the closest point of each segment/box to the given position and collect them. Named by what it does
	virtual void FindClosestPoints( void *pUnknown, const Vector &vecListener, CUtlVector< Vector > *pPoints ) = 0;
};

#endif // SOUND_EVENT_ENTITY_H
