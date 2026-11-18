#ifndef FGDMETADATA_H
#define FGDMETADATA_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// FGD tags
//--------------------------------------------------------------------------------------------------
#include "schemasystem/schemametatag.h"

// KV3 added to the class FGD entry, e.g. "{ standard_yielding_flow = true }"
DECLARE_SCHEMA_META_TAG( MCustomFGDMetadata, META_TAG_ON_CLASS, META_VALUE( const char * ) );

// FGD helper, e.g. "game_data_list{ key = 'CDestructiblePart' }"
DECLARE_SCHEMA_META_TAG( MFgdHelper, META_TAG_ON_CLASS, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MFgdFromSchemaCompletelySkipField, META_TAG_ON_FIELD, META_TAG_ONLY() );

#endif // FGDMETADATA_H
