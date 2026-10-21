#ifndef TSPOINTERLIST_H
#define TSPOINTERLIST_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/tslist.h"
#include "tier0/mempool.h"

//-----------------------------------------------------------------------------
// Node of a thread-safe list of pointers
//-----------------------------------------------------------------------------
struct TSPointerListNode_t : public TSLNodeBase_t
{
	void *pData;
};

// Pool that the nodes of the pointer lists come from
PLATFORM_INTERFACE CUtlMemoryPoolMT< TSPointerListNode_t > g_TSPointerListNodes;

#endif // TSPOINTERLIST_H
