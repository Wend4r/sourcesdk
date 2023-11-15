//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef KEYVALUES_H
#define KEYVALUES_H

#ifdef _WIN32
#pragma once
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#include <vstdlib/IKeyValuesSystem.h>

#include "platform.h"
#include "utlvector.h"
#include "color.h"
#include "exprevaluator.h"


#define FOR_EACH_SUBKEY( kvRoot, kvSubKey ) \
	for ( KeyValues * kvSubKey = kvRoot->GetFirstSubKey(); kvSubKey != NULL; kvSubKey = kvSubKey->GetNextKey() )

#define FOR_EACH_TRUE_SUBKEY( kvRoot, kvSubKey ) \
	for ( KeyValues * kvSubKey = kvRoot->GetFirstTrueSubKey(); kvSubKey != NULL; kvSubKey = kvSubKey->GetNextTrueSubKey() )

#define FOR_EACH_VALUE( kvRoot, kvValue ) \
	for ( KeyValues * kvValue = kvRoot->GetFirstValue(); kvValue != NULL; kvValue = kvValue->GetNextValue() )


class IBaseFileSystem;
class IFileSystem;
class CUtlBuffer;
class Color;
class CKeyValuesTokenReader;
class KeyValues;
class IKeyValuesDumpContext;
class IKeyValuesSystem;
class IKeyValuesErrorSpew;
typedef void * FileHandle_t;
class CKeyValuesGrowableStringTable;


// single byte identifies a xbox kv file in binary format
// strings are pooled from a searchpath/zip mounted symbol table
#define KV_BINARY_POOLED_FORMAT 0xAA


#define FOR_EACH_SUBKEY( kvRoot, kvSubKey ) \
	for ( KeyValues * kvSubKey = kvRoot->GetFirstSubKey(); kvSubKey != NULL; kvSubKey = kvSubKey->GetNextKey() )

#define FOR_EACH_TRUE_SUBKEY( kvRoot, kvSubKey ) \
	for ( KeyValues * kvSubKey = kvRoot->GetFirstTrueSubKey(); kvSubKey != NULL; kvSubKey = kvSubKey->GetNextTrueSubKey() )

#define FOR_EACH_VALUE( kvRoot, kvValue ) \
	for ( KeyValues * kvValue = kvRoot->GetFirstValue(); kvValue != NULL; kvValue = kvValue->GetNextValue() )
DECLARE_POINTER_HANDLE( HTemporaryKeyValueAllocationScope );

class CTemporaryKeyValues
{
	CTemporaryKeyValues() : m_pKeyValues(nullptr), m_hScope() {}
	~CTemporaryKeyValues()
	{
		// GAMMACASE: TODO: Complete with actual KeyValuesSystem call once it's reversed too.
#if 0
		KeyValuesSystem()->ReleaseTemporaryAllocationScope( m_hScope );
#endif
	}

private:
	KeyValues *m_pKeyValues;
	HTemporaryKeyValueAllocationScope m_hScope;
};

class CKeyValues_Data
{
public:
	// Data type
	enum types_t
	{
		TYPE_NONE = 0,
		TYPE_STRING,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_PTR,
		TYPE_WSTRING,
		TYPE_COLOR,
		TYPE_UINT64,
		TYPE_COMPILED_INT_BYTE,			// hack to collapse 1 byte ints in the compiled format
		TYPE_COMPILED_INT_0,			// hack to collapse 0 in the compiled format
		TYPE_COMPILED_INT_1,			// hack to collapse 1 in the compiled format
		TYPE_NUMTYPES,
	};

private:
	DLL_CLASS_IMPORT IKeyValuesSystem *KVSystem() const;

	DLL_CLASS_IMPORT Color ResolveColorValue() const;
	DLL_CLASS_IMPORT float ResolveFloatValue() const;
	DLL_CLASS_IMPORT int ResolveIntValue() const;
	DLL_CLASS_IMPORT uint64 ResolveUint64Value() const;
	DLL_CLASS_IMPORT const char *ResolveStringValue() const;
	DLL_CLASS_IMPORT const wchar_t *ResolveWStringValue() const;
	DLL_CLASS_IMPORT KeyValues *ResolveSubKeyValue() const;

	DLL_CLASS_IMPORT void Internal_ClearAll();
	DLL_CLASS_IMPORT void Internal_ClearData();

	DLL_CLASS_IMPORT void Internal_CopyData( const CKeyValues_Data &kv );
	DLL_CLASS_IMPORT void Internal_CopyName( const CKeyValues_Data &kv );

