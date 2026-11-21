//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that loads the entity subclass vdata files. Factory
//			"EntitySubclassGameSystem".
//
//=============================================================================//

#ifndef ENTITYSUBCLASSGAMESYSTEM_H
#define ENTITYSUBCLASSGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "resourcesystem/ivdataresourcelistener.h"
#include "tier0/bufferstring.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlsymbol.h"
#include "tier0/utlvector.h"

class CEntitySubclassVDataBase;

// Named by what the code calls on it (no implementation in the server): supplies
// the entity classes of a scope file (slot 0 the count, slot 1 the class name by
// index), the vdata class of an entity class (slot 3), the scope's vdata class
// name (slot 4)
class IEntitySubclassScopeResolver;

// Named by what the code does with it: told (slot 0, no arguments) after the
// subclasses were reloaded
class IEntitySubclassObserver;

//-----------------------------------------------------------------------------
// Static instance, 344 bytes on both platforms. libserver.so repeats the
// OnVDataReloaded override at the end of the main table (slot 66). The members
// are named by what the code does with them (the error messages name
// LoadSubclasses, ParseVDataFile and ParseSubclass); m_VDataMap, m_Observers and
// m_ReloadIndices keep the names of the plugins' copy of this class.
//-----------------------------------------------------------------------------
class CEntitySubclassGameSystem : public CAutoGameSystem, public IVDataResourceListener
{
public:
	DECLARE_GAME_SYSTEM();

	// Allocates the two scopes, names them after the classes with a MEntitySubclassScopeFile
	// (file and MPropertyFriendlyName), registers this system as the listener of each file and
	// fills m_RegisteredVDataClasses from the static registrations
	virtual bool Init() override;
	// Unregisters the listeners and deletes the scopes
	virtual void Shutdown() override;
	// LoadSubclasses, unless m_bSubclassesLoaded
	GS_EVENT( GameInit );
	// Releases the vdata resources and empties m_VDataMap and m_SubclassAliases
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// IVDataResourceListener: reparses the scope of the resource (m_ReloadIndices) and tells
	// the observers
	virtual void OnVDataReloaded( ResourceHandle_t hResource ) override;

public:
	// One subclass vdata file (scope); 304 bytes
	struct Scope_t
	{
		// The vdata file name
		CBufferStringN< 200 > m_FileName;
		// Zeroed with the scope, neither written nor read afterwards
		uint64 m_nNeverAccessed[ 2 ];
		// MPropertyFriendlyName of the scope's class
		CUtlString m_FriendlyName;
		// The loaded file (a reference is held)
		ResourceHandle_t m_hResource;
		// Tokens of the entity class names of the scope
		CUtlHashtable< CUtlStringToken > m_EntityClassTokens;
		// The entity class names of the scope, built once from m_pResolver and the entity classes
		CUtlVector< CUtlString > m_EntityClasses;
		// From a static registration list that is empty in the server
		IEntitySubclassScopeResolver *m_pResolver;
	};

	// The vdata class of an entity class
	struct VDataClassInfo_t
	{
		CUtlString m_VDataClassName;
		// Index into m_pScopes, -1 for none
		int m_nScope;
	};

	// new[] array of the two scopes
	Scope_t *m_pScopes;
	// Lowercase entity class name token -> its vdata class, cached by the lookups
	CUtlHashtable< CUtlStringToken, VDataClassInfo_t > m_EntityClassVDataClasses;
	// Lowercase name token -> vdata class, from the static registrations (CCSWeaponBaseVData,
	// CBasePlayerWeaponVData, ...)
	CUtlHashtable< CUtlStringToken, VDataClassInfo_t > m_RegisteredVDataClasses;
	// Subclass name token -> parsed subclass (deleted through its virtual destructor)
	CUtlHashtable< CUtlStringToken, CEntitySubclassVDataBase * > m_VDataMap;
	// Subclass name token -> token of the subclass it stands for
	CUtlHashtable< CUtlStringToken, CUtlStringToken > m_SubclassAliases;
	// Indexed by the symbol of a vdata class name in m_VDataClassSymbols: the entity class
	// names that use that vdata class, sorted
	CUtlVector< CUtlVector< CUtlString > > m_EntityClassesByVDataClass;
	CUtlSymbolTable m_VDataClassSymbols;
	// The scope ParseVDataFile works on (-1 outside of it)
	int m_nCurrentParseScope;
	// Set while a reload is pending
	bool m_bReloading;
	// Set once the scopes' entity class lists were built
	bool m_bEntityClassListsBuilt;
	bool m_bSubclassesLoaded;
	CUtlVector< IEntitySubclassObserver * > m_Observers;
	// The scopes to reparse
	CUtlVector< int > m_ReloadIndices;
};

#endif // ENTITYSUBCLASSGAMESYSTEM_H
