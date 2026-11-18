#ifndef KV3FORMATS_H
#define KV3FORMATS_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "tier0/platform.h"

// KeyValues3 identifies encodings and formats by a UUID plus a human-friendly name.
// The name exists only in the text encoding; the binary encoding carries the UUID alone.
struct KV3ID_t
{
	const char* m_name;
	uint64		m_data1;
	uint64		m_data2;
};

// encodings
const KV3ID_t g_KV3Encoding_Text 		= { "text", 0x41C58A33E21C7F3Cull, 0xDAA323A6DA77799ull };
const KV3ID_t g_KV3Encoding_Binary 		= { "binary", 0x40C1F7D81B860500ull, 0x14E76782A47582ADull };
const KV3ID_t g_KV3Encoding_BinaryLZ4 	= { "binary_lz4", 0x4F5C63A16847348Aull, 0x19B1D96F805397A1ull };
const KV3ID_t g_KV3Encoding_BinaryZSTD 	= { "binary_zstd", 0x4305FEF06F620A00ull, 0x29DBB14623045FA3ull };
const KV3ID_t g_KV3Encoding_BinaryBC 	= { "binary_bc", 0x4F6C95BC95791A46ull, 0xD2DFB7A1BC050BA7ull };
const KV3ID_t g_KV3Encoding_BinaryAuto 	= { "binary_auto", 0x45836B856EB109E6ull, 0x8C06046E3A7012A3ull };

// formats
const KV3ID_t g_KV3Format_Generic = { "generic", 0x469806E97412167Cull, 0xE73790B53EE6F2AFull };

#endif // KV3FORMATS_H
