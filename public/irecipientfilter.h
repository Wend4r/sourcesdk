//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef IRECIPIENTFILTER_H
#define IRECIPIENTFILTER_H
#ifdef _WIN32
#pragma once
#endif

#include "eiface.h"
#include "inetchannel.h"

//-----------------------------------------------------------------------------
// Purpose: Generic interface for routing messages to users
//-----------------------------------------------------------------------------
class IRecipientFilter
{
public:
	virtual			~IRecipientFilter() {}

	virtual bool	IsInitMessage() const = 0;
	virtual NetChannelBufType_t	GetNetworkBufType() const = 0;
	virtual const CPlayerBitVec &GetRecipients() const = 0;
	int	GetRecipientCount() const { return GetRecipients().PopulationCount(); }
};

#endif // IRECIPIENTFILTER_H
