#ifndef FUNC_MOVELINEAR_H
#define FUNC_MOVELINEAR_H

#pragma once

#include "basetoggle.h"

//-----------------------------------------------------------------------------
// CFuncMoveLinear adds 1 method to CBaseToggle: Linux slot 272, Windows 271.
//-----------------------------------------------------------------------------
template < class TBase = CBaseToggleVTable<> >
abstract_class CFuncMoveLinearVTable : public TBase
{
public:
	// With m_bCreateNavObstacle (key "CreateNavObstacle") set creates and registers the nav obstacle. Named by what it does
	virtual void CreateNavObstacle() = 0;
};

#endif // FUNC_MOVELINEAR_H
