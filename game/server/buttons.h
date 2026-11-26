#ifndef BUTTONS_H
#define BUTTONS_H

#pragma once

#include "basetoggle.h"

//-----------------------------------------------------------------------------
// CBaseButton adds 10 methods to CBaseToggle: Linux slots 272-281, Windows 271-280.
//-----------------------------------------------------------------------------
template < class TBase = CBaseToggleVTable<> >
abstract_class CBaseButtonVTable : public TBase
{
public:
	// CBaseButton::Spawn calls it: defaults m_flSpeed / m_flWait / m_flLip, collapses m_vecPosition2 onto m_vecPosition1
	virtual void SetupButton() = 0;
	virtual void ButtonActivate() = 0;
	virtual void ButtonTouch( CBaseEntity *pOther ) = 0;
	// ButtonActivate makes it the move-done function (m_pfnMoveDone)
	virtual void TriggerAndWait() = 0;
	// Msg "calling ButtonReturn with a toggle state of %d"
	virtual void ButtonReturn() = 0;
	// ButtonReturn makes it the move-done function (m_pfnMoveDone)
	virtual void ButtonBackHome() = 0;
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
	virtual void Enable() = 0;
	virtual void Disable() = 0;
};

#endif // BUTTONS_H
