#ifndef TIER0_CONVARREF_H
#define TIER0_CONVARREF_H

#ifdef _WIN32
#pragma once
#endif

#include "schemasystem/schematypes.h"

//-----------------------------------------------------------------
// Used to read/write/create convars (replaces the FindVar method)
//-----------------------------------------------------------------
class ConVarRef
{
private:
	static const uint16 kInvalidAccessIndex = 0xFFFFu;
	static const uint16 kInvalidCommandIndex = 0xFFFFu;

public:
	ConVarRef( uint16 convar_idx = kInvalidAccessIndex, uint16 concom_idx = kInvalidCommandIndex, uint32 registered_idx = 0u ) : m_ConVar{ convar_idx, concom_idx, registered_idx } {}
	ConVarRef( uint64 handle ) : m_Handle( handle ) {}
	ConVarRef( const char *name, bool allow_defensive = false );

	void InvalidateRef() { m_ConVar.m_iAccessIndex = kInvalidAccessIndex; m_ConVar.m_iRegisteredIndex = 0; }
	bool IsValidRef() const { return m_ConVar.m_iAccessIndex != kInvalidAccessIndex; }
	uint16 GetAccessIndex() const { return m_ConVar.m_iAccessIndex; }
	uint32 GetRegisteredIndex() const { return m_ConVar.m_iRegisteredIndex; }
	explicit operator bool() const { return m_ConVar.m_iAccessIndex != kInvalidAccessIndex; }
	operator uint64() const { return m_Handle; }

protected:
	struct Handle_t
	{
		// Index into internal linked list of convars
		uint16 m_iAccessIndex;

		// Index into internal linked list of concommands
		uint16 m_iCommandIndex;

		// ConVar registered positional index
		uint32 m_iRegisteredIndex;
	};

	union
	{
		uint64 m_Handle;
		Handle_t m_ConVar;
	};
};

#endif // TIER0_CONVARREF_H
