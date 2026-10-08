#ifndef SCHEMACOMPILER_CODEGEN_H
#define SCHEMACOMPILER_CODEGEN_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Code generation from the models of all inputs
//--------------------------------------------------------------------------------------------------
#include "schemacompiler.h"

struct CodegenOutput_t
{
	CUtlString m_sPath;
	CUtlBuffer m_Text;
};

// models is parallel to project.m_Inputs; returns false on errors, the outputs are then incomplete.
// The outputs are owned by the caller
bool Codegen_Generate( const SchemaProject_t &project, const CUtlLeanVector< const ModelValue_t * > &models, CUtlLeanVector< CodegenOutput_t * > &outputs );

#endif // SCHEMACOMPILER_CODEGEN_H
