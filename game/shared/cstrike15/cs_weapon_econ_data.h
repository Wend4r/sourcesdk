#ifndef CS_WEAPON_ECON_DATA_H
#define CS_WEAPON_ECON_DATA_H
#ifdef _WIN32
#pragma once
#endif

#include "econ/econ_item_constants.h"

#include <tier0/utlstring.h>
#include <tier0/utlvector.h>

class CEconItemDefinition;
struct WeaponPaintableMaterial_t;

//-----------------------------------------------------------------------------
// The data the game keeps per item definition index and refreshes when the item schema changes.
// No source names the class members or the method; they are named by what the code does with
// them. The class has no virtual destructor: its owner deletes the concrete type.
//-----------------------------------------------------------------------------
class CItemDataCache
{
public:
	// Rereads the cached data from m_pItemDef; returns whether the definition is usable.
	// The result is kept in m_bValid
	virtual bool Update() = 0;

public:
	// The m_pszDefinitionName of the item definition, set by Update
	const char *m_pszDefinitionName;

	item_definition_index_t m_nDefIndex;
	bool m_bValid;

	// The version of the item schema m_pItemDef was looked up in; -1 until the first lookup.
	// When the schema's version differs, the definition is looked up again and Update runs
	int m_nSchemaVersion;

	// The definition of m_nDefIndex in the item schema (a CCStrike15ItemDefinition)
	const CEconItemDefinition *m_pItemDef;
};

//-----------------------------------------------------------------------------
// The econ data of a CS weapon item definition (80 bytes on both platforms), created on demand
// and owned by CCSWeaponSystem::m_WeaponEconData; also cached in
// CEconItemDefinition::m_pWeaponEconData. Every member is named by what Update does with it.
//-----------------------------------------------------------------------------
class CWeaponEconData : public CItemDataCache
{
public:
	// Fails when the definition has no asset info, or when its item class is not one of the
	// 47 stock weapon and equipment classes the game knows
	virtual bool Update() override;

public:
	// The item base name of the definition (its virtual getter, m_pszItemBaseName by default)
	CUtlString m_ItemBaseName;

	// The m_pszItemClassname of the definition, e.g. "weapon_ak47"; looked up in the table of
	// stock weapon classes
	CUtlString m_ItemClassname;

	// A second copy of the m_pszItemClassname of the definition; neither the server nor the
	// client fills it from anything else
	CUtlString m_ItemClassnameCopy;

	// The definition of the stock item of m_ItemClassname (the game's table holds a
	// CSchemaItemDefHandle per class name)
	const CEconItemDefinition *m_pStockItemDef;

	// The m_PaintData of the definition; set only when Update succeeds
	const CUtlVector< WeaponPaintableMaterial_t > *m_pPaintData;

	// CCStrike15ItemDefinition::GetUsedByTeam of the definition: the team the item is limited
	// to, or 0
	int m_nUsedByTeam;
};

#endif // CS_WEAPON_ECON_DATA_H
