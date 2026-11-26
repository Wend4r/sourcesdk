//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: The asset browser of the tools: its window, embedded browsers and pickers, asset menus
//
//===========================================================================//

#ifndef IASSETBROWSERSYSTEM_H
#define IASSETBROWSERSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier1/utlvector.h"
#include "appframework/iappsystem.h"

class QWidget;
class QMenu;
class IAsset;
class IWorkingSet;
class IAssetBrowseCallbacks;
class IEmbeddedAssetPicker;
class IEmbeddedAssetBrowser;

abstract_class IAssetBrowserSystem : public IAppSystem
{
public:
	// Creates the asset browser window and loads its options ( asset_browser settings )
	virtual void StartAssetBrowserSystem() = 0;
	// Named by what it does: writes back the state of the browser pages that changed
	virtual void SaveBrowserPageState() = 0;

	// Context menu of a list of assets ( "Asset Tools", "Asset Info", "Copy Asset Name", ... ); Create allocates
	// the menu and fills it through Populate
	virtual QMenu *CreateAssetContextMenu( const CUtlVector< IAsset * > &assets, IAssetBrowseCallbacks *pCallbacks ) = 0;
	virtual void PopulateAssetContextMenu( QMenu *pMenu, const CUtlVector< IAsset * > &assets, IAssetBrowseCallbacks *pCallbacks ) = 0;
	// Named by what it does: builds the context menu of the assets and runs it at the cursor position
	virtual void ExecAssetContextMenu( const CUtlVector< IAsset * > &assets, int nFlags ) = 0;

	virtual IEmbeddedAssetPicker *CreateEmbeddedAssetPicker( QWidget *pParent, const char *pszAssetType, void *pContext ) = 0;
	virtual void DestroyEmbeddedAssetPicker( IEmbeddedAssetPicker *pPicker ) = 0;
	virtual IEmbeddedAssetBrowser *CreateEmbeddedAssetBrowser( QWidget *pParent, bool bShowWorkingSetTabs ) = 0;
	virtual void DestroyEmbeddedAssetBrowser( IEmbeddedAssetBrowser *pBrowser ) = 0;

	// Named by what they do: an asset picker dialog for a browse request ( selects the requested asset
	// deferred ); the result is the dialog's interface, released with CloseAssetPickerDialog
	virtual void *OpenAssetPickerDialog( void *pContext, QWidget *pParent, const void *pRequest, const char *pszTitle, bool bModal ) = 0;

	// The native window handle of the asset browser window
	virtual uint64 GetMainWindowHandle() = 0;
	// Shows, restores and raises the asset browser window ( or the legacy asset dialog )
	virtual void ShowAssetBrowser() = 0;
	// Named by what it does: the address of the embedded options object
	virtual void *GetOptions() = 0;
	// "Revision Helper" window for the assets
	virtual void ShowRevisionHelper( const CUtlVector< IAsset * > &assets, bool bShowUnmodified ) = 0;

	// Named by what they do: the assets of the list whose asset type is one of the filter's types
	virtual void FilterAssets( const CUtlVector< IAsset * > &assets, const void *pFilter, CUtlVector< IAsset * > *pFiltered ) = 0;
	virtual bool AssetPassesFilter( IAsset *pAsset, const void *pFilter ) = 0;

	virtual void CloseAssetPickerDialog( void *pDialog ) = 0;

	// Named by what they do: the selection of the main browser page, and a selection change of it
	virtual void GetSelectedAssets( CUtlVector< IAsset * > *pAssets ) = 0;
	virtual void SelectAssets( const CUtlVector< IAsset * > &assets, void *pContext, int nFlags ) = 0;

	// "Asset List" popup with the assets
	virtual void ShowAssetListPopup( const char *pszTitle, int nFlags, const CUtlVector< IAsset * > *pAssets, void *pContext ) = 0;

	virtual QWidget *CreateAssetComboBox( QWidget *pParent, const char *pszAssetType, void *pContext ) = 0;
	virtual QWidget *CreateSubassetComboBox( QWidget *pParent, const char *pszSubassetType ) = 0;

	// Named by what they do: shows the "Switch to 'All Assets'" button on the pages that hide the asset, makes
	// the pages refresh an asset, and closes the tabs of a working set
	virtual void ShowSwitchToAllAssetsHint( IAsset *pAsset ) = 0;
	virtual void RefreshAsset( IAsset *pAsset ) = 0;
	virtual void CloseWorkingSetTabs( IWorkingSet *pWorkingSet ) = 0;

	// Context menu of a subasset ( "Highlight '%1' In %2" definitions / references )
	virtual QMenu *CreateSubassetContextMenu( const void *pSubasset ) = 0;
	virtual void PopulateSubassetContextMenu( QMenu *pMenu, const void *pSubasset ) = 0;
};

#endif // IASSETBROWSERSYSTEM_H
