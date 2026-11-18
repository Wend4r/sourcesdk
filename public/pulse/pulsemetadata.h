#ifndef PULSEMETADATA_H
#define PULSEMETADATA_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Pulse tags. Location flags are where each tag was found; META_TAG_ON_METHOD is script-bound function metadata,
// which no lookup takes
//--------------------------------------------------------------------------------------------------
#include "schemasystem/schemametatag.h"

DECLARE_SCHEMA_META_TAG( MPulseAdvanced, META_TAG_ON_METHOD, META_TAG_ONLY() );
DECLARE_SCHEMA_META_TAG( MPulseCellInflow_IsDefault, META_TAG_ON_METHOD, META_TAG_ONLY() );
DECLARE_SCHEMA_META_TAG( MPulseCursorTerminates, META_TAG_ON_METHOD, META_TAG_ONLY() );

// Pulse editor canvas spec of the node, e.g. "{ item_factory = 'IntervalTimer' }"
DECLARE_SCHEMA_META_TAG( MPulseEditorCanvasItemSpecKV3, META_TAG_ON_CLASS | META_TAG_ON_METHOD, META_VALUE( const char * ) );

// Node header built from its parameters, e.g. ""Find " + entityType + " Entity"
DECLARE_SCHEMA_META_TAG( MPulseEditorHeaderExpr, META_TAG_ON_METHOD, META_VALUE( const char * ) );

// Node header helper, e.g. "Helper_PlayVOLine"
DECLARE_SCHEMA_META_TAG( MPulseEditorHeaderHelper, META_TAG_ON_CLASS, META_VALUE( const char * ) );

// Node header icon, e.g. "tools/images/pulse_editor/node_timer.png"
DECLARE_SCHEMA_META_TAG( MPulseEditorHeaderIcon, META_TAG_ON_CLASS | META_TAG_ON_METHOD, META_VALUE( const char * ) );

// Sub-header labels and their fields, e.g. "{ 'TagName'='m_TagName' }"
DECLARE_SCHEMA_META_TAG( MPulseEditorSubHeaderText, META_TAG_ON_CLASS, META_VALUE( const char * ) );

// Function name in Pulse expressions, e.g. "fmt"
DECLARE_SCHEMA_META_TAG( MPulseExpressionAlias, META_TAG_ON_METHOD, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MPulseFGDSkipField, META_TAG_ON_FIELD | META_TAG_ON_METHOD, META_TAG_ONLY() );
DECLARE_SCHEMA_META_TAG( MPulseFunctionAddsCursorTag, META_TAG_ON_METHOD, META_TAG_ONLY() );
DECLARE_SCHEMA_META_TAG( MPulseFunctionHiddenInTool, META_TAG_ON_CLASS | META_TAG_ON_METHOD, META_TAG_ONLY() );

// Former name of the function; may repeat
DECLARE_SCHEMA_META_TAG( MPulseLegacyName, META_TAG_ON_METHOD, META_VALUE( const char * ) );

// Parameter the return type follows, e.g. "entityType:to_subtype"
DECLARE_SCHEMA_META_TAG( MPulsePolymorphicDependentReturn, META_TAG_ON_METHOD, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MPulseRequirementCheck, META_TAG_ON_METHOD, META_TAG_ONLY() );
DECLARE_SCHEMA_META_TAG( MPulseRequirementCommit, META_TAG_ON_METHOD, META_TAG_ONLY() );

// Outflow field the function is the signature of, e.g. "m_OnFired"
DECLARE_SCHEMA_META_TAG( MPulseSignatureForOutflow, META_TAG_ON_METHOD, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MPulseValuePure, META_TAG_ON_METHOD, META_TAG_ONLY() );

#endif // PULSEMETADATA_H
