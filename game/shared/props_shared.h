//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The prop data game system. Factory "PropData".
//
//=============================================================================//

#ifndef PROPS_SHARED_H
#define PROPS_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "resourcesystem/ivdataresourcelistener.h"

class KeyValues3;

//-----------------------------------------------------------------------------
// Reallocating game system, 48 bytes on both platforms. libserver.so repeats the
// OnVDataReloaded override at the end of the main table (slot 66).
//-----------------------------------------------------------------------------
class CPropData : public CAutoGameSystem, public IVDataResourceListener
{
public:
	DECLARE_GAME_SYSTEM();

	// Registers this system as the listener of "scripts/propdata.vdata", then takes the
	// file's KeyValues3 root when the resource is loaded
	GS_EVENT( GameInit );
	// Unregisters the listener and clears m_pKVPropData
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// IVDataResourceListener: takes the root again
	virtual void OnVDataReloaded( ResourceHandle_t hResource ) override;

public:
	// Root of the propdata.vdata KeyValues3 context
	KeyValues3 *m_pKVPropData;
	bool m_bPropDataLoaded;
};

#endif // PROPS_SHARED_H
