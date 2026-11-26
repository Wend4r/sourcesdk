#ifndef MODELENTITIES_H
#define MODELENTITIES_H

#pragma once

#include "../shared/basemodelentity.h"

//-----------------------------------------------------------------------------
// CFuncBrush adds 3 methods to CBaseModelEntity: Linux slots 272-274, Windows 271-273.
//-----------------------------------------------------------------------------
template < class TBase = CBaseModelEntityVTable<> >
abstract_class CFuncBrushVTable : public TBase
{
public:
	virtual void TurnOff() = 0;
	virtual void TurnOn() = 0;
	// !( m_fEffects & EF_NODRAW )
	virtual bool IsOn() = 0;
};

#endif // MODELENTITIES_H
