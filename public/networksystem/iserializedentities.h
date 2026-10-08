#ifndef ISERIALIZEDENTITIES_H
#define ISERIALIZEDENTITIES_H

#pragma once

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier1/utlleanvector.h"

#include "iflattenedserializers.h"

class CUtlScratchMemoryPool;

abstract_class ISerializedEntities
{
public:
	virtual SerializedEntityData_t *AllocateSerializedEntity( const FlattenedSerializerDesc_t &serializer, bool bReuseDataBuffer, int nInitialDataBytes ) = 0;
	virtual void FreeSerializedEntity( SerializedEntityData_t *pEntity ) = 0;

	virtual SerializedEntityData_t *DuplicateSerializedEntity( const SerializedEntityData_t *pFrom ) = 0;
	virtual void CopySerializedEntity( SerializedEntityData_t *pTo, const SerializedEntityData_t *pFrom ) = 0;

	virtual int GetFieldPaths( const SerializedEntityData_t *pEntity, CUtlLeanVectorFixedGrowable< int, 4 > *pOutFieldPaths ) = 0;

	virtual void *PackSerializedEntity( const SerializedEntityData_t *pEntity, CUtlLeanVectorFixedGrowable< int, 4 > *pFieldPaths, CUtlScratchMemoryPool *pPool ) = 0;
	virtual void *AllocPackedSerializedEntity( int nBytes ) = 0;

	virtual void ReleasePackedSerializedEntity( void *pPacked ) = 0;
	virtual void FreePackedSerializedEntity( void *pPacked ) = 0;
	virtual size_t GetPackedSerializedEntitySize( const void *pPacked ) = 0;

	virtual bool CompareSerializedEntities( const SerializedEntityData_t *pTo, const SerializedEntityData_t *pFrom, int nBucket ) = 0;

	virtual bool AreSerializedEntitiesEqual( const SerializedEntityData_t *pEntity1, const SerializedEntityData_t *pEntity2 ) = 0;

	// Spews the serialized entity pool memory statistics.
	virtual void SpewMemoryUsage() = 0;

	// "fc=%d, m=%d, sum=%d (poly:%d), bits=%u"
	virtual CUtlString DescribeSerializedEntity( const SerializedEntityData_t *pEntity ) = 0;

	virtual void *GetFieldPathPool() = 0;

	virtual void SetReuseDataBuffer( SerializedEntityData_t *pEntity, bool bReuseDataBuffer ) = 0;

	// Array counts must be below 100000.
	virtual bool IsValidArrayCount( uint nCount ) = 0;

	virtual void ValidateArrayCount( uint nCount, const char *pszFieldName ) = 0;

	virtual size_t GetMemoryUsage( void *pSeenAllocations, const SerializedEntityData_t *pEntity ) = 0;

	virtual void AccumulateFieldMemoryUsage( void *pFieldStats, void *pSeenAllocations, const SerializedEntityData_t *pEntity ) = 0;

	virtual FlattenedSerializerDesc_t GetSerializer( const SerializedEntityData_t *pEntity ) = 0;

	virtual const SerializedEntityMetadataEntry_t *FindFieldMetadata( const SerializedEntityData_t *pEntity, int nFieldPath ) = 0;

	virtual void CollectFieldMetadata( const SerializedEntityData_t *pEntity, void *pOutEntries, byte nType ) = 0;
};

#endif // ISERIALIZEDENTITIES_H
