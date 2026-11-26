//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: The asset database of the tools: assets, asset types, mods, working sets, tags and the
//          file change processing that keeps compiled resources up to date
//
//===========================================================================//

#ifndef IASSETSYSTEM_H
#define IASSETSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier1/utlvector.h"
#include "appframework/iappsystem.h"

class IAsset;
class IAssetDir;
class IAssetType;
class IWorkingSet;
class IAssetCacheListener;
class IAssetFileListener;
class IAssetTagListener;
class IAssetFileSystemInterface;
class KeyValues3;

// Named by what it holds: the index of a mod in the asset system ( -1 when none ); returned by value
struct AssetModHandle_t
{
	int m_nModIndex;
};

// Assets are passed in vectors of IAsset pointers. A mod index is the position of a mod ( a game or content
// search path layer ) in the asset system; tags are 16-bit symbols of the asset tag table.
abstract_class IAssetSystem : public IAppSystem
{
public:
	// Working sets: the ones created by the user first, then the procedural ones
	virtual int GetWorkingSetCount() = 0;
	virtual IWorkingSet *GetWorkingSet( int nIndex ) = 0;
	virtual IWorkingSet *FindWorkingSet( const CUtlString &name ) = 0;
	// An empty name creates "Unnamed"
	virtual IWorkingSet *CreateWorkingSet( const CUtlString &name ) = 0;
	virtual bool DestroyWorkingSet( IWorkingSet *pWorkingSet ) = 0;
	virtual void AddProceduralWorkingSet( IWorkingSet *pWorkingSet ) = 0;
	virtual void RemoveProceduralWorkingSet( IWorkingSet *pWorkingSet ) = 0;
	// Named by what it does: drops the working set from the most recently used list ( and refreshes the MRU
	// working set when it is that one )
	virtual void ForgetWorkingSet( IWorkingSet *pWorkingSet ) = 0;
	virtual IWorkingSet *GetAllAssetsWorkingSet() = 0;
	virtual IWorkingSet *GetMRUWorkingSet() = 0;
	// Named by what it does: returns nullptr
	virtual IWorkingSet *GetEmptyWorkingSet() = 0;
	// "name (2)", "name (3)", ... until no working set has the name
	virtual void MakeUniqueWorkingSetName( const CUtlString &baseName, CUtlString *pUniqueName ) = 0;

	// Assets by file name ( absolute, mod relative or asset relative ); nullptr when unknown
	virtual IAsset *FindAssetByFilename( const char *pszFilename ) = 0;
	virtual IAsset *FindAssetByAssetRelativePath( const char *pszRelativePath ) = 0;
	virtual IAssetDir *FindAssetDirByAssetRelativePath( const char *pszRelativePath ) = 0;
	// Named by what they do: a list of asset names separated by new lines, spaces, tabs, commas or semicolons
	// to assets, and assets to their names separated by new lines
	virtual void ParseAssetList( const char *pszText, CUtlVector< IAsset * > *pAssets ) = 0;
	virtual void FormatAssetList( const CUtlVector< IAsset * > &assets, CUtlString *pText ) = 0;
	// "Game" for 0, "Content" for 1
	virtual const char *GetAssetLocationName( int nLocation ) = 0;

	// Asset types. The overloads of a name sit in reverse declaration order in the table
	virtual IAssetType *FindAssetTypeByExtension( const char *pszExtension ) = 0;
	virtual IAssetType *FindAssetTypeByExtension( const CUtlString &extension ) = 0;
	virtual IAssetType *FindAssetTypeByName( const char *pszName ) = 0;
	virtual IAssetType *FindAssetTypeByName( const CUtlString &name ) = 0;
	virtual void GetAssetTypes( CUtlVector< IAssetType * > *pAssetTypes ) = 0;
	virtual IAssetType *FindAssetTypeByResourceType( int64 nResourceType ) = 0;

	virtual void GetModNames( CUtlVector< CUtlString > *pModNames ) = 0;

	// Reload ( "Manual Reload" ) and recompile of assets
	virtual void ReloadAssets( const CUtlVector< IAsset * > &assets ) = 0;
	virtual void RecompileAssets( const CUtlVector< IAsset * > &assets, uint32 nFlags ) = 0;
	virtual void HandleFileChange( const CUtlString &filename ) = 0;

	virtual void AddAssetCacheListener( IAssetCacheListener *pListener ) = 0;
	virtual void RemoveAssetCacheListener( IAssetCacheListener *pListener ) = 0;
	virtual void AddAssetFileListener( IAssetFileListener *pListener ) = 0;
	virtual void RemoveAssetFileListener( IAssetFileListener *pListener ) = 0;
	// Named by what they do: listeners kept per key ( the 32-bit value at +8 of pKey )
	virtual void AddKeyedAssetFileListener( IAssetFileListener *pListener, const void *pKey ) = 0;
	virtual void RemoveKeyedAssetFileListener( IAssetFileListener *pListener, const void *pKey ) = 0;

	virtual bool InitializeAssetSystem( IAssetFileSystemInterface *pFileSystem ) = 0;
	virtual void ProcessFileChanges() = 0;
	// Checks the assets out from revision control; assets of read-only mods are skipped
	virtual void OpenForEdit( const CUtlVector< IAsset * > &assets, uint32 nFlags ) = 0;

	// Named by what they do: three byte flags of the asset system state
	virtual bool IsInitialized() = 0;
	virtual bool IsScanning() = 0;
	virtual bool HasPendingChanges() = 0;
	// Named by what it does: the pending changes as ", " separated names ( "+name" for added ones ); false when
	// there are none
	virtual bool GetPendingChangesDescription( CUtlString *pDescription ) = 0;
	virtual IAssetFileSystemInterface *GetFileSystem( int nLocation ) = 0;

	// File CRCs ( cached per asset; computed through the asset file system interface when missing )
	virtual bool GetAssetFileCRC( IAsset *pAsset, uint32 *pCRC ) = 0;
	virtual bool GetFileCRC( const char *pszFilename, uint32 *pCRC ) = 0;
	virtual int UpdateAllAssetFileCRCs( uint32 nFlags, uint64 *pTotalBytes ) = 0;
	virtual bool GetAssetFullPath( IAsset *pAsset, CUtlString *pFullPath ) = 0;

	virtual int GetAssetTypeCount() = 0;
	virtual int GetModCount() = 0;
	// Named by what it does: the mod handle of the current mod ( -1 when none )
	virtual AssetModHandle_t GetCurrentMod() = 0;
	// Named by what it does: the search path roots of the mods
	virtual void GetModSearchPaths( CUtlVector< CUtlString > *pPaths ) = 0;
	virtual AssetModHandle_t FindMod( const char *pszModName ) = 0;
	virtual const char *GetModName( int nModIndex ) = 0;
	virtual bool IsModReadOnly( int nModIndex ) = 0;
	// Named by what it does: the root paths of the mods for a location ( 0 game, 1 content )
	virtual void GetModRootPaths( int nLocation, CUtlVector< CUtlString > *pPaths ) = 0;
	// Named by what it does: empty
	virtual void NoOp() = 0;

	// Named by what they do: the per-asset persistent info ( read: unresolvable assets are skipped, write: ids
	// as "%llu" ); the two Reset slots share one body
	virtual bool ReadPersistentAssetInfo( void *pBuffer, int nModIndex ) = 0;
	virtual void ResetPersistentAssetInfo() = 0;
	virtual bool ReadPersistentAssetInfoForMod( void *pBuffer, int nModIndex, bool bOverride ) = 0;
	virtual bool WritePersistentAssetInfo( void *pBuffer, int nModIndex, bool bOverride ) = 0;
	virtual void ResetPersistentAssetInfoForMod() = 0;

	// Writes the persistent asset info when nMinSecondsSinceLastFlush have passed
	virtual bool FlushPersistentAssetInfo( double flMinSecondsSinceLastFlush ) = 0;
	// Named by what it does: logs the orphaned compiled resources ( child resources without a parent or a content
	// file ) of the assets that pFilter accepts
	virtual void ReportOrphanedResources( void *pFilter, CUtlVector< IAsset * > *pOrphans, bool bVerbose ) = 0;
	// "Handling batched file changes..." : HandleFileChange for each file
	virtual void HandleFileChanges( const char **ppFilenames, int nCount ) = 0;
	// Named by what it does: hands the compiled data of the assets to pResource ( dynamic_cast IAsset -> CAsset )
	virtual bool UpdateResourceFromAssets( void *pResource, void *pContext, const CUtlVector< IAsset * > &assets ) = 0;
	// Named by what it does: saves the persistent info of a mod
	virtual bool SaveModPersistentInfo( int nModIndex ) = 0;
	// "AssetSystem: Mod '%s' will use override info from '%s'"
	virtual bool SetModOverrideInfo( const char *pszModName, const char *pszOverrideModName ) = 0;

	// Asset tags
	virtual const char *GetTagName( uint16 nTag ) = 0;
	virtual void *GetTagInfo( uint16 nTag ) = 0;
	virtual void FindTags( const char *pszSearch, CUtlVector< uint16 > *pTags ) = 0;
	// Named by what they do: a nesting counter that defers saving the changed tags ( saved when it returns to
	// zero ), and the dirty flag of the tag database
	virtual void BeginTagChanges( bool bBegin ) = 0;
	virtual void SetTagsDirty( bool bDirty ) = 0;
	virtual void GetAllTags( CUtlVector< uint16 > *pTags ) = 0;
	virtual void GetUsedTags( CUtlVector< uint16 > *pTags ) = 0;
	virtual uint16 FindTag( const char *pszTagName ) = 0;
	// Named by what it does: whether every tag has an owner ( false on customer machines )
	virtual bool AreAllTagsOwned() = 0;
	virtual void AddAssetTagListener( IAssetTagListener *pListener ) = 0;
	virtual void RemoveAssetTagListener( IAssetTagListener *pListener ) = 0;

	// Named by what it does: whether the 64-bit asset id is known
	virtual bool IsValidAssetId( uint64 nAssetId ) = 0;
	// Named by what it does: the assets whose names match the wildcard filter, appended to pAssets
	virtual void FindAssetsByWildcard( const char *pszFilter, void *pResults, CUtlVector< IAsset * > *pAssets ) = 0;
	// Named by what they do: adds the asset that another system reports as active to the MRU working set, and
	// adds an asset to the MRU working set
	virtual void AddActiveAssetToMRUWorkingSet() = 0;
	virtual void AddToMRUWorkingSet( IAsset *pAsset, void *pContext ) = 0;

	// Subasset types and subassets
	virtual void *FindSubasset( const char *pszSubassetTypeName, const char *pszSubassetName ) = 0;
	virtual void *FindSubassetType( const char *pszTypeName ) = 0;
	virtual void GetSubassetTypes( CUtlVector< void * > *pTypes ) = 0;
	// Named by what it does: matches the string that slot 17 of each asset type returns
	virtual IAssetType *FindAssetTypeByKey( const char *pszKey ) = 0;
	// Named by what it does: a counter of the asset system state
	virtual int GetStateSerialNumber() = 0;
	// Named by what it does: the assets whose asset type reports nResourceType
	virtual void GetAssetsOfResourceType( CUtlVector< IAsset * > *pAssets, int nResourceType ) = 0;
	// Named by what it does: the address of an embedded member object
	virtual void *GetTagsManager() = 0;
	// Named by what it does: FindAssetsMatching, then keeps the assets whose match flag is set
	virtual void FindAssetsMatchingFiltered( CUtlVector< IAsset * > *pAssets, void *pQuery, void *pContext ) = 0;
	virtual int GetMRUIndex( IAsset *pAsset ) = 0;
	// Named by what they do: resolve asset names ( with their asset type ) and dependencies in bulk
	virtual void ResolveAssetNames( const CUtlVector< CUtlString > &names, CUtlVector< IAsset * > *pAssets ) = 0;
	virtual void GetAllAssets( CUtlVector< IAsset * > *pAssets ) = 0;
	// Named by what they do: the current entry of an asset map, and a lookup in that map by name
	virtual IAsset *GetActiveAsset() = 0;
	virtual IAsset *FindAssetByResourceName( const char *pszResourceName ) = 0;
	virtual void FindAssetsMatching( CUtlVector< IAsset * > *pAssets, CUtlVector< bool > *pMatches, void *pQuery, void *pContext, void *pOptions ) = 0;
	// Whether a file name is one of the generated map compile outputs ( _buildcubemaps, _bakeresourcecache,
	// _buildsparseshadowtree )
	virtual bool IsGeneratedMapCompileFile( const char *pszFilename ) = 0;
};

abstract_class IAssetSystemTest : public IAppSystem
{
public:
	// Runs the asset system self test on a fake file system ( "==== AssetSystemTest Complete! ====" )
	virtual bool RunTests() = 0;
};

#endif // IASSETSYSTEM_H
