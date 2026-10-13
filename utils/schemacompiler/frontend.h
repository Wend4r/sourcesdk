#ifndef SCHEMACOMPILER_FRONTEND_H
#define SCHEMACOMPILER_FRONTEND_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Clang frontend: parses the inputs in schema compiler mode and fills one model table per input
//--------------------------------------------------------------------------------------------------
#include "schemacompiler.h"

struct FrontendOptions_t
{
	CUtlString m_sResourceDir;		// Clang builtin headers (lib/clang/<version>)
	int m_nThreads = 0;				// 0 is the hardware concurrency
	bool m_bVerbose = false;
};

// Pre-included headers and the PCH header, compiled once into a .pch
struct FrontendPrefix_t
{
	CUtlString m_sPchFile;			// Empty without a prefix
	CSchemaStringList m_Deps;

	// The prefix includes inputs: no PCH, the units include the prefix headers as text
	bool m_bTextual = false;
	CSchemaStringList m_TextualIncludes;

	// Markup the PCH hides from the preprocessor callbacks of the units, recorded while it is built
	ModelArena_t *m_pArena = nullptr;
	ModelValue_t *m_pMarkup = nullptr;
};

// One translation unit: a single input, or a unity batch of headers
struct FrontendUnit_t
{
	CUtlLeanVector< int > m_Inputs;					// Indices into SchemaProject_t::m_Inputs
	CUtlLeanVector< ModelValue_t * > m_Models;		// Filled tables, parallel to m_Inputs
	CSchemaStringList m_Deps;					// Files the unit read, normalized
	bool m_bSuccess = false;
	double m_flSeconds = 0.0;
};

// Arenas the units' models live in, one per worker thread; they outlive the parse
struct FrontendArenas_t
{
	CUtlLeanVector< ModelArena_t * > m_Arenas;

	~FrontendArenas_t();
};

bool Frontend_BuildPrefix( const SchemaProject_t &project, const FrontendOptions_t &options, const char *pszPchFile, FrontendPrefix_t &prefix );

void Frontend_ParseUnits( const SchemaProject_t &project, const SchemaAtomicMap_t &atomics, const FrontendOptions_t &options, const FrontendPrefix_t &prefix, CUtlLeanVector< FrontendUnit_t > &units, FrontendArenas_t &arenas );

// xxh3_64bits of a file or a string; false if the file cannot be read
bool Frontend_HashFile( const char *pszPath, uint64 &nHash );
uint64 Frontend_HashString( const char *pszData, int nLength );

// Modification time and size, a cheap check before hashing
bool Frontend_FileStatus( const char *pszPath, uint64 &nModTime, uint64 &nSize );

// Builtin header directory next to the executable, else the one of the Clang the tool was built with
CUtlString Frontend_DefaultResourceDir( const char *pszArgv0 );

double Frontend_Seconds();
uint64 Frontend_PeakMemory();

#endif // SCHEMACOMPILER_FRONTEND_H
