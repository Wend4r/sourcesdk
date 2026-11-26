//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The "CS2Bullets" debug overlay window.
//
//=============================================================================//

#ifndef CS_DEBUGOVERLAY_BULLETS_H
#define CS_DEBUGOVERLAY_BULLETS_H
#ifdef _WIN32
#pragma once
#endif

#include "debugoverlay_window.h"

#include <tier0/utlvector.h>

// The element types of the window's vectors are not identified yet: the destructor frees them
// without destructing elements (two vectors), 72-byte elements with a vector at 40, 144-byte
// elements with vectors at 8, 40 and 112, and a last vector of plain elements
struct DebugOverlayBulletSample_t;
struct DebugOverlayBulletImpact_t;
struct DebugOverlayBulletShot_t;
struct DebugOverlayBulletTrace_t;

//-----------------------------------------------------------------------------
// Reallocating factory, 288 bytes on both platforms. DrawOverlay, LoadSettings and SaveSettings
// are empty on the server. The members are
// named by what the constructor and the destructor do with them; no other server code was found
// that reads them.
//-----------------------------------------------------------------------------
class CDebugOverlayBullets : public CDebugOverlayWindow
{
public:
	DECLARE_GAME_SYSTEM();

	CDebugOverlayBullets();
	virtual ~CDebugOverlayBullets();

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// Empty
	virtual void DrawOverlay( void *pDebugOverlay ) override;
	virtual void LoadSettings() override;
	virtual void SaveSettings() override;

	// Records the bullets of the tick for every player pawn; returns at once unless the
	// overlay records
	GS_EVENT( ServerPostSimulate );

public:
	// Cleared by the constructor
	uint64 m_nClearedOnConstruction[ 8 ];
	// 100 after construction
	int m_nMaxSamples;
	// Not set by the constructor
	int m_nNotConstructed;
	// Two ring buffers: a head index, a capacity (16 after construction) and the storage
	int m_nSampleHead;
	int m_nSampleCapacity;
	CUtlVector< DebugOverlayBulletSample_t > m_Samples;
	int m_nTraceHead;
	int m_nTraceCapacity;
	CUtlVector< DebugOverlayBulletTrace_t > m_Traces;
	// -1 after construction
	int m_nSelectedShot;
	// Not set by the constructor nor freed by the destructor
	int m_nNotConstructed2;
	uint64 m_nNotConstructed3[ 3 ];
	CUtlVector< DebugOverlayBulletImpact_t > m_Impacts;
	CUtlVector< DebugOverlayBulletShot_t > m_Shots;
	CUtlVector< int > m_ShotIndices;
};

#endif // CS_DEBUGOVERLAY_BULLETS_H
