#ifndef VDATAMETADATA_H
#define VDATAMETADATA_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// VData tags
//--------------------------------------------------------------------------------------------------
#include "schemasystem/schemametatag.h"

// VData file of the class instances, e.g. "scripts/explosion_types.vdata"
DECLARE_SCHEMA_META_TAG( MVDataAssociatedFile, META_TAG_ON_CLASS, META_VALUE( const char * ) );

// 1 wherever found; meaning unknown
DECLARE_SCHEMA_META_TAG( MVDataNodeType, META_TAG_ON_CLASS, META_VALUE( int ) );

// 1 wherever found; meaning unknown
DECLARE_SCHEMA_META_TAG( MVDataOverlayType, META_TAG_ON_CLASS, META_VALUE( int ) );

// Tools preview widget, e.g. "noise_stream"
DECLARE_SCHEMA_META_TAG( MVDataPreviewWidget, META_TAG_ON_CLASS, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MVDataRoot, META_TAG_ON_CLASS, META_TAG_ONLY() );
DECLARE_SCHEMA_META_TAG( MVDataUniqueMonotonicInt, META_TAG_ON_FIELD, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MVDataUseLinkedEntityClasses, META_TAG_ON_CLASS, META_TAG_ONLY() );

#endif // VDATAMETADATA_H