	DLL_CLASS_IMPORT bool Internal_IsEqual( CKeyValues_Data *pOther );

	DLL_CLASS_IMPORT const char *Internal_GetName() const;

	DLL_CLASS_IMPORT KeyValues *Internal_GetSubKey() const;
	DLL_CLASS_IMPORT void Internal_SetSubKey( KeyValues *subkey );

	DLL_CLASS_IMPORT HKeySymbol Internal_GetNameSymbol() const;
	DLL_CLASS_IMPORT HKeySymbol Internal_GetNameSymbolCaseSensitive() const;

	DLL_CLASS_IMPORT void Internal_SetName( char const *szName );
	DLL_CLASS_IMPORT void Internal_SetNameFrom( CKeyValues_Data const &pOther );

	DLL_CLASS_IMPORT Color Internal_GetColor( Color defaultClr ) const;
	DLL_CLASS_IMPORT float Internal_GetFloat() const;
	DLL_CLASS_IMPORT int Internal_GetInt() const;
	DLL_CLASS_IMPORT uint64 Internal_GetUint64() const;
	DLL_CLASS_IMPORT char const *Internal_GetString( const char *defaultValue, char *szBuf, size_t maxlen );
	DLL_CLASS_IMPORT const wchar_t *Internal_GetWString( const wchar_t *defaultValue, wchar_t *szBuf, size_t maxlen );
	DLL_CLASS_IMPORT void *Internal_GetPtr() const;
	
	DLL_CLASS_IMPORT void Internal_SetColor( Color value );
	DLL_CLASS_IMPORT void Internal_SetFloat( float value );
	DLL_CLASS_IMPORT void Internal_SetInt( int value );
	DLL_CLASS_IMPORT void Internal_SetUint64( uint64 value );
	DLL_CLASS_IMPORT void Internal_SetString( const char *value );
	DLL_CLASS_IMPORT void Internal_SetWString( const wchar_t *value );
	DLL_CLASS_IMPORT void Internal_SetPtr( void *value );

	DLL_CLASS_IMPORT void Internal_SetFromString( types_t type, const char *value );

	DLL_CLASS_IMPORT bool Internal_HasEscapeSequences() const;
	DLL_CLASS_IMPORT void Internal_SetHasEscapeSequences( bool state );

private:
	union
	{
		int m_iValue;
		float m_flValue;
		void *m_pValue;
		unsigned char m_Color[4];
		uintp m_uValue;
		const char *m_sValue;
		const wchar_t *m_wsValue;
		KeyValues *m_pSub;
	};

	void *m_pUnk;

	uint32 m_iKeyNameCaseSensitive : 24;
	uint32 m_iDataType : 3;
	uint32 m_bHasEscapeSequences : 1;
	uint32 m_bAllocatedExternalMemory : 1;
	uint32 m_bKeySymbolCaseSensitiveMatchesCaseInsensitive : 1;
	uint32 m_bStoredSubKey : 1;
};

//-----------------------------------------------------------------------------
// Purpose: Simple recursive data access class
//			Used in vgui for message parameters and resource files
//			Destructor deletes all child KeyValues nodes
//			Data is stored in key (string names) - (string/int/float)value pairs called nodes.
//
//	About KeyValues Text File Format:

//	It has 3 control characters '{', '}' and '"'. Names and values may be quoted or
//	not. The quote '"' character must not be used within name or values, only for
//	quoting whole tokens. You may use escape sequences wile parsing and add within a
//	quoted token a \" to add quotes within your name or token. When using Escape
//	Sequence the parser must now that by setting KeyValues::UsesEscapeSequences( true ),
//	which it's off by default. Non-quoted tokens ends with a whitespace, '{', '}' and '"'.
//	So you may use '{' and '}' within quoted tokens, but not for non-quoted tokens.
//  An open bracket '{' after a key name indicates a list of subkeys which is finished
//  with a closing bracket '}'. Subkeys use the same definitions recursively.
//  Whitespaces are space, return, newline and tabulator. Allowed Escape sequences
//	are \n, \t, \\, \n and \". The number character '#' is used for macro purposes 
//	(eg #include), don't use it as first character in key names.
//-----------------------------------------------------------------------------
class KeyValues : public CKeyValues_Data
{
	friend class CKeyValuesTokenReader;

public:
	//	By default, the KeyValues class uses a string table for the key names that is
	//	limited to 4MB. The game will exit in error if this space is exhausted. In
	//	general this is preferable for game code for performance and memory fragmentation
	//	reasons.
	//
	//	If this is not acceptable, you can use this call to switch to a table that can grow
	//	arbitrarily. This call must be made before any KeyValues objects are allocated or it
	//	will result in undefined behavior. If you use the growable string table, you cannot
	//	share KeyValues pointers directly with any other module. You can serialize them across
	//	module boundaries. These limitations are acceptable in the Steam backend code 
	//	this option was written for, but may not be in other situations. Make sure to
	//	understand the implications before using this.
	static void SetUseGrowableStringTable( bool bUseGrowableTable );

