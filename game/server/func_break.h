#ifndef FUNC_BREAK_H
#define FUNC_BREAK_H

#pragma once

#include "../shared/basemodelentity.h"

//-----------------------------------------------------------------------------
// CBreakable adds 1 method to CBaseModelEntity: Linux slot 272, Windows 271.
//-----------------------------------------------------------------------------
template < class TBase = CBaseModelEntityVTable<> >
abstract_class CBreakableVTable : public TBase
{
public:
	virtual void Die() = 0;
};

#endif // FUNC_BREAK_H
