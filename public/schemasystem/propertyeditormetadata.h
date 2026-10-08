#ifndef PROPERTYEDITORMETADATA_H
#define PROPERTYEDITORMETADATA_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Property editor tags. Location flags are where each tag was found; META_TAG_ON_METHOD is script-bound
// function metadata, which no lookup takes
//--------------------------------------------------------------------------------------------------
#include "schemasystem/schemametatag.h"

class IPropertyEditDomain;

enum PropertyAttrState_t
{
	PROPERTY_ATTR_STATE_DEFAULT = 0,
	PROPERTY_ATTR_STATE_READONLY,
	PROPERTY_ATTR_STATE_SUPPRESSED,
};

enum PropertyChangeDirtyResult_t
{
	PROPERTY_CHANGE_DIRTY_NONE = 0,
	PROPERTY_CHANGE_DIRTY_VALUES,
	PROPERTY_CHANGE_DIRTY_TOPOLOGY,
};

// Edits a class as a single string
struct PropertyClassAsStringFuncs_t
{
	typedef void ( *GetterFn_t )( IPropertyEditDomain *pDomain, const void *pObject, CUtlString *pOutString );
	typedef void ( *SetterFn_t )( IPropertyEditDomain *pDomain, void *pObject, const char *pszString );

	GetterFn_t m_pGetAsString;
	SetterFn_t m_pSetAsString;
};

typedef PropertyChangeDirtyResult_t ( *PropertyAttrChangeCallbackFn_t )( void *pObject );
typedef PropertyAttrState_t ( *PropertyAttrStateCallbackFn_t )( void *pObject );
typedef void ( *PropertyElementNameFn_t )( void *pObject, CUtlString *pOutName );

// Called on edit; tells what to refresh
// Element member that names the element in an array
DECLARE_SCHEMA_META_TAG( MPropertyArrayElementNameKey, META_TAG_ON_CLASS, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MPropertyAttrChangeCallback, META_TAG_ON_FIELD, META_VALUE( PropertyAttrChangeCallbackFn_t ) );

// Whether the field is shown, read-only or suppressed
DECLARE_SCHEMA_META_TAG( MPropertyAttrStateCallback, META_TAG_ON_FIELD, META_VALUE( PropertyAttrStateCallbackFn_t ) );

// Enum to choose from, e.g. "IKSolverType"
DECLARE_SCHEMA_META_TAG( MPropertyAttributeChoiceEnumName, META_TAG_ON_FIELD, META_VALUE( const char * ) );

// Kind of name to choose from, e.g. "Bone" or "FloatParameter"
DECLARE_SCHEMA_META_TAG( MPropertyAttributeChoiceName, META_TAG_ON_FIELD, META_VALUE( const char * ) );

// Editor widget, e.g. "VDataChoice( scripts/ammo.vdata )" or "ParticleConfigName()"
DECLARE_SCHEMA_META_TAG( MPropertyAttributeEditor, META_TAG_ON_FIELD | META_TAG_ON_METHOD, META_VALUE( const char * ) );

// Minimum and maximum, optionally prefixed, e.g. "0 255" or "biased 0.001 100"
DECLARE_SCHEMA_META_TAG( MPropertyAttributeRange, META_TAG_ON_FIELD | META_TAG_ON_METHOD, META_VALUE( const char * ) );

// Source of suggested values, e.g. "pulse_model_sequence_name"
DECLARE_SCHEMA_META_TAG( MPropertyAttributeSuggestionName, META_TAG_ON_FIELD | META_TAG_ON_METHOD, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MPropertyAutoExpandSelf, META_TAG_ON_CLASS | META_TAG_ON_FIELD, META_TAG_ONLY() );
DECLARE_SCHEMA_META_TAG( MPropertyAutoRebuildOnChange, META_TAG_ON_FIELD, META_TAG_ONLY() );
DECLARE_SCHEMA_META_TAG( MPropertyColorWithNoAlpha, META_TAG_ON_FIELD, META_TAG_ONLY() );

