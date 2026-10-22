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
#include "tier0/utlmap.h"
#include "appframework/iappsystem.h"
#include "tier3/tier3dm.h"

class IResourceCompileFileSystemInterface;
class KeyValues;
class KeyValues3;
class CKV3MemberName;

struct ModuleMetadataValue_t
{
	// "module_name" of the metadata file
	const char *m_pszModuleName;

	KeyValues3 *m_pValue;
};

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

	// pDependencyData is the data the compiler stored with the special input dependency
	virtual void *CalculateSpecialInputDependencyFingerprint( void *pResult, const char *pModName, const char *pCompilerName, const char *pInputFile, const char *pSpecialDependency, KeyValues3 *pDependencyData ) = 0;

	// Same as GetSpecialDependencyString for a special input dependency
	virtual const char *GetSpecialInputDependencyString( const char *pModName, const char *pCompilerName, const char *pInputFile, const char *pSpecialDependency, CBufferString *pOut ) = 0;

	virtual void InstallTestFilesystem( IResourceCompileFileSystemInterface *pTestFS ) = 0;
	virtual void UninstallTestFilesystem( IResourceCompileFileSystemInterface *pTestFS ) = 0;

	// Appends the member of that name of every loaded module metadata file ("vmodulemetadata")
	virtual void GetModuleMetadataValues( CKV3MemberName name, CUtlVector< ModuleMetadataValue_t > *pResults ) = 0;
};

class CResourceCompilerSystem : public CTier3DmAppSystem< IResourceCompilerSystem >
{
public:
	// -v: also passed to IP4::SetVerbose and to the compile contexts
	bool m_bVerbose;

	// Set when the module was loaded by resourcecompiler.exe; Init then starts the "GlobPool" thread pool
	bool m_bIsResourceCompilerExe;

	// Results of RecompileResource, as in the compile summary
	int m_nCompiledCount;
	int m_nSkippedCount;
	int m_nFailedCount;

	int m_nInitCount;
	int m_nConnectCount;
	CreateInterfaceFn m_pConnectFactory;

	// Default Perforce changelists of the input files and of the compiled files a compile adds,
	// "Tools Auto-Checkout" for the compiled ones; GenerateResourceFile parameters override them
	CUtlString m_sInputFilesChangelist;
	CUtlString m_sOutputFilesChangelist;

	// Asset types loaded from assettypes_common.txt (and assettypes_internal.txt) by Init
	void *m_pAssetConfig;

	CUtlVector< CUtlString > m_vecUnk060;

	// File extensions given with -skiptype
	CUtlVector< CUtlString > m_SkipTypes;

	// A CUtlHashtable with 8-byte keys that only the destructor touches
	uint8 m_unk090[ 0x20 ];

	// gameinfo.gi of each mod name the compilers query, a "<game>_addons" mod shares the one of <game>
	CUtlOrderedMap< const char *, KeyValues *, CDefFastCaselessStringLess > m_GameInfos;

	// Vector of 48-byte compile messages that carry their severity (1 warning, 2 error)
	uint8 m_CompileMessages[ 0x18 ];

	// Warnings and errors that were not logged because there were too many, added to the compile summary
	int m_nUnloggedWarningCount;
	int m_nUnloggedErrorCount;
};

#endif // IRESOURCECOMPILERSYSTEM_H
