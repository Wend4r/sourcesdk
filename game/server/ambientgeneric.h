#ifndef AMBIENTGENERIC_H
#define AMBIENTGENERIC_H

#pragma once

#include "baseentity.h"

//-----------------------------------------------------------------------------
// CAmbientGeneric adds 3 methods to CPointEntity (through CPointEntity, which adds no method): Linux slots 239-241, Windows 238-240.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CAmbientGenericVTable : public TBase
{
public:
	virtual void ToggleSound() = 0;
	// Starts the sound event of the entity
	virtual void SendSound() = 0;
	// Stops the playing sound event (guid at +2072); called by the PlaySound/StopSound inputs and ToggleSound. Named by what it does
	virtual void StopSound() = 0;
};

#endif // AMBIENTGENERIC_H