// Editor of the whole class or atomic, e.g. "multi_float( 4 )"
DECLARE_SCHEMA_META_TAG( MPropertyCustomEditor, META_TAG_ON_CLASS | META_TAG_ON_ATOMIC, META_VALUE( const char * ) );

// FGD type, e.g. "string", "sound" or "curve"
DECLARE_SCHEMA_META_TAG( MPropertyCustomFGDType, META_TAG_ON_CLASS | META_TAG_ON_FIELD | META_TAG_ON_ATOMIC, META_VALUE( const char * ) );

// Help text
DECLARE_SCHEMA_META_TAG( MPropertyDescription, META_TAG_ON_CLASS | META_TAG_ON_FIELD | META_TAG_ON_ENUMERATOR | META_TAG_ON_METHOD, META_VALUE( const char * ) );

// Converts the class to and from a string
DECLARE_SCHEMA_META_TAG( MPropertyEditClassAsString, META_TAG_ON_CLASS, META_VALUE( PropertyClassAsStringFuncs_t ) );

// Names a list element of the class
DECLARE_SCHEMA_META_TAG( MPropertyElementNameFn, META_TAG_ON_CLASS, META_VALUE( PropertyElementNameFn_t ) );

// Custom class editor, e.g. "StateMachinePropertyEditor"
DECLARE_SCHEMA_META_TAG( MPropertyExtendedEditor, META_TAG_ON_CLASS, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MPropertyFlattenIntoParentRow, META_TAG_ON_FIELD, META_TAG_ONLY() );

// Display name
DECLARE_SCHEMA_META_TAG( MPropertyFriendlyName, META_TAG_ON_CLASS | META_TAG_ON_FIELD | META_TAG_ON_ENUMERATOR | META_TAG_ON_ATOMIC | META_TAG_ON_METHOD, META_VALUE( const char * ) );

// Field group, e.g. "Water"
DECLARE_SCHEMA_META_TAG( MPropertyGroupName, META_TAG_ON_FIELD, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MPropertyHideField, META_TAG_ON_FIELD, META_TAG_ONLY() );

// Edit context the value provides, e.g. "ToolEditContext_ID_VMDL"
DECLARE_SCHEMA_META_TAG( MPropertyProvidesEditContextString, META_TAG_ON_FIELD, META_VALUE( const char * ) );

// Field order, e.g. 100 or 90
DECLARE_SCHEMA_META_TAG( MPropertySortPriority, META_TAG_ON_FIELD, META_VALUE( int ) );

// Opens a field group here, e.g. "+Model Setup/+Body Group"
DECLARE_SCHEMA_META_TAG( MPropertyStartGroup, META_TAG_ON_FIELD, META_VALUE( const char * ) );

// Hidden base class field, e.g. "m_iSlot"
DECLARE_SCHEMA_META_TAG( MPropertySuppressBaseClassField, META_TAG_ON_CLASS, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MPropertySuppressEnumerator, META_TAG_ON_ENUMERATOR, META_TAG_ONLY() );

// Hides the field when true, e.g. "m_bShouldDestroyOnDeath == false"
DECLARE_SCHEMA_META_TAG( MPropertySuppressExpr, META_TAG_ON_FIELD | META_TAG_ON_METHOD, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MPropertySuppressField, META_TAG_ON_FIELD, META_TAG_ONLY() );

// Never found placed; any location allowed
DECLARE_SCHEMA_META_TAG( MPropertyAutoExpandGroup, META_TAG_ON_CLASS | META_TAG_ON_FIELD | META_TAG_ON_METHOD | META_TAG_ON_ENUM | META_TAG_ON_ENUMERATOR | META_TAG_ON_ATOMIC, META_TAG_ONLY() );

// Value of unknown type built at startup; presence only
DECLARE_SCHEMA_META_TAG( MPropertyEditContextOverrideKey, META_TAG_ON_FIELD, META_TAG_ONLY() );
DECLARE_SCHEMA_META_TAG( MPropertyEditContextOverrideValue, META_TAG_ON_METHOD, META_TAG_ONLY() );

#endif // PROPERTYEDITORMETADATA_H