	DLL_CLASS_IMPORT explicit KeyValues( const char *setName, IKeyValuesSystem *customSystem = NULL, bool ownsCustomSystem = false );

	//
	// AutoDelete class to automatically free the keyvalues.
	// Simply construct it with the keyvalues you allocated and it will free them when falls out of scope.
	// When you decide that keyvalues shouldn't be deleted call Assign(NULL) on it.
	// If you constructed AutoDelete(NULL) you can later assign the keyvalues to be deleted with Assign(pKeyValues).
	//
	class AutoDelete
	{
	public:
		explicit inline AutoDelete( KeyValues *pKeyValues ) : m_pKeyValues( pKeyValues ) {}
		explicit inline AutoDelete( const char *pchKVName ) : m_pKeyValues( new KeyValues( pchKVName ) ) {}
		inline ~AutoDelete( void ) { delete m_pKeyValues; }
		inline void Assign( KeyValues *pKeyValues ) { m_pKeyValues = pKeyValues; }
		/// behaves more like an auto pointer detach ( flags itself to not delete the contained object, and returns a pointer to it)
		inline KeyValues * Detach() { KeyValues *retval = m_pKeyValues; Assign( NULL ); return retval; }
		KeyValues *operator->()	{ return m_pKeyValues; }
		operator KeyValues *()	{ return m_pKeyValues; }
	private:
		AutoDelete( AutoDelete const &x ); // forbid
		AutoDelete & operator= ( AutoDelete const &x ); // forbid
	protected:
		KeyValues *m_pKeyValues;
	};

	//
	// AutoDeleteInline is useful when you want to hold your keyvalues object inside
	// and delete it right after using.
	// You can also pass temporary KeyValues object as an argument to a function by wrapping it into KeyValues::AutoDeleteInline
	// instance:   call_my_function( KeyValues::AutoDeleteInline( new KeyValues( "test" ) ) )
	//
	class AutoDeleteInline : public AutoDelete
	{
	public:
		explicit inline AutoDeleteInline( KeyValues *pKeyValues ) : AutoDelete( pKeyValues ) {}
		inline operator KeyValues *() const { return m_pKeyValues; }
		inline KeyValues * Get() const { return m_pKeyValues; }
	};

	// Quick setup constructors
	DLL_CLASS_IMPORT KeyValues( const char *setName, const char *firstKey, const char *firstValue );
	DLL_CLASS_IMPORT KeyValues( const char *setName, const char *firstKey, const wchar_t *firstValue );
	DLL_CLASS_IMPORT KeyValues( const char *setName, const char *firstKey, int firstValue );
	DLL_CLASS_IMPORT KeyValues( const char *setName, const char *firstKey, const char *firstValue, const char *secondKey, const char *secondValue );
	DLL_CLASS_IMPORT KeyValues( const char *setName, const char *firstKey, int firstValue, const char *secondKey, int secondValue );

	DLL_CLASS_IMPORT ~KeyValues();

	// Section name
	DLL_CLASS_IMPORT const char *GetName() const;
	DLL_CLASS_IMPORT void SetName( const char *setName );

	// gets the name as a unique int
	DLL_CLASS_IMPORT HKeySymbol GetNameSymbol() const;
	DLL_CLASS_IMPORT HKeySymbol GetNameSymbolCaseSensitive() const;

