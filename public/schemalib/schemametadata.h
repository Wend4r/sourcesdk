#ifndef SCHEMALIB_SCHEMAMETADATA_H
#define SCHEMALIB_SCHEMAMETADATA_H

#ifdef _WIN32
#pragma once
#endif

#include "schemasystem/minschema.h"

//--------------------------------------------------------------------------------------------------
// m_pData points at the tag value, null for a tag without one
//--------------------------------------------------------------------------------------------------
class CSchemaMetadataEntry : public SchemaMetadataEntryData_t
{
};

#endif // SCHEMALIB_SCHEMAMETADATA_H
