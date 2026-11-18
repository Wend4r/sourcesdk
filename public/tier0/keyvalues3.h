#ifndef TIER0_KEYVALUES3_H
#define TIER0_KEYVALUES3_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "tier0/platform.h"
#include "kv3lib/kv3formats.h"

class KeyValues3;
class CKV3Arena;
class CBufferString;
class CUtlBuffer;
class CUtlString;
struct KV1ToKV3Translation_t;
struct KV3ToKV1Translation_t;

enum KV1TextEscapeBehavior_t
{
	KV1_NO_ESCAPE_SEQUENCES = 0,
	KV1_HAS_ESCAPE_SEQUENCES = 1,
};

// Only LoadKV3FromOldSchemaText reads these, which LoadKV3 calls for old schema text.
enum KV3LoadTextFlags_t
{
	KV3_LOAD_TEXT_NONE = 0,
	KV3_LOAD_TEXT_OBJECT_REFERENCES_AS_STRINGS = 1 << 0, // keeps "&name" and NULL object references as strings instead of resolving them

	// Previous name of KV3_LOAD_TEXT_OBJECT_REFERENCES_AS_STRINGS, kept for compatibility.
	KV3_LOAD_TEXT_HAS_OBJECT_REFERANCES = KV3_LOAD_TEXT_OBJECT_REFERENCES_AS_STRINGS,
};

enum KV3SaveTextFlags_t
{
	KV3_SAVE_TEXT_NONE = 0,
	KV3_SAVE_TEXT_TAGGED = (1 << 0), // adds subtype name before value
};

PLATFORM_OVERLOAD void DebugPrintKV3( const KeyValues3* kv );

// When using some LoadKV3/SaveKV3 functions, KV3ID_t structures must be filled in, which specify the format or encoding of the data.

