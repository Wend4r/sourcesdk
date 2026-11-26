//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Game specific hooks of the tools ( map compile modifiers, VData stripping, tools rendering )
//
//===========================================================================//

#ifndef ISOURCE2MODTOOLS_H
#define ISOURCE2MODTOOLS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier1/utlvector.h"
#include "appframework/iappsystem.h"

class IRenderContext;
class KeyValues3;

// One VData entry that the VData compiler strips from compiled files ( entries of pszVDataFile whose name
// starts with pszEntryPrefix )
struct ModToolsVDataStripEntry_t
{
	const char *m_pszVDataFile;
	int m_nType;
	const char *m_pszEntryPrefix;
};

// Named by what it holds: a view of a static array of strip entries, returned by value
struct ModToolsVDataStripList_t
{
	const ModToolsVDataStripEntry_t *m_pEntries;
	int m_nCount;
};

abstract_class ISource2ModTools : public IAppSystem
{
public:
	// Named by what it does: fills the tools visualisation constant buffer ( 656 bytes ) of the render context
	// from the settings ( "Invalid MaterialSamplerMode: %d. Can't setup tools vis cbuffer." )
	virtual void *SetupToolsVisConstantBuffer( IRenderContext *pRenderContext, const void *pSettings ) = 0;

	// Runs the map compile modifier named by the "name" member of pModifier; false when there is none
	virtual bool RunMapCompileModifier( KeyValues3 *pModifier ) = 0;

	// Named by what it does: returns an empty string in this game
	virtual CUtlString GetCustomData() = 0;

	// Named by what it does: lets the game change the localization token path of a VData node; false ( no
	// change ) in this game
	virtual bool AdjustVDataLocalizationToken( const char *pszFile, const char *pszNode, void *pContext, CUtlVector< CUtlString > *pTokenPath, void *pNode ) = 0;

	// Named by what it does: extra map data items for the map data provider; empty in this game
	virtual void GetAdditionalMapDataItems( CUtlVector< void * > *pItems ) = 0;

	// The VData entries stripped from compiled VData files ( "VData ModTools Stripping Version" )
	virtual ModToolsVDataStripList_t GetVDataStripList() = 0;
};

#endif // ISOURCE2MODTOOLS_H
