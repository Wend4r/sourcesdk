#ifndef SCHEMALIB_SCHEMAENUMINFO_DATA_H
#define SCHEMALIB_SCHEMAENUMINFO_DATA_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/utlmap.h"
#include "schemalib/schemametadata_data.h"

class CSchemaSystemTypeScope;
class CSchemaEnumInfo;

struct SchemaEnumeratorInfoData_t
{
	const char* m_pszName;
	
	int64 m_nValue;
	
	uint16 m_nStaticMetadataCount;
	SchemaMetadataEntryData_t* m_pStaticMetadata;
};

struct SchemaEnumInfoData_t
{
	CSchemaEnumInfo* m_pSchemaBinding;
	
	const char* m_pszName;
	const char* m_pszProjectName;
	
	uint8 m_nSize;
	uint8 m_nAlignment;
	
	uint16 m_nFlags;

	uint16 m_nEnumeratorCount;
	uint16 m_nStaticMetadataCount;
	
	SchemaEnumeratorInfoData_t* m_pEnumerators;
	SchemaMetadataEntryData_t* m_pStaticMetadata;
	
	CSchemaSystemTypeScope* m_pTypeScope;
	
	int64 m_nMinEnumeratorValue;
	int64 m_nMaxEnumeratorValue;
};

#endif // SCHEMALIB_SCHEMAENUMINFO_DATA_H
