//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: This header (re-)enables the schema marker macros.
// NOTE: Use schemaoff.h to disable them.
//
// These macros are plain identifiers, so they clash with third-party code
// that uses the same names, such as protobuf or glaze. To keep them from
// leaking, a header that uses them includes schemaon.h after its own includes
// and schemaoff.h at its end. A source file that uses them, directly or
// through a macro like CLASS_USES_KV3TRANSFER_DATA, includes schemaon.h after
// its last include, so third-party headers above it never see the macros:
//
//	// myclass.cpp
//	#include "tier0/basetypes.h"
//
//	#include "schemacompiler/schemaon.h"
//
//	schema class CMyClass
//	{
//	public:
//		CLASS_USES_KV3TRANSFER_DATA( CMyClass );
//
//		int m_nValue; META( MNotSaved );
//	};
//
// $NoKeywords: $
//=============================================================================//

// SPECIAL NOTE! This file must *not* use include guards; we need to be able
// to include this potentially multiple times (since we can deactivate the
// markers by including schemaoff.h)

#ifndef valve_schema
#define valve_schema namespace ValveSchemaMarker {}
#endif

#ifndef schema
#define schema valve_schema
#endif

#ifndef noschema
#define noschema
#endif

#ifndef schema_pragma
#define schema_pragma( ... )
#endif

#ifndef META
#define META( ... )
#endif

#ifndef TYPEMETA
#define TYPEMETA( ... )
#endif

#ifndef META_USE_CODEGEN_TAG
#define META_USE_CODEGEN_TAG( ... )
#endif

#ifdef COMPILING_SCHEMA
#define UNSCHEMATIZED_METHOD( x )
#define INTERNAL_SCHEMA_CLASS_MARKER_DATA `__schema_class_marker_data__`
#else
#define UNSCHEMATIZED_METHOD( x ) x
#define INTERNAL_SCHEMA_CLASS_MARKER_DATA
#endif

// Data classes (no vtable)
#define DECLARE_SCHEMA_DATA_CLASS( _className ) \
	INTERNAL_SCHEMA_CLASS_MARKER_DATA
