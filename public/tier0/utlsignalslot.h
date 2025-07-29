
#ifndef UTLSLOT_H
#define UTLSLOT_H

#ifdef _WIN32
#pragma once
#endif

#include "basetypes.h"
#include "tier0/utlstring.h"
#include "utldelegateimpl.h"
#include "utlvector.h"

struct CUtlSlot;

struct CUtlSignaller_Base
{
	CUtlDelegate< void ( CUtlSlot* ) > m_SlotDeletionDelegate;
};

struct CUtlSlot
{
	CUtlVectorMT< CUtlVector<CUtlSignaller_Base*>, CCopyableLock<CThreadFastMutex> > m_ConnectedSignallers;
};

#endif // UTLSLOT_H
