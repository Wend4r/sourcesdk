#pragma once

#include "tier0/basetypes.h"
#include "tier1/utlsymbollarge.h"

DECLARE_POINTER_HANDLE(FlattenedSerializerHandle_t);

struct FlattenedSerializerDesc_t
{
	CUtlSymbolLarge             m_name;
	FlattenedSerializerHandle_t m_handle;
};

struct BuildFlattenedSerializerInfo_t {
};

abstract_class IFlattenedSerializers
{
public:
	virtual void FindOrCreateCreateFlattenedSerializers( BuildFlattenedSerializerInfo_t* pInfo, int nBucket ) = 0;
};