	// File access. Set UsesEscapeSequences true, if resource file/buffer uses Escape Sequences (eg \n, \t)
	DLL_CLASS_IMPORT void UsesEscapeSequences( bool state ); // default false
	DLL_CLASS_IMPORT bool LoadFromFile( IFileSystem *filesystem, const char *resourceName, const char *pathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL, void *pUnk1 = NULL, const char *pUnk2 = NULL );
	DLL_CLASS_IMPORT bool SaveToFile( IFileSystem *filesystem, const char *resourceName, const char *pathID = NULL, bool bAllowEmptyString = false );
	// Read from a buffer...  Note that the buffer must be null terminated
	DLL_CLASS_IMPORT bool LoadFromBuffer( char const *resourceName, const char *pBuffer, IFileSystem *pFileSystem = NULL, const char *pPathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL, IKeyValuesErrorSpew *pErrorSpew = NULL, void *pUnk1 = NULL, const char *pUnk2 = NULL );

	// Read from a utlbuffer...
	DLL_CLASS_IMPORT bool LoadFromBuffer( char const *resourceName, CUtlBuffer &buf, IFileSystem *pFileSystem = NULL, const char *pPathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL, IKeyValuesErrorSpew *pErrorSpew = NULL, void *pUnk1 = NULL, const char *pUnk2 = NULL );

	DLL_CLASS_IMPORT CTemporaryKeyValues *LoadTemporaryFromBuffer( bool, char const *resourceName, const char *pBuffer, IFileSystem *pFileSystem = NULL, const char *pPathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL, IKeyValuesErrorSpew *pErrorSpew = NULL, void *pUnk1 = NULL, const char *pUnk2 = NULL );
	DLL_CLASS_IMPORT CTemporaryKeyValues *LoadTemporaryFromBuffer( bool, char const *resourceName, CUtlBuffer &buf, IFileSystem *pFileSystem = NULL, const char *pPathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL, void *pUnk1 = NULL, const char *pUnk2 = NULL );

	DLL_CLASS_IMPORT CTemporaryKeyValues *LoadTemporaryFromFile( bool, IFileSystem *filesystem, const char *resourceName, const char *pathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL, void *pUnk1 = NULL, const char *pUnk2 = NULL );

	// Find a keyValue, create it if it is not found.
	// Set bCreate to true to create the key if it doesn't already exist (which ensures a valid pointer will be returned)
	DLL_CLASS_IMPORT KeyValues *FindKey( HKeySymbol keySymbol ) const;
	DLL_CLASS_IMPORT const KeyValues *FindKey( const char *keyName ) const;
	DLL_CLASS_IMPORT KeyValues *FindKey( const char *keyName, bool bCreate );
	DLL_CLASS_IMPORT KeyValues *FindKeyAndParent( const char *keyName, KeyValues **pParent, bool );
	DLL_CLASS_IMPORT bool FindAndDeleteSubKey( const char *keyName );

	DLL_CLASS_IMPORT KeyValues* CreateKey( const char *keyName );
	DLL_CLASS_IMPORT KeyValues *CreateNewKey();		// creates a new key, with an autogenerated name.  name is guaranteed to be an integer, of value 1 higher than the highest other integer key name
	DLL_CLASS_IMPORT KeyValues *CreatePeerKey( const char *keyName );

	DLL_CLASS_IMPORT KeyValues *AddKey( const char *keyName );
	DLL_CLASS_IMPORT void AddSubKey( KeyValues *pSubkey );	// Adds a subkey. Make sure the subkey isn't a child of some other keyvalues

	DLL_CLASS_IMPORT KeyValues *CreateKeyUsingKnownLastChild( const char *keyName, KeyValues *pChild );

	DLL_CLASS_IMPORT void RemoveSubKey( KeyValues *subKey, bool bDelete = true, bool bUnk = false );	// removes a subkey from the list, DOES NOT DELETE IT
	DLL_CLASS_IMPORT void RemoveOptionalSubKey( KeyValues *subKey );
	DLL_CLASS_IMPORT void InsertSubKey( int nIndex, KeyValues *pSubKey ); // Inserts the given sub-key before the Nth child location
	DLL_CLASS_IMPORT bool ContainsSubKey( KeyValues *pSubKey ); // Returns true if this key values contains the specified sub key, false otherwise.
	DLL_CLASS_IMPORT void SwapSubKey( KeyValues *pExistingSubKey, KeyValues *pNewSubKey );	// Swaps an existing subkey for a new one, DOES NOT DELETE THE OLD ONE but takes ownership of the new one
	DLL_CLASS_IMPORT void ElideSubKey( KeyValues *pSubKey );	// Removes a subkey but inserts all of its children in its place, in-order (flattens a tree, like firing a manager!)

