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
	virtual bool IsPredictionDiffMgrActive() const = 0;

	// Drops the lines recorded so far for the command on the given side
	virtual void StartCommand( int nPlayerSlot, bool bServer, int nCommandNumber ) = 0;

	// Appends a line to the record of the command on the given side
	virtual void AddToDiff( int nPlayerSlot, bool bServer, int nCommandNumber, const char *pszText ) = 0;

	// Compares the server and client records of the slot and prints the differences
	virtual void Validate( int nPlayerSlot, bool bServer ) = 0;
};

#endif // IPREDICTIONDIFFMANAGER_H
