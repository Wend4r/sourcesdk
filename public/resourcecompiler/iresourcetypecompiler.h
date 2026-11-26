//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Compiler of one resource type, looked up by the resource compiler by its identifier
//
//===========================================================================//

#ifndef IRESOURCETYPECOMPILER_H
#define IRESOURCETYPECOMPILER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CUtlBuffer;
class CBufferString;
class IResourceCompilerContext;

// The special dependency queries get a context built by IResourceCompilerSystem ( opaque here ) whose first
// virtual returns the special dependency's name
abstract_class IResourceTypeCompiler
{
public:
	// Named by what they do: compile the input of pContext; the resource compiler calls CompileFromBuffer when
	// the input file is replaced by override data ( "___OverrideInputData___" ) and Compile otherwise
	virtual bool CompileFromBuffer( CUtlBuffer *pInputData, IResourceCompilerContext *pContext ) = 0;
	virtual bool Compile( IResourceCompilerContext *pContext ) = 0;

	virtual int GetSpecialDependencyType( const void *pContext ) = 0;
	virtual uint32 CalculateSpecialDependencyFingerprint( const void *pContext ) = 0;
	virtual const char *GetSpecialDependencyDescription( const void *pContext, CBufferString *pOut ) = 0;

	// Named by what it does: the size of the buffers the compile writes the resource into; 0 means 512 MB
	virtual uint32 GetMaxResourceDataSize() = 0;

	// Named by what it does: the answer is stored with the compile's VNotify message
	virtual bool IsVNotifyQuiet() = 0;

	// Named by what it does: checks the size of the non-streaming data of the written resource file; false
	// fails the compile
	virtual bool ValidateNonStreamingDataSize( IResourceCompilerContext *pContext, uint32 nNonStreamingDataSize ) = 0;

	virtual uint32 CalculateSpecialInputDependencyFingerprint( const void *pContext ) = 0;
	virtual const char *GetSpecialInputDependencyDescription( const void *pContext, CBufferString *pOut ) = 0;
};

#endif // IRESOURCETYPECOMPILER_H
