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

class IResourceCompileFileSystemInterface;

abstract_class IResourceCompilerSystem : public IAppSystem
{
public:
	virtual bool GenerateResourceFile( const char *pInputFile, const void *pParams ) = 0;

	virtual bool RecompileResource( const char *pInputFile, const char *pOverrideInputData, void *pUnk1, void *pUnk2 ) = 0;

	virtual void *unk013( void *pUnk, const char *pCompilerName, const char *pSpecialDependency, int nUserData ) = 0;
	virtual void *CalculateSpecialDependencyFingerprint( void *pResult, void *pUnk, const char *pCompilerName, const char *pSpecialDependency, int nUserData ) = 0;

	virtual const char *unk015( void *pUnk, const char *pCompilerName, const char *pSpecialDependency, int nUserData, CBufferString *pOut ) = 0;

	virtual void *CalculateSpecialInputDependencyFingerprint( void *pResult, void *pUnk1, const char *pCompilerName, const char *pInputFile, const char *pSpecialDependency, void *pUnk2 ) = 0;

	// Same as unk015 for a special input dependency
	virtual const char *unk017( void *pUnk, const char *pCompilerName, const char *pInputFile, const char *pSpecialDependency, CBufferString *pOut ) = 0;

	virtual void InstallTestFilesystem( IResourceCompileFileSystemInterface *pTestFS ) = 0;
	virtual void UninstallTestFilesystem( IResourceCompileFileSystemInterface *pTestFS ) = 0;

	virtual void unk020( void *pKey, void *pResults ) = 0;
};

#endif // IRESOURCECOMPILERSYSTEM_H
