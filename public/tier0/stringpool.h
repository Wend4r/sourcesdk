//========= Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef STRINGPOOL_H
#define STRINGPOOL_H

#if defined( _WIN32 )
#pragma once
#endif

#include "utlrbtree.h"
#include "utlvector.h"
#include "tier0/utlbuffer.h"
#include "generichash.h"

//-----------------------------------------------------------------------------
// Purpose: Allocates memory for strings, checking for duplicates first,
//			reusing exising strings if duplicate found.
//-----------------------------------------------------------------------------

enum StringPoolCase_t
{
	StringPoolCaseInsensitive,
	StringPoolCaseSensitive
};

class CStringPool
{
public:
	DLL_CLASS_IMPORT CStringPool( StringPoolCase_t caseSensitivity = StringPoolCaseInsensitive );
	DLL_CLASS_IMPORT ~CStringPool();

	DLL_CLASS_IMPORT unsigned int Count() const;

	DLL_CLASS_IMPORT const char * Allocate( const char *pszValue );
	// This feature is deliberately not supported because it's pretty dangerous
	// given current uses of CStringPool, which assume they can copy string pointers without
	// any refcounts.
	//void Free( const char *pszValue );
	DLL_CLASS_IMPORT void FreeAll();

	// searches for a string already in the pool
	DLL_CLASS_IMPORT const char * Find( const char *pszValue );

protected:
	typedef CUtlRBTree<const char *, unsigned short> CStrSet;

	CStrSet m_Strings;
};

//-----------------------------------------------------------------------------
// Purpose: A reference counted string pool.  
//
// Elements are stored more efficiently than in the conventional string pool, 
// quicker to look up, and storage is tracked via reference counts.  
//
// At some point this should replace CStringPool
//-----------------------------------------------------------------------------
// template<class T> // S2: only is unsigned short .
class CCountedStringPool
{
private: // HACK, hash_item_t structure should not be public.
	struct hash_item_t
	{
		char*			pString;
		unsigned short	nNextElement;
		unsigned char	nReferenceCount;
		unsigned char	pad;
	};

	enum
	{
		INVALID_ELEMENT = 0,
		MAX_REFERENCE   = 0xFF,
		HASH_TABLE_SIZE = 1024
	};

	CUtlVector<unsigned short>	m_HashTable;	// Points to each element
	CUtlVector<hash_item_t>		m_Elements;
	unsigned short				m_FreeListStart;
	StringPoolCase_t 			m_caseSensitivity;

public:
	DLL_CLASS_IMPORT CCountedStringPool( StringPoolCase_t caseSensitivity = StringPoolCaseInsensitive );
	DLL_CLASS_IMPORT virtual ~CCountedStringPool();

	DLL_CLASS_IMPORT void			FreeAll();

	DLL_CLASS_IMPORT char			*FindString( const char* pIntrinsic ); 
	DLL_CLASS_IMPORT char			*ReferenceString( const char* pIntrinsic );
	DLL_CLASS_IMPORT void			DereferenceString( const char* pIntrinsic );

	// These are only reliable if there are less than 64k strings in your string pool
	DLL_CLASS_IMPORT unsigned short	FindStringHandle( const char* pIntrinsic ); 
	DLL_CLASS_IMPORT unsigned short	ReferenceStringHandle( const char* pIntrinsic );
	DLL_CLASS_IMPORT char			*HandleToString( unsigned short handle );
	DLL_CLASS_IMPORT void			SpewStrings();
	DLL_CLASS_IMPORT unsigned		Hash( const char *pszKey );

	DLL_CLASS_IMPORT bool			SaveToBuffer( CUtlBuffer &buffer );
	DLL_CLASS_IMPORT bool			RestoreFromBuffer( CUtlBuffer &buffer );

	// Debug helper method to validate that we didn't overflow
	void			VerifyNotOverflowed( unsigned int value )  { Assert( value < 0xffff ); };
};

#endif // STRINGPOOL_H