#ifndef AIDEBUGSNAPSHOTMETADATA_H
#define AIDEBUGSNAPSHOTMETADATA_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// AI debug snapshot tags
//--------------------------------------------------------------------------------------------------
#include "schemasystem/schemametatag.h"

typedef void ( *DebugSnapshotDataRenderFn_t )( const void *pSnapshotData );
typedef bool ( *DebugSnapshotDataSummaryFn_t )( const void *pSnapshotData, CBufferString *pOutSummary );

DECLARE_SCHEMA_META_TAG( MDebugSnapshotDataRenderFn, META_TAG_ON_CLASS, META_VALUE( DebugSnapshotDataRenderFn_t ) );
DECLARE_SCHEMA_META_TAG( MDebugSnapshotDataSummaryFn, META_TAG_ON_CLASS, META_VALUE( DebugSnapshotDataSummaryFn_t ) );

#endif // AIDEBUGSNAPSHOTMETADATA_H
