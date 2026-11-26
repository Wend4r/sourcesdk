//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Asset move / copy / delete / replace operations of the tools and their dialogs
//
//===========================================================================//

#ifndef IASSETRENAMESYSTEM_H
#define IASSETRENAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier1/utlvector.h"
#include "appframework/iappsystem.h"

class IAsset;

abstract_class IAssetRenameSystem : public IAppSystem
{
public:
	// Dialogs over a list of assets; nothing happens for an empty list. The move and copy dialogs are the same
	// dialog ( "Asset Move (%1)" / "Asset Copy (%1)" ) opened with its mode flag set or clear
	virtual void ShowMoveAssetsDialog( const CUtlVector< IAsset * > &assets ) = 0;
	virtual void ShowMoveUnusedAssetsDialog( const CUtlVector< IAsset * > &assets ) = 0;
	virtual void ShowCopyAssetsDialog( const CUtlVector< IAsset * > &assets ) = 0;
	virtual void ShowDeleteAssetsDialog( const CUtlVector< IAsset * > &assets ) = 0;
	virtual void ShowReplaceAssetsDialog( const CUtlVector< IAsset * > &assets ) = 0;
	virtual void ShowAssetStatisticsDialog( const CUtlVector< IAsset * > &assets ) = 0;
	virtual void ShowAssetReferenceVisualization( const CUtlVector< IAsset * > &assets ) = 0;

	// Named by what they do: normalise a CONTENT path in place; the first also replaces it with the path the
	// file system resolves for it
	virtual void NormalizeAndResolveContentPath( CUtlString *pPath ) = 0;
	virtual void NormalizeContentPath( CUtlString *pPath ) = 0;

	// Named by what they do: build the file operations of a move and of a copy of assets ( source, destination,
	// name remapping and options in, operations out )
	virtual bool BuildMoveOperations( const void *pAssets, const char *pszDestFolder, const void *pRemapping, void *pOptions, void *pOperations, void *pErrors ) = 0;
	virtual bool BuildCopyOperations( const void *pAssets, const char *pszDestFolder, const void *pRemapping, void *pOptions, void *pOperations, void *pErrors ) = 0;

	// Named by what they do: run the file operations ( revision control edit / integrate / add / delete or plain
	// file copies, "<hr>" separated log ); the first forwards through its own table slot
	virtual bool ExecuteOperationsForwarded( const void *pOperations, CUtlString *pLog ) = 0;
	virtual bool ExecuteOperations( const void *pOperations, CUtlString *pLog ) = 0;
};

#endif // IASSETRENAMESYSTEM_H