	// Key iteration.
	//
	// NOTE: GetFirstSubKey/GetNextKey will iterate keys AND values. Use the functions 
	// below if you want to iterate over just the keys or just the values.
	//
	DLL_CLASS_IMPORT KeyValues *GetFirstSubKey() const;	// returns the first subkey in the list
	DLL_CLASS_IMPORT KeyValues *FindLastSubKey() const;
	DLL_CLASS_IMPORT KeyValues *GetNextKey() const;		// returns the next subkey
	DLL_CLASS_IMPORT void SetNextKey( KeyValues *pDat );

	DLL_CLASS_IMPORT KeyValues *FindLastSubKey();	// returns the LAST subkey in the list.  This requires a linked list iteration to find the key.  Returns NULL if we don't have any children

	bool BInteriorNode() const
	{
		return GetFirstSubKey() != NULL;
	}
	bool BLeafNode() const
	{
		return GetFirstSubKey() == NULL;
	}
	//
	// These functions can be used to treat it like a true key/values tree instead of 
	// confusing values with keys.
	//
	// So if you wanted to iterate all subkeys, then all values, it would look like this:
	//     for ( KeyValues *pKey = pRoot->GetFirstTrueSubKey(); pKey; pKey = pKey->GetNextTrueSubKey() )
	//     {
	//		   Msg( "Key name: %s\n", pKey->GetName() );
	//     }
	//     for ( KeyValues *pValue = pRoot->GetFirstValue(); pKey; pKey = pKey->GetNextValue() )
	//     {
	//         Msg( "Int value: %d\n", pValue->GetInt() );  // Assuming pValue->GetDataType() == TYPE_INT...
	//     }
	DLL_CLASS_IMPORT KeyValues* GetFirstTrueSubKey() const;
	DLL_CLASS_IMPORT KeyValues* GetNextTrueSubKey() const;

	DLL_CLASS_IMPORT KeyValues* GetFirstValue() const;	// When you get a value back, you can use GetX and pass in NULL to get the value.
	DLL_CLASS_IMPORT KeyValues* GetNextValue() const;

	// Data access
	DLL_CLASS_IMPORT int GetInt( const char *keyName = NULL, int defaultValue = 0 ) const;
	DLL_CLASS_IMPORT uint64 GetUint64( const char *keyName = NULL, uint64 defaultValue = 0 ) const;
	DLL_CLASS_IMPORT float GetFloat( const char *keyName = NULL, float defaultValue = 0.0f ) const;
	DLL_CLASS_IMPORT const char *GetString( const char *keyName = NULL, const char *defaultValue = "", char *pszOut = NULL, size_t maxlen = 0 );
	DLL_CLASS_IMPORT const wchar_t *GetWString( const char *keyName = NULL, const wchar_t *defaultValue = L"", char *pszOut = NULL, size_t maxlen = 0 );
	DLL_CLASS_IMPORT void *GetPtr( const char *keyName = NULL, void *defaultValue = (void *)0 ) const;
	DLL_CLASS_IMPORT Color GetColor( const char *keyName = NULL, const Color &defaultColor = Color( 0, 0, 0, 0 ) ) const;
	DLL_CLASS_IMPORT bool GetBool( const char *keyName = NULL, bool defaultValue = false ) const { return GetInt( keyName, defaultValue ? 1 : 0 ) ? true : false; }
	DLL_CLASS_IMPORT bool IsEmpty( const char *keyName = NULL ) const;

	// Data access
	int GetInt( HKeySymbol keySymbol, int defaultValue = 0 ) const;
	uint64 GetUint64( HKeySymbol keySymbol, uint64 defaultValue = 0 ) const;
	float GetFloat( HKeySymbol keySymbol, float defaultValue = 0.0f ) const;
	const char *GetString( HKeySymbol keySymbol, const char *defaultValue = "");
	const wchar_t *GetWString( HKeySymbol keySymbol, const wchar_t *defaultValue = L"");
	void *GetPtr( HKeySymbol keySymbol, void *defaultValue = (void *)0 ) const;
	Color GetColor( HKeySymbol keySymbol, const Color &defaultColor = Color( 0, 0, 0, 0 ) ) const;
	bool GetBool( HKeySymbol keySymbol, bool defaultValue = false ) const { return GetInt( keySymbol, defaultValue ? 1 : 0 ) ? true : false; }
	bool IsEmpty( HKeySymbol keySymbol ) const;

