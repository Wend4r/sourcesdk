#ifndef ENVIRONMENTBLOCK_H
#define ENVIRONMENTBLOCK_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"

class CBufferString;

//-----------------------------------------------------------------------------
// A copy of a process environment: "KEY=value" strings packed into one block,
// with a pointer to each entry and a null pointer after the last one
//-----------------------------------------------------------------------------
class CEnvironmentBlock
{
public:
	DLL_CLASS_IMPORT CEnvironmentBlock();

	DLL_CLASS_IMPORT void CaptureProcessEnvironment();
	DLL_CLASS_IMPORT void Copy( const CEnvironmentBlock &other );

	// Returns the index of the entry or -1, ppValue receives the text after '='
	DLL_CLASS_IMPORT int FindEntry( const char *pKey, const char **ppValue, bool bCaseInsensitive ) const;
	// Returns the whole "KEY=value" entry
	DLL_CLASS_IMPORT const char *GetVarKeyAndValue( int nIndex ) const;
	// Returns the value and copies the key into pKey when it is not null
	DLL_CLASS_IMPORT const char *GetVarKeyAndValue( int nIndex, CBufferString *pKey ) const;
	DLL_CLASS_IMPORT const char *GetVarKey( int nIndex, CBufferString *pKey ) const;
	DLL_CLASS_IMPORT const char *GetVarValue( int nIndex ) const;

	DLL_CLASS_IMPORT bool Remove( const char *pKey );
	// An empty or null value removes the variable
	DLL_CLASS_IMPORT void Set( const char *pKey, const char *pValue );

protected:
	DLL_CLASS_IMPORT void ValidateStorage();

public:
	// Points into m_Storage
	CUtlVector< char * > m_Entries;
	CUtlVector< char > m_Storage;
};

#endif // ENVIRONMENTBLOCK_H
