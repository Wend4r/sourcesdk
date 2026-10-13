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

template< class IInterface >
class CBaseEngineService : public CTier4AppSystem<IInterface> {

public:
	const char *m_pszName;
	bool m_bActive;
	uint16 m_nServiceIndex;
};

#endif // ENGINE2_IENGINESERVICEMGR_H
