#include "vscript_shared.h"
#include "tier0/bufferstring.h"
#include "tier0/logging.h"
#include "tier0/strtools.h"

DEFINE_LOGGING_CHANNEL_NO_TAGS( LOG_VSCRIPT, "VScript" );

// The game keeps one nesting counter for every script include path; this one only guards VScriptRunScript.
static int s_nIncludeDepth = 0;

HSCRIPT VScriptCompileScript( const char *pszScriptName, bool bWarnMissing )
{
	if ( !g_pScriptVM )
	{
		Log_Warning( LOG_VSCRIPT, "Scripting disabled or no server running\n" );

		return nullptr;
	}

	CBufferStringN< 256 > sPath;

	if ( !V_strstr( pszScriptName, "scripts/vscripts/" ) )
	{
		sPath.Format( "scripts/vscripts/%s", pszScriptName );
		pszScriptName = sPath.Get();
	}

	HSCRIPT hScript = nullptr;

	if ( g_pScriptVM->LoadAndCompileScriptFile( pszScriptName, "GAME", &hScript ) == 1 )
	{
		// Game: also raises a module flag that records the missing script.
		if ( bWarnMissing )
			Log_Warning( LOG_VSCRIPT, "Script not found (%s) \n", pszScriptName );
	}

	return hScript;
}

bool VScriptRunScript( const char *pszScriptName, HSCRIPT hScope, bool bWarnMissing )
{
	if ( !g_pScriptVM )
	{
		Log_Warning( LOG_VSCRIPT, "Scripting disabled or no server running\n" );

		return false;
	}

	if ( !pszScriptName || !*pszScriptName )
	{
		Log_Warning( LOG_VSCRIPT, "Cannot run script: NULL script name\n" );

		return false;
	}

	if ( s_nIncludeDepth > 16 )
	{
		Log_Warning( LOG_VSCRIPT, "IncludeScript stack overflow\n" );

		return false;
	}

	++s_nIncludeDepth;

	bool bSuccess = false;
	HSCRIPT hScript = VScriptCompileScript( pszScriptName, bWarnMissing );

	if ( hScript )
	{
		bSuccess = g_pScriptVM->Run( hScript, hScope, true ) != SCRIPT_ERROR;

		if ( !bSuccess )
			Log_Warning( LOG_VSCRIPT, "Error running script named %s\n", pszScriptName );

		g_pScriptVM->ReleaseScript( hScript );
	}

	--s_nIncludeDepth;

	return bSuccess;
}
