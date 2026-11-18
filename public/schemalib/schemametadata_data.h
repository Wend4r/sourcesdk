#ifndef SCHEMALIB_SCHEMAMETADATA_DATA_H
#define SCHEMALIB_SCHEMAMETADATA_DATA_H

#ifdef _WIN32
#pragma once
#endif

struct SchemaMetadataEntryData_t
{
	const char* m_pszName;
	void* m_pData;
};

#endif // SCHEMALIB_SCHEMAMETADATA_DATA_H
