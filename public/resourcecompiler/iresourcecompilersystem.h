//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Compiles content files into game resources
//
//===========================================================================//

#ifndef IRESOURCECOMPILERSYSTEM_H
#define IRESOURCECOMPILERSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/bufferstring.h"
#include "tier0/utlstring.h"
#include "tier1/utlvector.h"
#include "appframework/iappsystem.h"
#include "tier3/tier3dm.h"

class IResourceCompileFileSystemInterface;
class KeyValues;

abstract_class IResourceCompilerSystem : public IAppSystem
{
public:
	virtual bool GenerateResourceFile( const char *pInputFile, const void *pParams ) = 0;

	// pParams are the compile parameters (string, int, float and pointer keys). The first virtual of
	// pOutputCallback receives the compiled resource instead of the file being written when it is set
	virtual bool RecompileResource( const char *pInputFile, const char *pOverrideInputData, void *pOutputCallback, KeyValues *pParams ) = 0;

	// pModName is the game directory whose gameinfo.gi the compiler can query; a "<game>_addons" directory
	// uses the one of <game>
	virtual void *unk013( const char *pModName, const char *pCompilerName, const char *pSpecialDependency, int nUserData ) = 0;
	virtual void *CalculateSpecialDependencyFingerprint( void *pResult, const char *pModName, const char *pCompilerName, const char *pSpecialDependency, int nUserData ) = 0;

	// The compiler's string for the special dependency in pOut; without such a compiler pOut is the
	// dependency itself
	virtual const char *GetSpecialDependencyString( const char *pModName, const char *pCompilerName, const char *pSpecialDependency, int nUserData, CBufferString *pOut ) = 0;

	virtual void *CalculateSpecialInputDependencyFingerprint( void *pResult, void *pUnk1, const char *pCompilerName, const char *pInputFile, const char *pSpecialDependency, void *pUnk2 ) = 0;

	// Same as GetSpecialDependencyString for a special input dependency
	virtual const char *GetSpecialInputDependencyString( void *pUnk, const char *pCompilerName, const char *pInputFile, const char *pSpecialDependency, CBufferString *pOut ) = 0;

	virtual void InstallTestFilesystem( IResourceCompileFileSystemInterface *pTestFS ) = 0;
	virtual void UninstallTestFilesystem( IResourceCompileFileSystemInterface *pTestFS ) = 0;

	virtual void unk020( void *pKey, void *pResults ) = 0;
};

class CResourceCompilerSystem : public CTier3DmAppSystem< IResourceCompilerSystem >
{
public:
	bool m_bUnk028;

	// Results of RecompileResource, as in the compile summary
	int m_nCompiledCount;
	int m_nSkippedCount;
	int m_nFailedCount;

	uint8 m_unk038[ 0x10 ];
	CUtlString m_sUnk048;
	CUtlString m_sUnk050;
	void *m_pUnk058;
	CUtlVector< CUtlString > m_vecUnk060;
	CUtlVector< CUtlString > m_vecUnk078;
	uint8 m_unk090[ 0x48 ];

	// Vector of 48-byte compile messages that carry their severity (1 warning, 2 error)
	uint8 m_CompileMessages[ 0x18 ];

	// Warning and error counts added to the compile summary
	int m_nUnk0F0;
	int m_nUnk0F4;
};

#endif // IRESOURCECOMPILERSYSTEM_H
