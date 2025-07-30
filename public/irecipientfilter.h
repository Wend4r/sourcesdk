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
	virtual uint64 GetRecipients() const = 0;

	int	GetRecipientCount() const {
		uint64 x = GetRecipients();
#if defined(__GNUC__) || defined(__clang__)
		return __builtin_popcountll(x);
#elif defined(_MSC_VER)
		return static_cast<int>(__popcnt64(x));
#else
#error "Unsupported compiler: popcount not available"
#endif
	}
};

#endif // IRECIPIENTFILTER_H
