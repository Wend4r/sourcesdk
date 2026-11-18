#ifndef SCHEMASYSTEM_MINSCHEMA_H
#define SCHEMASYSTEM_MINSCHEMA_H

#ifdef _WIN32
#pragma once
#endif

#include "schemasystem/schematypes.h"

typedef const CSchemaClassInfo *ClassIntrospectionHandle_t;

typedef const CSchemaClassField *FieldIntrospectionHandle_t;

enum SchemaTypeScope_t : uint8
{
	SCHEMA_GLOBAL_TYPE_SCOPE = 0,
	SCHEMA_MODULE_LOCAL_TYPE_SCOPE,
	SCHEMA_DEFAULT_TYPE_SCOPE,
};

#endif // SCHEMASYSTEM_MINSCHEMA_H
