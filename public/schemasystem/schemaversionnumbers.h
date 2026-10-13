#ifndef SCHEMAVERSIONNUMBERS_H
#define SCHEMAVERSIONNUMBERS_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Format versions shared by schemacompiler and the code it generates; bump on incompatible changes
//--------------------------------------------------------------------------------------------------

// Shape of the generated records, registrar and schemagen.h helpers
#define SCHEMA_BINDING_VERSION 1

// schema_project.<config>.kv3 written by sourcesdk_target_schema()
#define SCHEMA_PROJECT_VERSION 1

#endif // SCHEMAVERSIONNUMBERS_H
