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
#include "appframework/iappsystem.h"

class KeyValues;
class CKV3MemberName;
class IResourceCompileFileSystemInterface;

// The special dependency queries build a fingerprint context and pass it to the IResourceTypeCompiler
// registered under pCompilerName. pModName selects the gameinfo the compiler sees ( a "<mod>_addons"
// name falls back to "<mod>" ). The fingerprint queries return { uint32 fingerprint; bool valid } through
// pResult ( the hidden return slot of MSVC ); valid is false when the compiler does not exist.
abstract_class IResourceCompilerSystem : public IAppSystem
{
public:
	virtual bool GenerateResourceFile( const char *pInputFile, const void *pParams ) = 0;

	// pOverrideInputData replaces the content of pInputFile ( "___OverrideInputData___" ). When pOutputCallback
	// is set, its first virtual receives the compiled resource instead of the file being written; pArgs holds
	// the compile options
	virtual bool RecompileResource( const char *pInputFile, const char *pOverrideInputData, void *pOutputCallback, KeyValues *pArgs ) = 0;

	// Named by what it does: the compiler's answer for the special dependency ( IResourceTypeCompiler slot 2 )
	virtual int GetSpecialDependencyType( const char *pModName, const char *pCompilerName, const char *pSpecialDependency, int nUserData ) = 0;
	virtual void *CalculateSpecialDependencyFingerprint( void *pResult, const char *pModName, const char *pCompilerName, const char *pSpecialDependency, int nUserData ) = 0;

	// Named by what it does: the compiler's text for the special dependency ( its name when the compiler
	// does not exist )
	virtual const char *GetSpecialDependencyDescription( const char *pModName, const char *pCompilerName, const char *pSpecialDependency, int nUserData, CBufferString *pOut ) = 0;

	// pDependencyData is handed to the compiler through the context's third virtual
	virtual void *CalculateSpecialInputDependencyFingerprint( void *pResult, const char *pModName, const char *pCompilerName, const char *pInputFile, const char *pSpecialDependency, const void *pDependencyData ) = 0;

	// Named by what it does: same as GetSpecialDependencyDescription for a special input dependency
	virtual const char *GetSpecialInputDependencyDescription( const char *pModName, const char *pCompilerName, const char *pInputFile, const char *pSpecialDependency, CBufferString *pOut ) = 0;

	virtual void InstallTestFilesystem( IResourceCompileFileSystemInterface *pTestFS ) = 0;
	virtual void UninstallTestFilesystem( IResourceCompileFileSystemInterface *pTestFS ) = 0;

	// Named by what it does: appends { module name, KeyValues3 * } ( 16 bytes ) for every loaded module
	// metadata file ( .vmmd ) that has the member name
	virtual void FindModuleMetadataMember( const CKV3MemberName &name, void *pResults ) = 0;
};

#endif // IRESOURCECOMPILERSYSTEM_H
