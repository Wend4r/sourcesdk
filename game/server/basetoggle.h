#ifndef BASETOGGLE_H
#define BASETOGGLE_H

#pragma once

#include "../shared/basemodelentity.h"

//-----------------------------------------------------------------------------
// CBaseToggle adds no method to CBaseModelEntity. Linux 272 slots, Windows 271.
//-----------------------------------------------------------------------------
template < class TBase = CBaseModelEntityVTable<> >
abstract_class CBaseToggleVTable : public TBase
{
public:
};

#endif // BASETOGGLE_H
