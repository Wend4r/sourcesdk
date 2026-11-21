//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The lag compensation manager. Factory "LagCompensationManager".
//
//=============================================================================//

#ifndef PLAYER_LAGCOMPENSATION_H
#define PLAYER_LAGCOMPENSATION_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ilagcompensationmanager.h"
#include "ehandle.h"
#include "tier0/utlcommon.h"
#include "tier1/utlhashmaplarge.h"

//-----------------------------------------------------------------------------
// Static instance, 248 bytes on both platforms. libserver.so repeats the eleven
// ILagCompensationManager overrides at the end of the main table (slots 66..76).
// The weapon range is kept in the ILagCompensation.
//-----------------------------------------------------------------------------
class CLagCompensationManager : public CAutoGameSystem, public ILagCompensationManager
{
public:
	DECLARE_GAME_SYSTEM();

	// Both delete the lag data of m_CompensatedEntities
	virtual void Shutdown() override;
	GS_EVENT( GameShutdown );
	// Records the entities' lag data with more than one player while lag compensation is enabled, else deletes
	// the lag data
	GS_EVENT( ServerPostEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// ILagCompensationManager
	virtual void StartLagCompensation( CBasePlayerPawn *pPlayer, LagCompensationType lagCompensationType, const Vector &weaponPos, const QAngle &weaponAngles, float weaponRange ) override;
	virtual void StartLagCompensation( CBasePlayerPawn *pPlayer, LagCompensationType lagCompensationType, const Vector &weaponPos, const QAngle &weaponAngles, float weaponRange, const LagCompensationTargetTime_t &targetTime, bool bCompensationFlag ) override;
	virtual void StartLagCompensation( CBasePlayerPawn *pPlayer, LagCompensationType lagCompensationType, ILagCompensation *pLagCompensation ) override;
	virtual void StartLagCompensation( CBasePlayerPawn *pPlayer, const LagCompensationParams_t &params ) override;
	virtual void FinishLagCompensation( CBasePlayerPawn *pPlayer ) override;
	virtual LagCompensationTargetTime_t GetTargetTime( CBasePlayerPawn *pPlayer ) override;
	virtual LagCompensationTargetTime_t GetTargetTime( CBasePlayerPawn *pPlayer, const LagCompensationTargetTime_t &requestedTime ) override;
	virtual void AddAdditionalEntity( CBaseEntity *pEntity ) override;
	virtual void RemoveAdditionalEntity( CBaseEntity *pEntity ) override;
	virtual bool IsCurrentlyDoingLagCompensation() const override;
	virtual LagCompensationTargetTime_t GetCurrentTargetTime() const override;

public:
	// The recorded tracks and restore data of one entity; FinishLagCompensation restores the
	// entities whose first byte is set
	struct EntityLagData;

	// Entities with lag data
	CUtlHashMap< CHandle< CBaseEntity >, EntityLagData * > m_CompensatedEntities;
	// The player of the running compensation
	CBasePlayerPawn *m_pCurrentPlayer;
	// Only zeroed by the constructor
	LagCompensationType m_lagCompensationType;
	// The ILagCompensation of the running compensation
	ILagCompensation *m_pCurrentLagCompensation;
	bool m_isCurrentlyDoingCompensation;
	bool m_bNeedToRestore;
	// The entities mappers flagged for compensation (a set)
	CUtlHashMap< CHandle< CBaseEntity >, empty_t > m_AdditionalEntities;
	// Set by StartLagCompensation's weapon overload, not constructed
	Vector m_weaponPos;
	QAngle m_weaponAngles;
};

#endif // PLAYER_LAGCOMPENSATION_H
