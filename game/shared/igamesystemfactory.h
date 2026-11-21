//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef IGAMESYSTEMFACTORY_H
#define IGAMESYSTEMFACTORY_H
#ifdef _WIN32
#pragma once
#endif

#include <string.h>
#include "tier0/dbg.h"
#include "igamesystem.h"

/*
* AMNOTE: To register your own game system, CGameSystemStaticFactory could be used to register it,
* but since the actual factory lives inside the game originally and we don't have direct access to it
* you need provide the CBaseGameSystemFactory::sm_pFirst by whatever means are possible to you.
* As a starting point you can look for "Game System %s is defined twice" string in the server binary
* to find it, if that string is missing then this comment is out of date, please, report or update it!
*
* Once CBaseGameSystemFactory::sm_pFirst is initialized and correct address provided,
* you can create your own gamesystem (refer to igamesystem.h on what it should be like) and register it
* via:
*
* // The factory links itself into the list of the game. Unlink and free it with ``Destroy`` inside the
* // ``Shutdown`` method of ``YourGameSystemClass``; a common way is to keep the factory as a static member
* // of ``YourGameSystemClass``.
* auto *pFactory = new CGameSystemStaticFactory< YourGameSystemClass >( "YourGameSystemName", &YourGameSystemInstance );
*
* after the new factory object has been created and if all is done correctly your game system should be viewable to the game
* and its callbacks should trigger.
*/

using GameSystemListeners_t = CUtlVector< CUtlVector< IGameSystem * > >;

//-----------------------------------------------------------------------------
// Creates and destroys one game system for IGameSystem::InitAllSystems.
// The interface has no virtual destructor (10 slots on both platforms); the factories are
// static objects of the game.
//-----------------------------------------------------------------------------
abstract_class IGameSystemFactory
{
public:
	virtual bool Init() = 0;
	virtual void PostInit() = 0;
	virtual void Shutdown() = 0;
	virtual IGameSystem *CreateGameSystem() = 0;
	virtual void DestroyGameSystem( IGameSystem *pGameSystem ) = 0;
	virtual bool ShouldAutoAdd() const = 0;
	virtual int GetPriority() const = 0;
	// IGameSystem::SetGameSystemGlobalPtrs of the created system forwards here
	virtual void SetGlobalPtr( void *pValue ) = 0;
	// IGameSystem::DoesGameSystemReallocate of the created system forwards here
	virtual bool IsReallocating() const = 0;
	virtual IGameSystem *GetStaticGameSystem() = 0;
};

//-----------------------------------------------------------------------------
// A named factory in the list that IGameSystem::InitAllSystems walks. The
// constructor puts it at the head of the list.
//-----------------------------------------------------------------------------
class CBaseGameSystemFactory : public IGameSystemFactory
{
protected:
	explicit CBaseGameSystemFactory( const char *pName )
	{
		m_pName = pName;
		m_pNext = *sm_pFirst;
		*sm_pFirst = this;
	}

	// Not virtual: the game's factories have no destructor slot. Destroy frees a factory
	// through its own type.
	~CBaseGameSystemFactory() = default;

	// Removes this factory from the list, a helper for Destroy of the derived factories
	void RemoveFromList()
	{
		CBaseGameSystemFactory *pFactoryCurrent = *sm_pFirst;
		CBaseGameSystemFactory *pFactoryPrevious = nullptr;
		while ( pFactoryCurrent )
		{
			if ( pFactoryCurrent == this )
			{
				if ( pFactoryPrevious == nullptr )
				{
					*sm_pFirst = pFactoryCurrent->m_pNext;
				}
				else
				{
					pFactoryPrevious->m_pNext = pFactoryCurrent->m_pNext;
				}
				m_pNext = nullptr;
				return;
			}
			pFactoryPrevious = pFactoryCurrent;
			pFactoryCurrent = pFactoryCurrent->m_pNext;
		}
	}

private:
	CBaseGameSystemFactory *m_pNext;
	const char *m_pName;

public:
	const char *GetName() const { return m_pName; }
	CBaseGameSystemFactory *GetNext() const { return m_pNext; }

