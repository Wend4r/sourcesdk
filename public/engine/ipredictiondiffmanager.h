//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Prediction diff check
//
//=============================================================================//

#ifndef IPREDICTIONDIFFMANAGER_H
#define IPREDICTIONDIFFMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>

abstract_class IPredictionDiffMgr : public IAppSystem
{
public:
	// Returns the value of the diffcheck convar
	virtual bool IsDiffCheckEnabled() = 0;

	virtual void ClearRecords( int nPlayerSlot, bool bServer, int nKey ) = 0;

	virtual void AddRecord( int nPlayerSlot, bool bServer, int nKey, const char *pszText ) = 0;

	virtual void SpewDiffs( int nPlayerSlot, bool bUnk ) = 0;
};

#endif // IPREDICTIONDIFFMANAGER_H
