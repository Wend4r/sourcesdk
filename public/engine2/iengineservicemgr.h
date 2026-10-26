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

enum EngineServiceDependencyFlags_t : uint32
{
	ESD_FORCE_INCLUSION = ( 1 << 0 ),
	ESD_MUST_RUN_AFTER = ( 1 << 1 ),
	ESD_MUST_RUN_BEFORE = ( 1 << 2 ),
};

// Arrays of these end with an entry whose m_pServiceName is NULL
struct EngineServiceDependencies_t
{
	const char *m_pServiceName;
	uint32 m_nFlags; // EngineServiceDependencyFlags_t
};

template< class IInterface >
class CBaseEngineService : public CTier4AppSystem<IInterface> {

public:
	const char *m_pszName;
	bool m_bActive;
	uint16 m_nServiceIndex;
};

#endif // ENGINE2_IENGINESERVICEMGR_H
