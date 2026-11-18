#ifndef SAVERESTOREMETADATA_H
#define SAVERESTOREMETADATA_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Save/restore tags
//--------------------------------------------------------------------------------------------------
#include "schemasystem/schemametatag.h"

// Excluded from the generated KV3 transfer and the data description
DECLARE_SCHEMA_META_TAG( MNotSaved, META_TAG_ON_FIELD, META_TAG_ONLY() );

// 1 wherever found; meaning unknown
DECLARE_SCHEMA_META_TAG( MSaveBehavior, META_TAG_ON_FIELD, META_VALUE( int ) );

#endif // SAVERESTOREMETADATA_H
