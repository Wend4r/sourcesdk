#ifndef KV3TEXT_H
#define KV3TEXT_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// KeyValues3 text and binary codec of kv3lib, independent of the tier0 LoadKV3/SaveKV3 imports.
// Values loaded into an arena-owned KeyValues3 keep their strings in the arena symbol table
//--------------------------------------------------------------------------------------------------
#include "tier0/platform.h"
#include "kv3lib/kv3formats.h"

class KeyValues3;
class CUtlBuffer;
class CUtlString;

enum KV3CodecSaveFlags_t
{
	KV3_CODEC_SAVE_NONE = 0,

	// No "<!-- kv3 ... -->" header line
	KV3_CODEC_SAVE_NO_HEADER = 1 << 0,
};

// Text encoding; on failure pError gets "<name>:<line>:<col>: <message>"
bool KV3Codec_LoadText( KeyValues3 *pKV, CUtlString *pError, const char *pszInput, int nInputLength, const char *pszName );
bool KV3Codec_LoadText( KeyValues3 *pKV, CUtlString *pError, const CUtlBuffer &input, const char *pszName );
bool KV3Codec_SaveText( const KeyValues3 *pKV, CUtlBuffer &output, const KV3ID_t &format = g_KV3Format_Generic, uint nFlags = KV3_CODEC_SAVE_NONE );

// Binary encoding of kv3lib (not one of Valve's binary_* encodings): string table plus value tree, little-endian
bool KV3Codec_LoadBinary( KeyValues3 *pKV, CUtlString *pError, const void *pInput, int nInputLength, const char *pszName );
bool KV3Codec_SaveBinary( const KeyValues3 *pKV, CUtlBuffer &output );

// Picks the encoding by the leading bytes
bool KV3Codec_Load( KeyValues3 *pKV, CUtlString *pError, const void *pInput, int nInputLength, const char *pszName );

#endif // KV3TEXT_H
