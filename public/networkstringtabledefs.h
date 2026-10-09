//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef NETWORKSTRINGTABLEDEFS_H
#define NETWORKSTRINGTABLEDEFS_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "utlstring.h"
#include "tier1/utldelegate.h"
#include "tier1/utlvector.h"

typedef int TABLEID;

#define INVALID_STRING_TABLE -1
const unsigned int INVALID_STRING_INDEX = -1;

// table index is sent in log2(MAX_TABLES) bits
#define MAX_TABLES	32  // Table id is 4 bits

#define INTERFACENAME_NETWORKSTRINGTABLESERVER "Source2EngineToServerStringTable001"
#define INTERFACENAME_NETWORKSTRINGTABLECLIENT "Source2EngineToClientStringTable001"

class StringTableInit_t;
class INetworkStringTable;

struct SetStringUserDataRequest_t
{
	void* m_pRawData;
	unsigned int m_cbDataSize;
};

struct StringUserData_t
{
	unsigned int m_cbDataSize;
	void* m_pRawData;
};

typedef CUtlDelegate< void ( INetworkStringTable *, int, const char *, const SetStringUserDataRequest_t * ) > StringChangedCallback_t;
// Returns false to fall back to a hex dump of the user data
typedef CUtlDelegate< bool ( const char *pString, const SetStringUserDataRequest_t *pUserData, CUtlVector< CUtlString > *pOutLines ) > UserDataFormatterDelegate_t;

//-----------------------------------------------------------------------------
// Purpose: Game .dll shared string table interfaces
//-----------------------------------------------------------------------------
class INetworkStringTable
{
public:

	virtual					~INetworkStringTable( void ) {};
		
	// Table Info
	virtual const char		*GetTableName( void ) const = 0;
	virtual TABLEID			GetTableId( void ) const = 0;
	virtual int				GetNumStrings( void ) const = 0;

	// Networking
	virtual void			SetStringChangeTick( int tick, void *unknown ) = 0;
	virtual int				GetTick( void ) = 0;
	virtual bool			ChangedBetweenTicks(int tickA, int tickB ) const = 0;

	virtual int				AddString( bool bIsServer, const char *value, const SetStringUserDataRequest_t* userdata = 0 ) = 0;

	virtual const char		*GetString( int stringNumber, bool bFailSilent = false ) const = 0;
	virtual bool			SetStringUserData(int stringNumber, const SetStringUserDataRequest_t *userdata, bool bForceOverride) = 0;
	virtual const StringUserData_t* GetStringUserData(int stringNumber) const = 0;
	virtual int				FindStringIndex( char const *string ) = 0; // returns INVALID_STRING_INDEX if not found
	virtual void			SetStringChangedCallback( const StringChangedCallback_t &callback, bool bCallForExistingStrings ) = 0;
	virtual void			SetAllowClientSideAddString( bool state ) = 0;
	virtual void			unk014( bool ) = 0;
	virtual void			unk015( const char *string ) = 0;
	virtual void			unk016( const UserDataFormatterDelegate_t &formatter ) = 0;
};

enum ENetworkStringtableFlags
{
	NSF_NONE = 0,
	NSF_DICTIONARY_ENABLED  = (1<<0), // Uses pre-calculated per map dictionaries to reduce bandwidth
};

class INetworkStringTableContainer: public IAppSystem
{
public:
	
	virtual					~INetworkStringTableContainer( void ) {};
	
	// table creation/destruction
	virtual INetworkStringTable	*CreateStringTable( StringTableInit_t *initClass ) = 0;
	virtual void				RemoveAllTables( void ) = 0;
	
	// table infos
	virtual INetworkStringTable	*FindTable( const char *tableName ) const = 0;
	virtual INetworkStringTable	*GetTable( TABLEID stringTable ) const = 0;
	virtual int					GetNumTables( void ) const = 0;
	virtual const char			*unk017( void ) const = 0;
};

#endif // NETWORKSTRINGTABLEDEFS_H