	static void LoopByFactories( void ( *pfnFunc )( CBaseGameSystemFactory *pFactory ) )
	{
		CBaseGameSystemFactory *pFactoryList = *sm_pFirst;
		while ( pFactoryList )
		{
			pfnFunc( pFactoryList );
			pFactoryList = pFactoryList->GetNext();
		}
	}

	static CBaseGameSystemFactory *GetFactoryByName( const char *pName )
	{
		CBaseGameSystemFactory *pFactoryList = *sm_pFirst;
		while ( pFactoryList )
		{
			if ( strcmp( pFactoryList->m_pName, pName ) == 0 )
			{
				return pFactoryList;
			}
			pFactoryList = pFactoryList->m_pNext;
		}
		return nullptr;
	}

	static IGameSystem *GetGlobalPtrByName( const char *pName )
	{
		CBaseGameSystemFactory *pFactory = GetFactoryByName( pName );
		if ( pFactory )
		{
			return pFactory->GetStaticGameSystem();
		}
		return nullptr;
	}

	// AMNOTE: This needs to be provided by a thirdparty application,
	// and is required if you want to register your own gamesystem
	static CBaseGameSystemFactory **sm_pFirst;
};

//-----------------------------------------------------------------------------
// Factory of a game system that is a global object of the module. The game's
// instantiations return the system's own ShouldAutoAdd and GetPriority values
// (most of them true and 0, as here).
//-----------------------------------------------------------------------------
template < class T, class U = T >
class CGameSystemStaticFactory final : public CBaseGameSystemFactory
{
public:
	CGameSystemStaticFactory( const char *pName, T *pActualGlobal, U **ppGlobalPointer = nullptr ) : CBaseGameSystemFactory( pName )
	{
		m_pActualGlobal = pActualGlobal;
		m_ppGlobalPointer = ppGlobalPointer;
		pActualGlobal->SetName( pName );
	}

	// Unlinks the factory from the list and frees it (for factories made with new)
	void Destroy()
	{
		RemoveFromList();
		delete this;
	}

	bool Init() override { return m_pActualGlobal->Init(); }
	void PostInit() override { m_pActualGlobal->PostInit(); }
	void Shutdown() override { m_pActualGlobal->Shutdown(); }

	IGameSystem *CreateGameSystem() override
	{
		m_pActualGlobal->SetGameSystemGlobalPtrs( m_pActualGlobal );
		return m_pActualGlobal;
	}

	void DestroyGameSystem( IGameSystem *pGameSystem ) override
	{
		m_pActualGlobal->SetGameSystemGlobalPtrs( nullptr );
	}

	bool ShouldAutoAdd() const override { return true; }
	int GetPriority() const override { return 0; }

	void SetGlobalPtr( void *pValue ) override
	{
		if ( m_ppGlobalPointer )
		{
			*m_ppGlobalPointer = reinterpret_cast< T * >( pValue );
		}
	}

	bool IsReallocating() const override { return false; }
	IGameSystem *GetStaticGameSystem() override { return m_pActualGlobal; }

private:
	T *m_pActualGlobal;
	U **m_ppGlobalPointer;
};

//-----------------------------------------------------------------------------
// Factory of a game system that is allocated by every CreateGameSystem. The
// game's instantiations call the system's static init/shutdown helpers from
// Init/Shutdown when it has them (CSource2EntitySystem::StaticInit, ...) and
// return its own ShouldAutoAdd and GetPriority values.
//-----------------------------------------------------------------------------
template < class T, class U = T >
class CGameSystemReallocatingFactory final : public CBaseGameSystemFactory
{
public:
	CGameSystemReallocatingFactory( const char *pName, U **ppGlobalPointer = nullptr ) : CBaseGameSystemFactory( pName )
	{
		m_ppGlobalPointer = ppGlobalPointer;
	}

