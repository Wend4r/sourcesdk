//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The countdown and interval timers the bot embeds by value.
//
//=============================================================================//

#ifndef CS_BOT_TIMERS_H
#define CS_BOT_TIMERS_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "entitytypes.h"
#include "spawngrouptypes.h"
#include "schemasystem/schematypes.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;

//-----------------------------------------------------------------------------
// Both timers are embedded schema classes with the network state change virtuals of an
// embedded network variable. None of the virtuals changes the timer. A timestamp of -1 means
// the timer is not running; the times are the world group's game time, which is
// gpGlobals->curtime for the main world.
//
// The two classes declare the same virtuals in a different order, and only CountdownTimer
// is networked. The network variable wrapper of a networked CountdownTimer member overrides
// NetworkStateChanged and NetworkStateChangedLog to forward them to the owning entity.
//
// The virtuals have bodies because the bot embeds the timers by value; the game's own
// vtable is the one in place on every timer it constructs.
//-----------------------------------------------------------------------------
class CountdownTimer
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() { return {}; }
	// Save and load m_duration, m_timestamp, m_timescale and m_nWorldGroupId.
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const {}
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) {}
	virtual const char *GetClassName() const { return "CountdownTimer"; }

	// Empty unless the timer is a networked member.
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data ) {}
	// Empty and not overridden by the network variable wrapper.
	virtual void NetworkStateChangedOld( uint32 nLocalOffset, int32 nArrayIndex, int32 nPathIndex ) {}
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo ) const {}

	// The "CountdownTimer" serializer registered with the network serializer class dictionary.
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const { return nullptr; }

	bool HasStarted() const { return m_timestamp.GetTime() > 0.0f; }
	bool IsElapsed( float flNow ) const { return flNow > m_timestamp.GetTime(); }
	float GetRemainingTime( float flNow ) const { return m_timestamp.GetTime() - flNow; }
	float GetCountdownDuration() const { return m_timestamp.GetTime() > 0.0f ? m_duration : 0.0f; }

public:
	float m_duration;
	// When the countdown ends.
	GameTime_t m_timestamp;
	// 1 unless changed.
	float m_timescale;
	WorldGroupId_t m_nWorldGroupId;
};

class IntervalTimer
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() { return {}; }

	// Empty: IntervalTimer is never a networked member. The order within the three follows
	// CountdownTimer's.
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data ) {}
	virtual void NetworkStateChangedOld( uint32 nLocalOffset, int32 nArrayIndex, int32 nPathIndex ) {}
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo ) const {}

	// Save and load m_timestamp and m_nWorldGroupId.
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const {}
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) {}
	virtual const char *GetClassName() const { return "IntervalTimer"; }

	bool HasStarted() const { return m_timestamp.GetTime() > 0.0f; }
	float GetElapsedTime( float flNow ) const { return HasStarted() ? flNow - m_timestamp.GetTime() : 99999.9f; }
	bool IsLessThan( float flNow, float flDuration ) const { return flNow - m_timestamp.GetTime() < flDuration; }
	bool IsGreaterThan( float flNow, float flDuration ) const { return flNow - m_timestamp.GetTime() > flDuration; }

public:
	// When the interval started.
	GameTime_t m_timestamp;
	WorldGroupId_t m_nWorldGroupId;
};

#endif // CS_BOT_TIMERS_H
