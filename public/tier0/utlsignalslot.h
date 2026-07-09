
#ifndef UTLSLOT_H
#define UTLSLOT_H

#ifdef _WIN32
#pragma once
#endif

#include "basetypes.h"
#include "tier0/threadtools.h"
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
	using MTElement_t = CUtlSignaller_Base *;

	CUtlSlot() {}

private:
	CThreadFastMutex m_Mutex;
	CUtlVector< MTElement_t > m_ConnectedSignallers;
};

static_assert( sizeof( CUtlSlot ) == 40, "CUtlSlot sizeof mismatch" );

#endif // UTLSLOT_H
