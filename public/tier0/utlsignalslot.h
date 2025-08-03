
#ifndef UTLSLOT_H
#define UTLSLOT_H

#ifdef _WIN32
#pragma once
#endif

#include "basetypes.h"
#include "tier0/utlstring.h"
#include "utldelegateimpl.h"
#include "utlvector.h"

class CUtlSlot;

class CUtlSignaller_Base
{
public:
	using Delegate_t = CUtlDelegate< void( CUtlSlot * ) >;

	CUtlSignaller_Base( const Delegate_t &other ) : m_SlotDeletionDelegate( other ) {}
	CUtlSignaller_Base( Delegate_t &&other ) : m_SlotDeletionDelegate( Move( other ) ) {}

private:
	Delegate_t m_SlotDeletionDelegate;
};

class CUtlSlot
{
public:
	using MTElement_t = CUtlVector< CUtlSignaller_Base * >;

	CUtlSlot() : m_ConnectedSignallers( 0, 1 ) {}

private:
	CUtlVectorMT< MTElement_t, CCopyableLock< CThreadFastMutex > > m_ConnectedSignallers;
};

#endif // UTLSLOT_H
