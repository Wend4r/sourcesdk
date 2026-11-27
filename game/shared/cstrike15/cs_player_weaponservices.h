#ifndef CSTRIKE15_CS_PLAYER_WEAPONSERVICES_H
#define CSTRIKE15_CS_PLAYER_WEAPONSERVICES_H

#pragma once

#include "../player_weaponservices.h"
#include "resourcesystem/itoolsresourcelistener.h"

//-----------------------------------------------------------------------------
// CCSPlayer_WeaponServices adds no virtual method of its own. See
// playerpawn_component.h.
//
// The game class also derives from IToolsResourcePreReloadListener (at offset 176,
// on the client 168) and IToolsResourceListener (at 184, on the client 176), in
// that order. Its overrides of their
// methods are entered into the primary table on Linux only (the two slots after
// RefillActiveWeaponClips); the Windows table keeps them in the tables of the
// second bases. The second bases sit right behind the table pointer of these
// member-less classes, not where the game has them, so do not convert a pointer
// to them.
//-----------------------------------------------------------------------------
template < class TBase = CPlayer_WeaponServicesVTable<> >
abstract_class CCSPlayer_WeaponServicesVTable : public TBase, public IToolsResourcePreReloadListener, public IToolsResourceListener
{
public:
	// Overrides IToolsResourcePreReloadListener: a slot of this table on Linux only
	virtual void NotifyResourcePreReload( ResourceHandle_t hResource, ResourceType_t nResourceType ) override = 0;
	// Overrides IToolsResourceListener: a slot of this table on Linux only
	virtual void NotifyResourceStatusChange( const ResourceStatusChange_t *pChanges, int64 nCount ) override = 0;
};

#endif // CSTRIKE15_CS_PLAYER_WEAPONSERVICES_H