	// Key writing
	DLL_CLASS_IMPORT void SetWString( const char *keyName, const wchar_t *value );
	DLL_CLASS_IMPORT void SetString( const char *keyName, const char *value );
	DLL_CLASS_IMPORT void SetInt( const char *keyName, int value );
	DLL_CLASS_IMPORT void SetUint64( const char *keyName, uint64 value );
	DLL_CLASS_IMPORT void SetFloat( const char *keyName, float value );
	DLL_CLASS_IMPORT void SetPtr( const char *keyName, void *value );
	DLL_CLASS_IMPORT void SetColor( const char *keyName, Color value);
	DLL_CLASS_IMPORT void SetBool( const char *keyName, bool value ) { SetInt( keyName, value ? 1 : 0 ); }

	// Memory allocation (optimized)
	DLL_CLASS_IMPORT void *operator new( size_t iAllocSize );
	DLL_CLASS_IMPORT void *operator new( size_t iAllocSize, int nBlockUse, const char *pFileName, int nLine );
	DLL_CLASS_IMPORT void operator delete( void *pMem );
	DLL_CLASS_IMPORT void operator delete( void *pMem, int nBlockUse, const char *pFileName, int nLine );

	DLL_CLASS_IMPORT KeyValues& operator=( KeyValues& src );

	DLL_CLASS_IMPORT bool IsEqual( KeyValues *pRHS );

	// Adds a chain... if we don't find stuff in this keyvalue, we'll look
	// in the one we're chained to.
	DLL_CLASS_IMPORT void ChainKeyValue( KeyValues* pChain );
	
	DLL_CLASS_IMPORT void RecursiveSaveToFile( CUtlBuffer& buf, int indentLevel );

	DLL_CLASS_IMPORT bool WriteAsBinary( CUtlBuffer &buffer ) const;
	DLL_CLASS_IMPORT bool ReadAsBinary( CUtlBuffer &buffer, int nStackDepth = 0 );

	// Same as the other binary functions, but filter out and remove empty keys (like when seralizing to a file )
	DLL_CLASS_IMPORT bool WriteAsBinaryFiltered( CUtlBuffer &buffer );
	DLL_CLASS_IMPORT bool ReadAsBinaryFiltered( CUtlBuffer &buffer, int nStackDepth = 0 );

	// Allocate & create a new copy of the keys
	DLL_CLASS_IMPORT KeyValues *MakeCopy( void ) const;

	// Make a new copy of all subkeys, add them all to the passed-in keyvalues
	DLL_CLASS_IMPORT void CopySubkeys( KeyValues *pParent ) const;

	// Clear out all subkeys, and the current value
	DLL_CLASS_IMPORT void Clear( void );

	// Data type
	enum types_t
	{
		TYPE_NONE = 0,
		TYPE_STRING,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_PTR,
		TYPE_WSTRING,
		TYPE_COLOR,
		TYPE_UINT64,
		TYPE_COMPILED_INT_BYTE,			// hack to collapse 1 byte ints in the compiled format
		TYPE_COMPILED_INT_0,			// hack to collapse 0 in the compiled format
		TYPE_COMPILED_INT_1,			// hack to collapse 1 in the compiled format
		TYPE_NUMTYPES, 
	};
	DLL_CLASS_IMPORT types_t GetDataType(const char *keyName = NULL);
	types_t GetDataType() const;

	// for backward compat
	void deleteThis();

	void SetStringValue( char const *strValue );

	// unpack a key values list into a structure
	DLL_CLASS_IMPORT void UnpackIntoStructure( struct KeyValuesUnpackStructure const *pUnpackTable, void *pDest );

	// Process conditional keys for widescreen support.
	DLL_CLASS_IMPORT bool ProcessResolutionKeys( const char *pResString );

	// Dump keyvalues recursively into a dump context
	DLL_CLASS_IMPORT bool Dump( IKeyValuesDumpContext *pDump, int nIndentLevel = 0 );

	// Merge operations describing how two keyvalues can be combined
	enum MergeKeyValuesOp_t
	{
		MERGE_KV_ALL,
		MERGE_KV_UPDATE,	// update values are copied into storage, adding new keys to storage or updating existing ones
		MERGE_KV_DELETE,	// update values specify keys that get deleted from storage
		MERGE_KV_BORROW,	// update values only update existing keys in storage, keys in update that do not exist in storage are discarded
	};
	DLL_CLASS_IMPORT void MergeFrom( KeyValues *kvMerge, MergeKeyValuesOp_t eOp = MERGE_KV_ALL );

	// Assign keyvalues from a string
	DLL_CLASS_IMPORT static KeyValues * FromString( char const *szName, char const *szStringVal, char const **ppEndOfParse = NULL );
		
