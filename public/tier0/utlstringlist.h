#ifndef UTLSTRINGLIST_H
#define UTLSTRINGLIST_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/bufferstring.h"
#include "tier1/utlvector.h"

// Base class for vectors owning dynamically allocated C strings.
template < class VectorType >
class CUtlStringList_Base : public VectorType
{
public:
	CUtlStringList_Base() {}

	~CUtlStringList_Base()
	{
		PurgeAndDeleteElements();
	}

	void PurgeAndFreeElements()
	{
		for ( int i = 0; i < this->Count(); ++i )
		{
			free( this->Element( i ) );
		}

		this->Purge();
	}

	void PurgeAndDeleteElements()
	{
		for ( int i = 0; i < this->Count(); ++i )
		{
			delete[] this->Element( i );
		}

		this->Purge();
	}

	int CopyAndAddToTail( const char *pString )
	{
		char *pNewString = new char[ strlen( pString ) + 1 ];

		V_strcpy( pNewString, pString );

		return this->AddToTail( pNewString );
	}

	void RemoveAndDelete( int nIndex )
	{
		delete[] this->Element( nIndex );

		this->Remove( nIndex );
	}

	static int __cdecl SortFunc( char * const *pString1, char * const *pString2 )
	{
		return V_strcmp( *pString1, *pString2 );
	}

private:
	CUtlStringList_Base( const CUtlStringList_Base & );
};

// Easy string list class for use with V_SplitString and related functions.
class CUtlStringList : public CUtlStringList_Base< CUtlVector< char * > >
{
public:
	CUtlStringList() {}

	CUtlStringList( const char *pString, const char *pSeparator )
	{
		SplitString( pString, &pSeparator, 1 );
	}

	CUtlStringList( const char *pString, const char **pSeparators, int nSeparators = -1 )
	{
		SplitString( pString, pSeparators, nSeparators );
	}

	PLATFORM_CLASS void SplitString( const char *pString, const char * const *pSeparators, int nSeparators = -1 );

	void SplitString2( const char *pString, const char **pSeparators, int nSeparators = -1 )
	{
		SplitString( pString, pSeparators, nSeparators );
	}

private:
	CUtlStringList( const CUtlStringList & );
};

#endif // UTLSTRINGLIST_H