	// Unlinks the factory from the list and frees it (for factories made with new)
	void Destroy()
	{
		RemoveFromList();
		delete this;
	}

	bool Init() override { return true; }
	void PostInit() override {}
	void Shutdown() override {}

	IGameSystem *CreateGameSystem() override
	{
		T *pObject = new T();
		pObject->SetGameSystemGlobalPtrs( pObject );
		return pObject;
	}

	void DestroyGameSystem( IGameSystem *pGameSystem ) override
	{
		pGameSystem->SetGameSystemGlobalPtrs( nullptr );
		delete pGameSystem;
	}

	bool ShouldAutoAdd() const override { return true; }
	int GetPriority() const override { return 0; }

	void SetGlobalPtr( void *pValue ) override
	{
		if ( m_ppGlobalPointer )
		{
			*m_ppGlobalPointer = reinterpret_cast< T * >( pValue );
		}
	}

	bool IsReallocating() const override { return true; }
	IGameSystem *GetStaticGameSystem() override { return nullptr; }

private:
	U **m_ppGlobalPointer;
};

//-----------------------------------------------------------------------------
// Keeps a global pointer of interface type I up to date for a game system base
// class T whose derived classes have their own factories. It is not in the
// factory list and creates nothing: T::SetGameSystemGlobalPtrs forwards to its
// SetGlobalPtr. The game has one, a static object for
// CGameSystemAbstractFactory< CVScriptGameSystem, IVScriptGameSystem >, which
// CVScriptGameSystem and CCSGOVScriptGameSystem call.
//-----------------------------------------------------------------------------
template < class T, class I >
class CGameSystemAbstractFactory : public IGameSystemFactory
{
public:
	explicit CGameSystemAbstractFactory( I **ppGlobalPointer ) : m_ppGlobalPointer( ppGlobalPointer ) {}

	bool Init() override { return false; }
	void PostInit() override {}
	void Shutdown() override {}
	IGameSystem *CreateGameSystem() override { return nullptr; }
	void DestroyGameSystem( IGameSystem *pGameSystem ) override {}
	bool ShouldAutoAdd() const override { return false; }
	int GetPriority() const override { return 0; }

	void SetGlobalPtr( void *pValue ) override
	{
		if ( m_ppGlobalPointer )
		{
			*m_ppGlobalPointer = reinterpret_cast< T * >( pValue );
		}
	}

	bool IsReallocating() const override { return false; }
	IGameSystem *GetStaticGameSystem() override { return nullptr; }

private:
	I **m_ppGlobalPointer;
};

struct AddedGameSystem_t
{
	IGameSystem *m_pGameSystem;
	int m_nPriority;
	int m_nInsertionOrder;
};

abstract_class IGameSystemEventDispatcher
{
public:
	virtual void RegisterListener( GameSystemEventId_t id, IGameSystem *pSystem ) = 0;
	virtual void UnregisterListener( IGameSystem *pSystem ) = 0;
};

class CGameSystemEventDispatcher : public IGameSystemEventDispatcher
{
public:
	CGameSystemEventDispatcher( GameSystemListeners_t *pListeners ) : m_funcListeners( pListeners ) {}

	GameSystemListeners_t &GetListenersRef() { Assert( m_funcListeners ); return *m_funcListeners; }

	void RegisterListener( GameSystemEventId_t id, IGameSystem *pSystem ) override { GetListenersRef().InsertAfter( id, {pSystem} ); }
	void UnregisterListener( IGameSystem *pSystem ) override { for ( auto &it : GetListenersRef() ) if ( it.FindAndRemove( pSystem ) ) break; }

private:
	GameSystemListeners_t *m_funcListeners;
};


#endif // IGAMESYSTEMFACTORY_H
