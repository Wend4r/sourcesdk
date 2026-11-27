#ifndef CSTRIKE15_CS_OBSERVER_OBSERVERSERVICES_H
#define CSTRIKE15_CS_OBSERVER_OBSERVERSERVICES_H

#pragma once

#include "../player_observerservices.h"

//-----------------------------------------------------------------------------
// CCSObserver_ObserverServices adds no virtual method to the server table and
// one to the client table, client slot 55. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayer_ObserverServicesVTable<> >
abstract_class CCSObserver_ObserverServicesVTable : public TBase
{
#ifdef CLIENT_DLL
public:
	// Whether a key binding of the observer goes through: true for the +/- form of one
	// command and in some engine states; two other bindings send replay_stop instead and
	// return false. The bindings compared are showscores, +attack, +attack2 and +jump.
	virtual bool Unk_55( const char *pszBinding ) = 0;
#endif
};

#endif // CSTRIKE15_CS_OBSERVER_OBSERVERSERVICES_H
