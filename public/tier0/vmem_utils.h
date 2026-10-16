#ifndef VMEM_UTILS_H
#define VMEM_UTILS_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/bufferstring.h"
#include "tier0/logging.h"

// The index works with GetRegion, GetRegionByStart and GetRegionByBytes
#define FOR_EACH_VMEM_REGION( snapshot, iter ) \
	for ( int iter = 0; iter < ( snapshot ).GetRegionsCount(); iter++ )
#define FOR_EACH_VMEM_REGION_BACK( snapshot, iter ) \
	for ( int iter = ( snapshot ).GetRegionsCount() - 1; iter >= 0; iter-- )

//-----------------------------------------------------------------------------
// Snapshot of the virtual memory regions mapped into a process
//-----------------------------------------------------------------------------
class CVirtualMemorySnapshot
{
public:
	struct Region_t
	{
		uint64 m_nStart;
		uint64 m_nBytes;
		uint32 m_nAccess;
		int m_nDescString; // Index of the description in the region strings, -1 if none
		uint32 m_bShared : 1;
		uint32 m_bSharedKnown : 1;
		uint32 m_bFileMapping : 1;
		uint32 m_bImageMapping : 1;
		uint32 m_bImageMappingKnown : 1;
	};

	DLL_CLASS_IMPORT CVirtualMemorySnapshot();
	DLL_CLASS_IMPORT ~CVirtualMemorySnapshot();

	DLL_CLASS_IMPORT bool Capture( uint32 nProcessId );
	DLL_CLASS_IMPORT bool Capture( void *hProcess );
	DLL_CLASS_IMPORT void Purge();
	DLL_CLASS_IMPORT void Report( LoggingChannelID_t channelID ) const;

	// Fills this snapshot with the regions that differ between the two
	DLL_CLASS_IMPORT void Diff( const CVirtualMemorySnapshot *pBefore, const CVirtualMemorySnapshot *pAfter );

	DLL_CLASS_IMPORT void GetLargestUnmappedRegion( uint64 *pStart, uint64 *pBytes, uint64 nMinAddress ) const;
	DLL_CLASS_IMPORT uint64 GetAddressAfterLastRegion( uint64 nBelowAddress ) const;

	DLL_CLASS_IMPORT static bool IsEqualRegion( const CVirtualMemorySnapshot *pSnapshotA, const Region_t *pRegionA, const CVirtualMemorySnapshot *pSnapshotB, const Region_t *pRegionB );

	uint32 GetProcessId() const { return m_nProcessId; }
	bool GetCaptureHasErrors() const { return m_bCaptureErrors; }
	bool IsDiff() const { return m_bIsDiff; }

	uint64 GetTotalVirtualBytes() const { return m_nTotalVirtualBytes; }
	uint64 GetPeakTotalVirtualBytes() const { return m_nPeakTotalVirtualBytes; }
	uint64 GetResidentVirtualBytes() const { return m_nResidentVirtualBytes; }
	uint64 GetPeakResidentVirtualBytes() const { return m_nPeakResidentVirtualBytes; }

	int GetRegionsCount() const { return m_nRegionsCount; }
	const int *GetRegionsByStartIndices() const { return m_pRegionsByStart; }
	const int *GetRegionsByBytesIndices() const { return m_pRegionsByBytes; }
	const Region_t *GetRegion( int nIndex ) const { return &m_pRegions[ nIndex ]; }
	const Region_t *GetRegionByStart( int nIndex ) const { return &m_pRegions[ m_pRegionsByStart[ nIndex ] ]; }
	const Region_t *GetRegionByBytes( int nIndex ) const { return &m_pRegions[ m_pRegionsByBytes[ nIndex ] ]; }

	// The description of a region, such as the mapped file, from the region strings
	const char *GetRegionString( int nOffset ) const { return m_RegionStrings.Get() + nOffset; }
	bool RegionHasDesc( const Region_t *pRegion ) const { return pRegion->m_nDescString >= 0; }
	const char *GetRegionDesc( const Region_t *pRegion ) const { return RegionHasDesc( pRegion ) ? GetRegionString( pRegion->m_nDescString ) : ""; }

private:
	uint32 m_nProcessId;
	uint64 m_nTotalVirtualBytes;
	uint64 m_nPeakTotalVirtualBytes;
	uint64 m_nResidentVirtualBytes;
	uint64 m_nPeakResidentVirtualBytes;
	CBufferString m_FileContent;
	CBufferString m_RegionStrings;
	Region_t *m_pRegions;
	int *m_pRegionsByStart;
	int *m_pRegionsByBytes;
	int m_nRegionsAlloc;
	int m_nRegionsCount;
	bool m_bCaptureErrors;
	bool m_bIsDiff;

	CVirtualMemorySnapshot( const CVirtualMemorySnapshot & ) = delete;
	CVirtualMemorySnapshot &operator=( const CVirtualMemorySnapshot & ) = delete;
};

#endif // VMEM_UTILS_H
