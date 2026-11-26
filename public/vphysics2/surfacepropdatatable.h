#ifndef SURFACEPROPDATATABLE_H
#define SURFACEPROPDATATABLE_H

#pragma once

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlstring.h"

class IPhysSurfacePropertyController;

//-----------------------------------------------------------------------------
// The base of the surface property tables. It has no virtual table and no data members (an empty
// base at offset 0 of the reloadable table).
//-----------------------------------------------------------------------------
class CSurfacePropDataTableBase
{
};

//-----------------------------------------------------------------------------
// A table of surface properties that is loaded from its file and loaded again when the surface
// property controller of the physics system changes: the constructor links every table into a
// list of the module, and on a change of the controller every table whose recorded change number
// differs is loaded again. CSurfacePropReloadableDataTable< CGameSurfaceProperties > derives from
// it and CGameSurfacePropertiesList, the list of the game, from that. Four slots on both
// platforms, no virtual destructor.
//
// The first three are pure here and have no name source; each comment says what
// CGameSurfacePropertiesList does in them. Their parameters are not recovered: the declarations
// describe the slots, not how to call them.
//-----------------------------------------------------------------------------
abstract_class CSurfacePropReloadableDataTableBase : public CSurfacePropDataTableBase
{
public:
	// Resets the element count and frees the entries
	virtual void Unk_0() = 0;
	// Copies one 32-byte entry over another
	virtual void Unk_1() = 0;
	// Parses one surface property block ("maxspeedfactor", "jumpfactor", "isstairs", ...)
	virtual void Unk_2() = 0;
	// Named by what it does: parses the file m_FileName (a "Parse error on %s" message on
	// failure) and records the change number of pController
	virtual void LoadFromFile( IPhysSurfacePropertyController *pController ) = 0;

public:
	// Named by what it holds: the table constructed before this one (the head of the list is a
	// static of the module).
	CSurfacePropReloadableDataTableBase *m_pNext;
	// "scripts/surfaceproperties_game.txt" for the list of the game.
	CUtlString m_FileName;
	// Named by what it holds: the change number of the controller the table was last loaded
	// from.
	int32 m_nLoadedChangeNumber;
};

//-----------------------------------------------------------------------------
// A reloadable table of T entries. The game implements the overrides.
//-----------------------------------------------------------------------------
template < class T >
abstract_class CSurfacePropReloadableDataTable : public CSurfacePropReloadableDataTableBase
{
public:
	virtual void Unk_0() override;
	virtual void Unk_1() override;

public:
	// Named by what it holds.
	CUtlVector< T > m_Entries;
};

#endif // SURFACEPROPDATATABLE_H
