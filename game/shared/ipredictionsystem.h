//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Suppression of predicted effects.
//
//=============================================================================//

#ifndef IPREDICTIONSYSTEM_H
#define IPREDICTIONSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CBaseEntity;

//-----------------------------------------------------------------------------
// Suppression of the effects the predicting client already played. No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class IPredictionSuppressEffects
{
public:
	virtual void SetSuppressEvent( bool state ) = 0;
	virtual void SetSuppressHost( CBaseEntity *host ) = 0;

	// Named by what it does: returns what SetSuppressEvent set.
	virtual bool GetSuppressEvent() const = 0;
	virtual const CBaseEntity *GetSuppressHost() = 0;
	virtual void _Push() = 0;
	virtual void _Pop() = 0;
};

#endif // IPREDICTIONSYSTEM_H
