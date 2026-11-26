#ifndef BASECLIENTUIENTITY_H
#define BASECLIENTUIENTITY_H

#pragma once

#include "basemodelentity.h"

//-----------------------------------------------------------------------------
// CBaseClientUIEntity adds 3 methods to CBaseModelEntity: Linux slots 272-274, Windows 271-273.
//-----------------------------------------------------------------------------
template < class TBase = CBaseModelEntityVTable<> >
abstract_class CBaseClientUIEntityVTable : public TBase
{
public:
	// Empty; CPointClientUIDialog stores the entity in m_hActivator
	virtual void Unk_271( CBaseEntity *pEntity ) = 0;
	// CPointClientUIWorldPanel sets m_bEnabled (network change), CPointClientUIDialog starts its think. Named by what it does
	virtual void Enable() = 0;
	// CPointClientUIWorldPanel clears m_bEnabled, CPointClientUIDialog stops its think. Named by what it does
	virtual void Disable() = 0;
};

#endif // BASECLIENTUIENTITY_H
