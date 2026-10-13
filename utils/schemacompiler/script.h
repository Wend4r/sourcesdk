#ifndef SCHEMACOMPILER_SCRIPT_H
#define SCHEMACOMPILER_SCRIPT_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Interpreter of DECLARE_SCHEMA_CODEGEN_TAG scripts, e.g. MEmitKV3Transfer
//--------------------------------------------------------------------------------------------------
#include "schemacompiler.h"

// Runs the script of pszTag for one class model; pCodegenTags is the codegen_tags array of its input
bool Script_EmitClass( const ModelValue_t *pClass, const ModelValue_t *pCodegenTags, const char *pszTag, CUtlBuffer &output );

#endif // SCHEMACOMPILER_SCRIPT_H
