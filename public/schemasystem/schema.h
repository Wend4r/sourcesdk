#ifndef SCHEMA_H
#define SCHEMA_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Schema system tags
//--------------------------------------------------------------------------------------------------
#include "schemasystem/schemametatag.h"

DECLARE_SCHEMA_META_TAG( MClassHasCustomAlignedNewDelete, META_TAG_ON_CLASS, META_TAG_ONLY() );

// Stored only as SCHEMA_CF1_INFO_TAG_* bits of the class record, e.g. on CEntityInstance
DECLARE_SCHEMA_META_TAG( MConstructibleClassBase, META_TAG_ON_CLASS, META_TAG_ONLY() );

// Field name used when comparing a class registered by two modules
DECLARE_SCHEMA_META_TAG( MFieldVerificationName, META_TAG_ON_FIELD, META_VALUE( const char * ) );

// Network type of the atomic, e.g. "int32" for HSequence
DECLARE_SCHEMA_META_TAG( MNetworkSerializeAs, META_TAG_ON_ATOMIC, META_VALUE( const char * ) );

// Class wrapping a single integer or float
DECLARE_SCHEMA_META_TAG( MIsBoxedIntegerType, META_TAG_ON_CLASS, META_TAG_ONLY() );
DECLARE_SCHEMA_META_TAG( MIsBoxedFloatType, META_TAG_ON_CLASS, META_TAG_ONLY() );

DECLARE_SCHEMA_META_TAG( MVectorIsSometimesCoordinate, META_TAG_ON_FIELD, META_VALUE( const char * ) );

// Flags enum whose enumerators may share bits
DECLARE_SCHEMA_META_TAG( MEnumFlagsWithOverlappingBits, META_TAG_ON_ENUM, META_TAG_ONLY() );

// An unknown enumerator in entity key values loads as 0 instead of failing
DECLARE_SCHEMA_META_TAG( MTreatUnknownEnumeratorsAsZero, META_TAG_ON_ENUM, META_TAG_ONLY() );

// Equals the enumerator name wherever found
DECLARE_SCHEMA_META_TAG( MAlternateSemanticName, META_TAG_ON_ENUMERATOR, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MEnumeratorIsNotAFlag, META_TAG_ON_ENUMERATOR, META_TAG_ONLY() );

#endif // SCHEMA_H
