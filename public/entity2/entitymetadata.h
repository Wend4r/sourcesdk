#ifndef ENTITYMETADATA_H
#define ENTITYMETADATA_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Entity tags
//--------------------------------------------------------------------------------------------------
#include "schemasystem/schemametatag.h"

DECLARE_SCHEMA_META_TAG( MEntityAllowsPortraitWorldSpawn, META_TAG_ON_CLASS, META_TAG_ONLY() );

// VData file of the scope entity subclasses, e.g. "scripts/weapons.vdata"
DECLARE_SCHEMA_META_TAG( MEntitySubclassScopeFile, META_TAG_ON_ENUMERATOR, META_VALUE( const char * ) );

#endif // ENTITYMETADATA_H
