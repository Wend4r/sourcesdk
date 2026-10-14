//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: This header disables the schema marker macros
// to prevent conflicts with libraries that use these names as identifiers,
// such as protobuf or glaze. Every header that includes schemaon.h includes
// this one at its end, so the macros do not leak to the includer.
// NOTE: Use schemaon.h to re-enable them.
//
// $NoKeywords: $
//=============================================================================//

// SPECIAL NOTE! This file must *not* use include guards; we need to be able
// to include this potentially multiple times (since we can reactivate the
// markers by including schemaon.h)

#undef schema
#undef valve_schema
#undef noschema
#undef schema_pragma
#undef META
#undef TYPEMETA
#undef META_USE_CODEGEN_TAG
#undef UNSCHEMATIZED_METHOD
#undef INTERNAL_SCHEMA_CLASS_MARKER_DATA
#undef DECLARE_SCHEMA_DATA_CLASS
