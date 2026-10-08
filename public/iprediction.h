//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Engine interface into the client side prediction system
//
//===========================================================================//

#ifndef IPREDICTION_H
#define IPREDICTION_H

#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "game/client/prediction.h"
#include "mathlib/vector.h"
#include "playerslot.h"
#include "splitscreenslot.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
abstract_class IPrediction : public IAppSystem
{
public:
	// Runs a full prediction pass
	virtual void Update( PredictionReason_t nReason ) = 0;

	virtual void NetUpdatePreStart() = 0;
	virtual void NetUpdateStart() = 0;
	virtual void PostEntityPacketReceived() = 0;
	virtual void PostNetworkDataReceived() = 0;
	// Resets the prediction state after a full update
	virtual void OnReceivedUncompressedPacket() = 0;

	virtual void unk017( int nSimulationTick, float flUnk, int *pUnk ) = 0;
	virtual float unk018() = 0;
	virtual void unk019( int nUnk, float flUnk1, float flUnk2 ) = 0;
	virtual int unk020() = 0;

	virtual void unk021( CPlayerSlot nSlot, Vector &vecOrigin ) = 0;
	virtual void unk022( CPlayerSlot nSlot, QAngle &angRotation ) = 0;

	virtual void GetViewAngles( CPlayerSlot nSlot, QAngle &ang ) = 0;
	virtual void SetViewAngles( CPlayerSlot nSlot, const QAngle &ang ) = 0;
	virtual void SetLocalViewAngles( CSplitScreenSlot nSlot, const QAngle &ang ) = 0;

	// Convar-backed gate for the engine's prediction reason spew
	virtual bool unk026() = 0;
	// True when a prediction slot with this id is tracked
	virtual bool unk027( int nSlot ) = 0;
	// True while TrueView is active
	virtual bool unk028() = 0;
	// True when the last TrueView offset could not be determined
	virtual bool unk029() = 0;
	virtual int unk030() = 0;
	virtual void unk031() = 0;
};

#endif // IPREDICTION_H
