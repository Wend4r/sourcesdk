#ifndef POINT_CLIENTUI_WORLD_PANEL_H
#define POINT_CLIENTUI_WORLD_PANEL_H

#pragma once

#include "baseclientuientity.h"

//-----------------------------------------------------------------------------
// CPointClientUIWorldPanel adds 1 method to CBaseClientUIEntity: Linux slot 275, Windows 274.
//-----------------------------------------------------------------------------
template < class TBase = CBaseClientUIEntityVTable<> >
abstract_class CPointClientUIWorldPanelVTable : public TBase
{
public:
	// Returns m_bIgnoreInput. Named by what it does
	virtual bool IgnoresInput() = 0;
};

#endif // POINT_CLIENTUI_WORLD_PANEL_H