PLATFORM_OVERLOAD bool LoadKV3( CKV3Arena* context, CUtlString* error, CUtlBuffer* input, const KV3ID_t& format, const char* kv_name, uint flags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3( KeyValues3* kv, CUtlString* error, CUtlBuffer* input, const KV3ID_t& format, const char* kv_name, uint flags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3( KeyValues3* kv, CUtlString* error, const char* input, const KV3ID_t& format, const char* kv_name, uint flags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromFile( CKV3Arena* context, CUtlString* error, const char* filename, const char* path, const KV3ID_t& format, uint flags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromFile( KeyValues3* kv, CUtlString* error, const char* filename, const char* path, const KV3ID_t& format, uint flags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromJSON( KeyValues3* kv, CUtlString* error, const char* input, const char* kv_name, uint flags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromJSONFile( KeyValues3* kv, CUtlString* error, const char* path, const char* filename, uint flags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromKV1File( KeyValues3* kv, CUtlString* error, const char* path, const char* filename, KV1TextEscapeBehavior_t esc_behavior, uint flags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromKV1Text( KeyValues3* kv, CUtlString* error, const char* input, KV1TextEscapeBehavior_t esc_behavior, const char* kv_name, bool bIgnoreConditionals, uint flags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromKV1Text_Translated( KeyValues3* kv, CUtlString* error, const char* input, KV1TextEscapeBehavior_t esc_behavior, const KV1ToKV3Translation_t* translation, int nTranslationCount, const char* kv_name, bool bIgnoreConditionals, uint flags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromKV3OrKV1( KeyValues3* kv, CUtlString* error, CUtlBuffer* input, const KV3ID_t& format, const char* kv_name, uint flags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3FromOldSchemaText( KeyValues3* kv, CUtlString* error, CUtlBuffer* input, const KV3ID_t& format, const char* kv_name, uint flags = KV3_LOAD_TEXT_NONE );
PLATFORM_OVERLOAD bool LoadKV3Text_NoHeader( KeyValues3* kv, CUtlString* error, const char* input, const KV3ID_t& format, const char* kv_name, uint flags = KV3_LOAD_TEXT_NONE );

PLATFORM_OVERLOAD bool SaveKV3( const KV3ID_t& encoding, const KV3ID_t& format, const KeyValues3* kv, CUtlString* error, CUtlBuffer* output, uint flags = KV3_SAVE_TEXT_NONE );
PLATFORM_OVERLOAD bool SaveKV3AsJSON( const KeyValues3* kv, CUtlString* error, CUtlBuffer* output );
PLATFORM_OVERLOAD bool SaveKV3AsJSON( const KeyValues3* kv, CUtlString* error, CUtlString* output );
PLATFORM_OVERLOAD bool SaveKV3AsKV1Text( const KeyValues3* kv, CUtlString* error, CUtlBuffer* output, KV1TextEscapeBehavior_t esc_behavior );
PLATFORM_OVERLOAD bool SaveKV3AsKV1Text_Translated( const KeyValues3* kv, CUtlString* error, CUtlBuffer* output, KV1TextEscapeBehavior_t esc_behavior, const KV3ToKV1Translation_t* translation, int nTranslationCount );
PLATFORM_OVERLOAD bool SaveKV3Text_NoHeader( const KeyValues3* kv, CUtlString* error, CBufferString* output, uint flags = KV3_SAVE_TEXT_NONE );
PLATFORM_OVERLOAD bool SaveKV3Text_NoHeader( const KeyValues3* kv, CUtlString* error, CUtlBuffer* output, uint flags = KV3_SAVE_TEXT_NONE );
PLATFORM_OVERLOAD bool SaveKV3Text_NoHeader( const KeyValues3* kv, CUtlString* error, CUtlString* output, uint flags = KV3_SAVE_TEXT_NONE );
PLATFORM_OVERLOAD bool SaveKV3Text_ToString( const KV3ID_t& format, const KeyValues3* kv, CUtlString* error, CBufferString* output, uint flags = KV3_SAVE_TEXT_NONE );
PLATFORM_OVERLOAD bool SaveKV3Text_ToString( const KV3ID_t& format, const KeyValues3* kv, CUtlString* error, CUtlString* output, uint flags = KV3_SAVE_TEXT_NONE );
PLATFORM_OVERLOAD bool SaveKV3ToFile( const KV3ID_t& encoding, const KV3ID_t& format, const KeyValues3* kv, CUtlString* error, const char* filename, const char* path, uint flags = KV3_SAVE_TEXT_NONE );

// Detection of the KV3 data in a buffer. pOutDetectedFormat receives the format; for text, its
// name is copied to pFormatName (nFormatNameSize chars) and pOutDetectedFormat points at it
PLATFORM_OVERLOAD bool LooksLikeKV3BinaryData( const void *pData, int nBufLen, KV3ID_t *pOutDetectedFormat );
PLATFORM_OVERLOAD bool LooksLikeKV3TextData( const void *pData, int nBufLen, KV3ID_t *pOutDetectedFormat, char *pFormatName, int nFormatNameSize );
PLATFORM_OVERLOAD bool LooksLikeOldSchemaText( const void *pData, int nBufLen, KV3ID_t *pOutDetectedFormat );
PLATFORM_OVERLOAD bool IsKV3Data( const void *pData, int nBufLen, KV3ID_t *pOutDetectedFormat, char *pFormatName, int nFormatNameSize );
PLATFORM_OVERLOAD bool IsKV3Data( const CUtlBuffer *pSrcBuffer, KV3ID_t *pOutDetectedFormat, char *pFormatName, int nFormatNameSize );

// The current format of the files with the extension, "generic" for an unknown one
PLATFORM_OVERLOAD KV3ID_t GetCurrentKV3FormatVersionForExtension( const char *pExtension, bool *pbUnknown );

// Converts the data in pRoot from one format to another
PLATFORM_INTERFACE bool ConvertKV3Format( KeyValues3 *pRoot, const KV3ID_t &fromFormat, const KV3ID_t &toFormat, CUtlString *pOutError );

// Whether the format is one of the model document formats, which pOutFormat receives
PLATFORM_INTERFACE bool IsModelDocFormat( const KV3ID_t &id, KV3ID_t *pOutFormat );

// Splits "Name{ args... }" into the name and the KV3 arguments
PLATFORM_INTERFACE bool SplitNameAndKV3Args( const char *pString, CBufferString *pOutName, KeyValues3 *pOutArgs, CUtlString *pOutError );

// Particle system data helpers
PLATFORM_OVERLOAD bool IsParticleSystemBehaviorVersionDeprecated( KeyValues3 *pParticleSystem, int *pBehaviorVersion );
PLATFORM_OVERLOAD bool TryCullParticleSystemDisabledBlocks( KeyValues3 *pParticleSystem );

#endif // TIER0_KEYVALUES3_H
