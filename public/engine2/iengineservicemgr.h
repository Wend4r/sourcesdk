#ifndef ENGINE2_IENGINESERVICEMGR_H
#define ENGINE2_IENGINESERVICEMGR_H

#ifdef _WIN32
#pragma once
#endif

#include "tier4/tier4.h"

enum EngineServiceActivateType_t
{
	ENGINE_SERVICE_ACTIVATE_NEVER = 0,
	ENGINE_SERVICE_ACTIVATE_ALWAYS,
	ENGINE_SERVICE_ACTIVATE_IF_ADDED_BY_LOOP_MODE,
};

// AMNOTE: Layout from the CGameResourceService constructor and destructor (libengine2.so sub_3D9390, engine2.dll
// sub_1801ADB90) and the IEngineService accessors; names of the first three fields from s&box (libengine2.so DWARF:
// m_pName, m_bIsActive, m_nServiceIndex). The derived class data starts at 56 on Linux and at 64 on Windows: the
// 8-bit uint32 bitfield after m_bIsServerInstance gets its own 4-byte unit at 56 under MSVC, while g++ packs it
// into byte 54.
template< class IInterface >
class CBaseEngineService : public CTier4AppSystem< IInterface >
{
public:
	const char *m_pszName;
	bool m_bActive;
	uint16 m_nServiceIndex;
	// AMNOTE: Zeroed by the constructor; no reader found in engine2 on either platform, so it has no name source
	uint8 m_nReservedByte;
	// Constructor argument
	bool m_bIsServerInstance;
	// AMNOTE: Zeroed by the constructor; no reader found in engine2, no name source (see the class note for its
	// place)
	uint32 m_nReservedFlags : 8;
};

#endif // ENGINE2_IENGINESERVICEMGR_H