	/// Create a child key, given that we know which child is currently the last child.
	/// This avoids the O(N^2) behaviour when adding children in sequence to KV,
	/// when CreateKey() wil have to re-locate the end of the list each time.  This happens,
	/// for example, every time we load any KV file whatsoever.

	DLL_CLASS_IMPORT KeyValues* CreateKeyUsingKnownLastChild( const char *keyName, KeyValues *pLastChild, IKeyValuesSystem *pKVSystem = NULL );
	DLL_CLASS_IMPORT void AddSubkeyUsingKnownLastChild( KeyValues *pSubKey, KeyValues *pLastChild );

private:
	uint32 m_iKeyName : 24;	// keyname is a symbol defined in KeyValuesSystem
	uint32 m_iKeyNameCaseSensitive1 : 8;	// 1st part of case sensitive symbol defined in KeyValueSystem

	// These are needed out of the union because the API returns string pointers
	char *m_sValue;
	wchar_t *m_wsValue;

	// we don't delete these
	union
	{
		int m_iValue;
		float m_flValue;
		void *m_pValue;
		unsigned char m_Color[4];
	};
	
	char	   m_iDataType;
	char	   m_bHasEscapeSequences; // true, if while parsing this KeyValue, Escape Sequences are used (default false)
	uint16	   m_iKeyNameCaseSensitive2;	// 2nd part of case sensitive symbol defined in KeyValueSystem;

	IKeyValuesSystem *m_pKeyValuesSystem;
	bool m_bOwnsCustomKeyValuesSystem;

	KeyValues *m_pPeer;	// pointer to next key in list
	KeyValues *m_pSub;	// pointer to Start of a new sub key list
	KeyValues *m_pChain;// Search here if it's not in our list
	
	GetSymbolProc_t	m_pExpressionGetSymbolProc;

private:
	// Statics to implement the optional growable string table
	// Function pointers that will determine which mode we are in
	static int (*s_pfGetSymbolForString)( const char *name, bool bCreate );
	static const char *(*s_pfGetStringForSymbol)( int symbol );
	static CKeyValuesGrowableStringTable *s_pGrowableStringTable;

public:
	// Functions that invoke the default behavior
	static int GetSymbolForStringClassic( const char *name, bool bCreate = true );
	static const char *GetStringForSymbolClassic( int symbol );
	
	// Functions that use the growable string table
	static int GetSymbolForStringGrowable( const char *name, bool bCreate = true );
	static const char *GetStringForSymbolGrowable( int symbol );
};

typedef KeyValues::AutoDelete KeyValuesAD;

enum KeyValuesUnpackDestinationTypes_t
{
	UNPACK_TYPE_FLOAT,										// dest is a float
	UNPACK_TYPE_VECTOR,										// dest is a Vector
	UNPACK_TYPE_VECTOR_COLOR,								// dest is a vector, src is a color
	UNPACK_TYPE_STRING,										// dest is a char *. unpacker will allocate.
	UNPACK_TYPE_INT,										// dest is an int
	UNPACK_TYPE_FOUR_FLOATS,	 // dest is an array of 4 floats. source is a string like "1 2 3 4"
	UNPACK_TYPE_TWO_FLOATS,		 // dest is an array of 2 floats. source is a string like "1 2"
};

#define UNPACK_FIXED( kname, kdefault, dtype, ofs ) { kname, kdefault, dtype, ofs, 0 }
#define UNPACK_VARIABLE( kname, kdefault, dtype, ofs, sz ) { kname, kdefault, dtype, ofs, sz }
#define UNPACK_END_MARKER { NULL, NULL, UNPACK_TYPE_FLOAT, 0 }

struct KeyValuesUnpackStructure
{
	char const *m_pKeyName;									// null to terminate tbl
	char const *m_pKeyDefault;								// null ok
	KeyValuesUnpackDestinationTypes_t m_eDataType;			// UNPACK_TYPE_INT, ..
	size_t m_nFieldOffset;									// use offsetof to set
	size_t m_nFieldSize;									// for strings or other variable length
};


//-----------------------------------------------------------------------------
// inline methods
//-----------------------------------------------------------------------------
inline int KeyValues::GetInt( HKeySymbol keySymbol, int defaultValue ) const
{
	KeyValues *dat = FindKey( keySymbol );
	return dat ? dat->GetInt( (const char *)NULL, defaultValue ) : defaultValue;
}

