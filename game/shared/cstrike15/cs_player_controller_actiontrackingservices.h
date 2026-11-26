#ifndef CSTRIKE15_CS_PLAYER_CONTROLLER_ACTIONTRACKINGSERVICES_H
#define CSTRIKE15_CS_PLAYER_CONTROLLER_ACTIONTRACKINGSERVICES_H

#pragma once

#include "../playercontroller_component.h"
#include "networksystem/networkvar_vtable.h"

//-----------------------------------------------------------------------------
// CCSPlayerController_ActionTrackingServices adds no virtual method. See
// playercontroller_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerControllerComponentVTable >
abstract_class CCSPlayerController_ActionTrackingServicesVTable : public TBase
{
};

class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// Virtual table of CSPerRoundStats_t, the statistics of one round (m_perRoundStats);
// CSMatchStats_t (m_matchStats) derives from it and only overrides. The class carries a
// VTable suffix and declares virtual methods only: the game class of the same name is a
// schema class, so a module declares its own class with the members it needs and derives it
// from this one. Five slots on both platforms, no virtual destructor.
//-----------------------------------------------------------------------------
abstract_class CSPerRoundStats_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED_VTABLE()
};

#endif // CSTRIKE15_CS_PLAYER_CONTROLLER_ACTIONTRACKINGSERVICES_H
