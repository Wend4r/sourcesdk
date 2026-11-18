//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//=============================================================================//

/* This is totally reverse-engineered code and may be wrong */

#include "tier0/dbg.h"
#include "tier0/bufferstring.h"
#include "tier0/strtools.h"
#include "interfaces/interfaces.h"
#include "resourcefile/manifestregistrar.h"
#include "schemasystem/schemaregistration.h"
#include "schemasystem/schemasystem.h"

#if defined( POSIX )
#include <dlfcn.h>
#endif

// Reported through BPL_PROJECTNAME
#ifndef MODULE_PROJECT_NAME
#define MODULE_PROJECT_NAME "sourcesdk"
#endif

// Reported through BPL_BUILDCONFIGURATION, BPL_ISDEBUG and BPL_ISRELEASE, CMake passes the build configuration
#ifndef MODULE_BUILD_CONFIGURATION
#ifdef _DEBUG
#define MODULE_BUILD_CONFIGURATION "Debug"
#else
#define MODULE_BUILD_CONFIGURATION "Release"
#endif
#endif

#ifndef MODULE_BUILD_IS_DEBUG
#ifdef _DEBUG
#define MODULE_BUILD_IS_DEBUG 1
#else
#define MODULE_BUILD_IS_DEBUG 0
#endif
#endif

// ------------------------------------------------------------------------------------ //
// GetNameOfModule.
// The engine bakes the file name in per module, the SDK resolves it from its own address.
// ------------------------------------------------------------------------------------ //
const char *GetNameOfModule()
{
	static char s_szModuleName[MAX_PATH] = "";

	if ( s_szModuleName[0] )
	{
		return s_szModuleName;
	}

	const char *pszPath = NULL;

#if defined( _WIN32 )
	char szPath[MAX_PATH];
	HMODULE hModule = NULL;

	if ( GetModuleHandleExA( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>( &GetNameOfModule ), &hModule ) &&
	    GetModuleFileNameA( hModule, szPath, sizeof( szPath ) ) )
	{
		pszPath = szPath;
	}
#elif defined( POSIX )
	Dl_info info;

	if ( dladdr( reinterpret_cast<void *>( &GetNameOfModule ), &info ) && info.dli_fname )
	{
		pszPath = info.dli_fname;
	}
#endif

	if ( !pszPath )
	{
		return "";
	}

	const char *pszName = pszPath;

	for ( const char *pszCur = pszPath; *pszCur; pszCur++ )
	{
		if ( *pszCur == '/' || *pszCur == '\\' )
		{
			pszName = pszCur + 1;
		}
	}

	V_strncpy( s_szModuleName, pszName, sizeof( s_szModuleName ) );

	return s_szModuleName;
}

// ------------------------------------------------------------------------------------ //
// Schema registration
// ------------------------------------------------------------------------------------ //
CSchemaRegistration *CSchemaRegistration::s_pSchemaRegistrationList = NULL;

void CSchemaRegistration::RegisterAllModuleData( ISchemaSystem *pSchemaSystem )
{
	CBufferString sFailureReason;

	for ( int nPhase = SCHEMA_REGISTRATION_PHASE_TYPE_TABLES; nPhase < SCHEMA_REGISTRATION_PHASE_COUNT; nPhase++ )
	{
		for ( int nLoops = 999; s_pSchemaRegistrationList; )
		{
			CBufferString *pFailureReason = nLoops == 1 ? &sFailureReason : NULL;

			bool bSucceeded = true;

			for ( CSchemaRegistration *pCur = s_pSchemaRegistrationList; pCur; pCur = pCur->m_pNext )
			{
				bSucceeded &= pCur->RegisterAllBindings( pSchemaSystem, static_cast< SchemaRegistrationPhase_t >( nPhase ), pFailureReason );
			}

			if ( bSucceeded )
			{
				break;
			}

			if ( --nLoops == 0 )
			{
				if ( sFailureReason.IsEmpty() )
				{
					sFailureReason.Set( "<no failure reason set>" );
				}

				Plat_FatalError( "unable to register all schema data: %s\n", sFailureReason.Get() );
				break;
			}
		}
	}

	pSchemaSystem->CompleteModuleRegistration( GetNameOfModule() );
}

// ------------------------------------------------------------------------------------ //
// Module exports.
// Queried by the engine through dynamic binding alongside CreateInterface.
// ------------------------------------------------------------------------------------ //
#ifndef SCHEMA_EXPORTS_OVERRIDE

static bool s_bSchemaBindingsInstalled = false;

bool InstallSchemaBindings( const char *pSchemaSystemInterfaceVersion, ISchemaSystem *pSchemaSystem )
{
	if ( V_strcmp( SCHEMASYSTEM_INTERFACE_VERSION, pSchemaSystemInterfaceVersion ) )
	{
		return false;
	}

	if ( !s_bSchemaBindingsInstalled )
	{
		g_pSchemaSystem = pSchemaSystem;
		CSchemaRegistration::RegisterAllModuleData( pSchemaSystem );
		s_bSchemaBindingsInstalled = true;
	}

	return true;
}

#endif // SCHEMA_EXPORTS_OVERRIDE

#ifndef RESOURCE_EXPORTS_OVERRIDE

int GetResourceManifestCount()
{
	return CManifestRegistrar::GetCount();
}

int GetResourceManifests( int nFirstIndex, ResourceManifestDesc_t **ppDesc, size_t nDescCount )
{
	size_t nCount = 0;
	int nIndex = 0;

	for ( CManifestRegistrar *pCur = CManifestRegistrar::GetFirst(); pCur; pCur = pCur->m_pNext, nIndex++ )
	{
		if ( nIndex < nFirstIndex )
		{
			continue;
		}

		ppDesc[ nCount++ ] = pCur->GetDesc();

		if ( nCount >= nDescCount )
		{
			break;
		}
	}

	return static_cast< int >( nCount );
}

#endif // RESOURCE_EXPORTS_OVERRIDE

int BinaryProperties_GetValue( BinaryProperties_Lookups_t propertyLookup, BinaryProperties_Value_t *pOutput )
{
	switch ( propertyLookup )
	{
		case BPL_BUILDCONFIGURATION:
		{
			pOutput->valueType = BPV_STRING;
			pOutput->val.pString = MODULE_BUILD_CONFIGURATION;

			return 1;
		}

		case BPL_ISDEBUG:
		case BPL_ISRELEASE:
		{
			pOutput->valueType = BPV_INT64;
			pOutput->val.nInt64 = propertyLookup == ( MODULE_BUILD_IS_DEBUG ? BPL_ISDEBUG : BPL_ISRELEASE );

			return 1;
		}

		case BPL_PROJECTNAME:
		{
			pOutput->valueType = BPV_STRING;
			pOutput->val.pString = MODULE_PROJECT_NAME;

			return 1;
		}

		case BPL_DEV_BUILD:
		case BPL_RETAIL_BUILD:
		{
			// Same as the shipped game modules
			pOutput->valueType = BPV_INT64;
			pOutput->val.nInt64 = propertyLookup == BPL_RETAIL_BUILD;

			return 1;
		}

		case BPL_BINARY_TYPE:
		{
			pOutput->valueType = BPV_INT64;
			pOutput->val.nInt64 = BP_BT_DLL;
	
			return 1;
		}

		default:
		{
			return 0;
		}
	}
}
