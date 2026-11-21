//===========================================================================//
//
// Purpose: The global lists that the modules share through tier0
//
//===========================================================================//

#ifndef TIER0_GLOBALLIST_H
#define TIER0_GLOBALLIST_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/threadtools.h"

// A link of an entry of a global list
struct GlobalListEntryBase_t
{
	GlobalListEntryBase_t *m_pA;
	GlobalListEntryBase_t *m_pB;
};

// The entries of the global list with one hash, from Plat_GetGlobalListRegistry
struct GlobalListTypelessRegistry_t
{
	CThreadRWLock_FastRead listRWLock;
	GlobalListEntryBase_t *pListHead;
	GlobalListEntryBase_t *pListTail;
};

#endif // TIER0_GLOBALLIST_H