inline uint64 KeyValues::GetUint64( HKeySymbol keySymbol, uint64 defaultValue ) const
{
	KeyValues *dat = FindKey( keySymbol );
	return dat ? dat->GetUint64( (const char *)NULL, defaultValue ) : defaultValue;
}

inline float KeyValues::GetFloat( HKeySymbol keySymbol, float defaultValue ) const
{
	KeyValues *dat = FindKey( keySymbol );
	return dat ? dat->GetFloat( (const char *)NULL, defaultValue ) : defaultValue;
}

inline const char *KeyValues::GetString( HKeySymbol keySymbol, const char *defaultValue )
{
	KeyValues *dat = FindKey( keySymbol );
	return dat ? dat->GetString( (const char *)NULL, defaultValue ) : defaultValue;
}

inline const wchar_t *KeyValues::GetWString( HKeySymbol keySymbol, const wchar_t *defaultValue )
{
	KeyValues *dat = FindKey( keySymbol );
	return dat ? dat->GetWString( (const char *)NULL, defaultValue ) : defaultValue;
}

inline void *KeyValues::GetPtr( HKeySymbol keySymbol, void *defaultValue ) const
{
	KeyValues *dat = FindKey( keySymbol );
	return dat ? dat->GetPtr( (const char *)NULL, defaultValue ) : defaultValue;
}

inline Color KeyValues::GetColor( HKeySymbol keySymbol, const Color &defaultColor ) const
{
	KeyValues *dat = FindKey( keySymbol );
	return dat ? dat->GetColor( (const char *)NULL, defaultColor ) : defaultColor;
}

inline bool KeyValues::IsEmpty( HKeySymbol keySymbol ) const
{
	KeyValues *dat = FindKey( keySymbol );
	return dat ? dat->IsEmpty() : true;
}

inline KeyValues::types_t KeyValues::GetDataType( void ) const
{
	return (types_t)m_iDataType;
}

// for backwards compat - we used to need this to force the free to run from the same DLL
// as the alloc
inline void KeyValues::deleteThis()
{ 
	delete this; 
}

inline void KeyValues::SetStringValue( char const *strValue )
{
	// delete the old value
	delete [] m_sValue;
	// make sure we're not storing the WSTRING  - as we're converting over to STRING
	delete [] m_wsValue;
	m_wsValue = NULL;

	if (!strValue)
	{
		// ensure a valid value
		strValue = "";
	}

	// allocate memory for the new value and copy it in
	size_t len = V_strlen( strValue );
	m_sValue = new char[len + 1];
	V_memcpy( m_sValue, strValue, len+1 );

	m_iDataType = TYPE_STRING;
}

//
// KeyValuesDumpContext and generic implementations
//

class IKeyValuesDumpContext
{
public:
	virtual bool KvBeginKey( KeyValues *pKey, int nIndentLevel ) = 0;
	virtual bool KvWriteValue( KeyValues *pValue, int nIndentLevel ) = 0;
	virtual bool KvEndKey( KeyValues *pKey, int nIndentLevel ) = 0;
};

class IKeyValuesDumpContextAsText : public IKeyValuesDumpContext
{
public:
	virtual bool KvBeginKey( KeyValues *pKey, int nIndentLevel );
	virtual bool KvWriteValue( KeyValues *pValue, int nIndentLevel );
	virtual bool KvEndKey( KeyValues *pKey, int nIndentLevel );

public:
	virtual bool KvWriteIndent( int nIndentLevel );
	virtual bool KvWriteText( char const *szText ) = 0;
};

class CKeyValuesDumpContextAsDevMsg : public IKeyValuesDumpContextAsText
{
public:
	// Overrides developer level to dump in DevMsg, zero to dump as Msg
	CKeyValuesDumpContextAsDevMsg( int nDeveloperLevel = 1 ) : m_nDeveloperLevel( nDeveloperLevel ) {}

public:
	virtual bool KvBeginKey( KeyValues *pKey, int nIndentLevel );
	virtual bool KvWriteText( char const *szText );

protected:
	int m_nDeveloperLevel;
};

inline bool KeyValuesDumpAsDevMsg( KeyValues *pKeyValues, int nIndentLevel = 0, int nDeveloperLevel = 1 )
{
	CKeyValuesDumpContextAsDevMsg ctx( nDeveloperLevel );
	return pKeyValues->Dump( &ctx, nIndentLevel );
}


#endif // KEYVALUES_H
