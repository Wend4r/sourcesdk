//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The lag compensation interface.
//
//=============================================================================//

#ifndef ILAGCOMPENSATIONMANAGER_H
#define ILAGCOMPENSATIONMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "mathlib/vector.h"

class CBaseEntity;
class CBasePlayerPawn;

// The object that decides which entities a lag compensation moves (polymorphic,
// four methods: whether an entity is compensated, the target
// time, ...). CDefaultLagCompensation implements it for the overloads that take
// a weapon position.
class ILagCompensation;

enum LagCompensationType
{
	LAG_COMPENSATE_BOUNDS,
	LAG_COMPENSATE_HITBOXES,
	LAG_COMPENSATE_HITBOXES_ALONG_RAY,
};

// Named by what it holds: a tick and the fraction into it (the code adds them as
// tick + fraction); 8 bytes
struct LagCompensationTargetTime_t
{
	int m_nTick;
	float m_flFraction;
};

// Named by what StartLagCompensation does with it: the parameters of one
// compensation, built by the overloads; 24 bytes
struct LagCompensationParams_t
{
	ILagCompensation *m_pLagCompensation;
	LagCompensationType m_eType;
	// Shifts the target time (seconds); -1 by default
	float m_flTimeOffset;
	// The unlag limit; 0 or less uses the convar (sv_maxunlag)
	float m_flMaxUnlag;
};

//-----------------------------------------------------------------------------
// StartLagCompensation, FinishLagCompensation, AddAdditionalEntity, RemoveAdditionalEntity,
// IsCurrentlyDoingLagCompensation and the methods CS2 added, 11 in all. The added ones are named
// by what they do. The StartLagCompensation and GetTargetTime overload sets are declared in the
// Linux order; MSVC lays each set out in reverse at the place of its first declaration, which
// matches the Windows table.
//-----------------------------------------------------------------------------
abstract_class ILagCompensationManager
{
public:
	// Called during player movement to set up/restore after lag compensation. Compensates to
	// the player's target time with a default ILagCompensation
	virtual void StartLagCompensation( CBasePlayerPawn *pPlayer, LagCompensationType lagCompensationType, const Vector &weaponPos, const QAngle &weaponAngles, float weaponRange ) = 0;
	// The same with an explicit target time (zero: the player's own) and a flag kept by the
	// default ILagCompensation; fails fatally without a user command
	virtual void StartLagCompensation( CBasePlayerPawn *pPlayer, LagCompensationType lagCompensationType, const Vector &weaponPos, const QAngle &weaponAngles, float weaponRange, const LagCompensationTargetTime_t &targetTime, bool bCompensationFlag ) = 0;
	// Compensates with the caller's ILagCompensation (no time offset, default unlag limit)
	virtual void StartLagCompensation( CBasePlayerPawn *pPlayer, LagCompensationType lagCompensationType, ILagCompensation *pLagCompensation ) = 0;
	// The implementation the other overloads end in
	virtual void StartLagCompensation( CBasePlayerPawn *pPlayer, const LagCompensationParams_t &params ) = 0;
	// Restores the moved entities
	virtual void FinishLagCompensation( CBasePlayerPawn *pPlayer ) = 0;
	// Named by what they do: the time the player's commands are compensated to (from the net
	// channel latency, limited to 200 ms from the requested time)
	virtual LagCompensationTargetTime_t GetTargetTime( CBasePlayerPawn *pPlayer ) = 0;
	virtual LagCompensationTargetTime_t GetTargetTime( CBasePlayerPawn *pPlayer, const LagCompensationTargetTime_t &requestedTime ) = 0;
	// Mappers can flag certain additional entities to lag compensate, this handles them
	virtual void AddAdditionalEntity( CBaseEntity *pEntity ) = 0;
	virtual void RemoveAdditionalEntity( CBaseEntity *pEntity ) = 0;
	virtual bool IsCurrentlyDoingLagCompensation() const = 0;
	// Named by what it does: the target time of the running compensation, zero outside of one
	virtual LagCompensationTargetTime_t GetCurrentTargetTime() const = 0;
};

#endif // ILAGCOMPENSATIONMANAGER_H
