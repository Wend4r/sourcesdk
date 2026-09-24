#ifndef SCRIPTKEYVALUES_H
#define SCRIPTKEYVALUES_H

#if _WIN32
#pragma once
#endif

#include "variant.h"

class CEntityKeyValues;
class KeyValues3;

// Script class "CScriptKeyValues": wraps the key values of an entity for the "DispatchSpawn" function of the entity framework script.
// The game creates it on the stack for the duration of the call.
class CScriptKeyValues
{
public:
	CScriptKeyValues( const CEntityKeyValues *pKeyValues ) : m_pKeyValues( pKeyValues ) {}

	// Registers this object with the script VM under its script class description; remove the returned instance before destroying it.
	HSCRIPT RegisterScriptInstance();

	// Script function "GetValue". Returns void for a missing key. Integer keys come back as float64,
	// the other types as KeyValues3ToVariant converts them.
	CVariant GetValue( const char *pszKey );

	// Converts null, bool, integer, double, string and three-double array values; the string is not copied.
	// Returns false and leaves pValue untouched for the types without a variant form.
	static bool KeyValues3ToVariant( const KeyValues3 *pKV, CVariant *pValue );

public:
	const CEntityKeyValues *m_pKeyValues;
};

#endif // SCRIPTKEYVALUES_H
