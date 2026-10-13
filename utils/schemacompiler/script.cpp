#include "script.h"

bool Script_EmitClass( const ModelValue_t *pClass, const ModelValue_t *pCodegenTags, const char *pszTag, CUtlBuffer &output )
{
	(void)pCodegenTags;
	(void)output;

	g_Diagnostics.Error( SC_SCRIPT, { CUtlString( Model_GetMemberString( pClass, MODEL_KEY_FILE ) ), static_cast< int >( Model_GetMemberInt( pClass, MODEL_KEY_LINE ) ), 0 }, "codegen tag '%s' cannot run yet", pszTag );

	return false;
}
