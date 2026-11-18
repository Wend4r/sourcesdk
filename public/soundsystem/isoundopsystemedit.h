//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef ISOUNDOPSYSTEMEDIT_H
#define ISOUNDOPSYSTEMEDIT_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "soundsystem/isoundopsystem.h"

class CUtlString;

// Sound event and stack editing for the tools (CSosEdit). Overloads are declared in the Linux order; MSVC
// reverses each overload set. Named by what the CS2 code does unless a log string names the function.
abstract_class ISoundOpSystemEdit : public IAppSystem
{
public:
	virtual SoundEventGuid_t StartSoundEvent( const char *pSoundEventName ) = 0;
	virtual void StopSoundEvent( SoundEventGuid_t guid, int nFlags ) = 0;
	virtual void StopAllSoundEvents() = 0;
	virtual bool IsSoundEventPlaying( SoundEventGuid_t guid ) = 0;
	// Forward to ISoundEventManager
	virtual bool AddSoundEvent( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding, bool bUpdate ) = 0;
	virtual void AddNewSoundEvent( const char *pSoundEventName, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void ListSoundEventsForResource( CUtlVector< const char * > &list, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void ListSoundEvents( CUtlVector< const char * > &list ) = 0;
	virtual void FixupResourceName( const char *pName, CUtlString &sName ) = 0;
	virtual void SetSoundEventUpdateStack( const char *pSoundEventName, const char *pStackName ) = 0;
	virtual int GetSoundEventBaseCount( const char *pSoundEventName ) = 0;
	// "ProcessSoundEvent: %s "base" sound event %s, has negative field offset"
	virtual bool SetSoundEventBase( const char *pSoundEventName, const char *pBaseName, uint32 nIndex ) = 0;
	virtual bool SetSoundEventBase( const char *pSoundEventName, void *pContext, const char *pBaseName, uint32 nIndex ) = 0;
	virtual bool GetSoundEventBaseDisplayName( const char *pSoundEventName, CUtlString &sName, uint32 nIndex ) = 0;
	virtual void ClearSoundEventBase( const char *pSoundEventName, uint32 nIndex ) = 0;
	virtual void RemoveSoundEventBase( const char *pSoundEventName, int nIndex ) = 0;
	virtual void MoveSoundEventBase( const char *pSoundEventName, int nFrom, int nTo ) = 0;
	virtual void SetSoundEventPreloadVsnd( const char *pSoundEventName, bool bPreload ) = 0;
	virtual bool SoundEventHasPreloadVsnd( const char *pSoundEventName ) = 0;
	virtual int GetStackCount() = 0;
	virtual int GetSoundEventUpdateGroupCount( const char *pSoundEventName ) = 0;
	virtual void SetSoundEventUpdateGroupStack( const char *pSoundEventName, const char *pStackName, int nGroup ) = 0;
	virtual bool HasSoundEventUpdateGroup( const char *pSoundEventName, const char *pStackName ) = 0;
	virtual void RemoveSoundEventUpdateGroup( const char *pSoundEventName, int nGroup ) = 0;
	virtual void GetSoundEventUpdateGroupNames( const char *pSoundEventName, CUtlVector< CUtlString > &names ) = 0;
	// "public." fields with their "_stack_properties" / "_field_properties" (display_category, advanced, locked)
	virtual void GetSoundEventPublicFields( const char *pSoundEventName, CUtlVector< CUtlString > &fields ) = 0;
	virtual bool GetSoundEventFieldDisplayName( const char *pSoundEventName, const char *pFieldName, CUtlString &sName ) = 0;
	virtual bool SoundEventHasStackField( const char *pSoundEventName, const char *pFieldName ) = 0;
	virtual bool IsSoundEventStackFieldPublic( const char *pSoundEventName, const char *pFieldName ) = 0;
	virtual bool HasSoundEventStaticField( const char *pSoundEventName, const char *pFieldName ) = 0;
	virtual bool HasSoundEventDynamicField( const char *pSoundEventName, const char *pFieldName ) = 0;
	virtual int GetSoundEventFieldArrayCount( const char *pSoundEventName, const char *pFieldName, int16 nIndex ) = 0;
	virtual int GetSoundEventFieldCount( const char *pSoundEventName, const char *pFieldName ) = 0;
	virtual bool GetSoundEventFieldString( const char *pSoundEventName, const char *pFieldName, char **ppValue, int16 nIndex ) = 0;
	virtual bool SetSoundEventFieldFromString( const char *pSoundEventName, const char *pFieldName, const char *pValue ) = 0;
	virtual bool RemoveSoundEventField( const char *pSoundEventName, const char *pFieldName, int nIndex ) = 0;
	virtual bool InsertSoundEventFieldElement( const char *pSoundEventName, const char *pFieldName, uint32 nIndex ) = 0;
	virtual bool RemoveSoundEventFieldElement( const char *pSoundEventName, const char *pFieldName, uint32 nIndex ) = 0;
	virtual bool MoveSoundEventFieldElement( const char *pSoundEventName, const char *pFieldName, int nFrom, int nTo ) = 0;
	virtual bool GetSoundEventFieldElements( const char *pSoundEventName, const char *pFieldName, CUtlVector< CUtlString > &elements ) = 0;
	// Returns where the field comes from (1 static, 2 base, 3 stack, 4-5 dynamic); the callers below branch on it
	virtual int GetSoundEventFieldSource( const char *pSoundEventName, const char *pFieldName, uint32 *pCount, int nIndex ) = 0;
	virtual bool SetSoundEventFieldValue( const char *pSoundEventName, const char *pFieldName, const void *pValue, int16 nIndex ) = 0;
	virtual bool ResetSoundEventFieldValue( const char *pSoundEventName, const char *pFieldName ) = 0;
	virtual bool GetStackFieldDefault( HSOSLIBSTACKHASH hStack, const char *pFieldName, void **ppValue, int nIndex ) = 0;
	virtual bool RemoveSoundEventFieldValue( const char *pSoundEventName, const char *pFieldName, uint32 nIndex ) = 0;
	virtual bool SetSoundEventParamForEdit( SoundEventGuid_t guid, uint32 nFieldToken, const void *pData, int16 nIndex ) = 0;
	virtual void EditGetDomainAttrInfo( void *pDomain, void *pAttr, void *pInfo, void *pContext ) = 0;
	virtual void EditGetDomainAttrInfo( void *pDomain, uint32 nStack, int nField, void *pInfo, void *pContext ) = 0;
	// Reports a missing field as EditGetDomainAttrInfo
	virtual void EditGetDomainAttrInfo( const char *pSoundEventName, uint32 nStack, void *pAttr, void *pInfo ) = 0;
	virtual bool EditGetDomainAttrChoice( void *pDomain, uint32 nStack, int nChoice ) = 0;
	virtual bool EditGetDomainAttrChoice( void *pDomain, uint32 nStack, int nChoice, CUtlVector< CUtlString > *pChoices ) = 0;
	// "_stack_properties" / "_field_properties" of the stacks
	virtual bool EditGetStackProperty( const char *pStackName, const char *pPropertyName, void *pValue ) = 0;
	virtual bool EditGetStackFieldProperty( const char *pStackName, const char *pFieldName, const char *pPropertyName, void *pValue ) = 0;
	virtual bool EditGetStackFieldProperty( const char *pStackName, const char *pFieldName, const char *pPropertyName ) = 0;
	virtual void GetStackName( HSOSLIBSTACKHASH hStack, CUtlString &sName ) = 0;
	virtual void GetStackNames( CUtlVector< CUtlString > &names ) = 0;
	// "operator_override"
	virtual void GetOperatorOverrideNames( CUtlVector< CUtlString > &names ) = 0;
	virtual void GetOperatorOverrideFields( CUtlVector< CUtlString > &names, CUtlVector< CUtlString > &fields ) = 0;
	virtual void GetDeferredStackNames( CUtlVector< CUtlString > &names ) = 0;
	virtual void GetLibraryStackNames( CUtlVector< CUtlString > &names ) = 0;
	// "display_game" of the stacks
	virtual void GetDisplayGames( CUtlVector< CUtlString > &names ) = 0;
	virtual void GetStackOperatorNames( HSOSLIBSTACKHASH hStack, CUtlVector< CUtlString > &names ) = 0;
	virtual bool GetStackOperatorName( CUtlString &sName, HSOSLIBSTACKHASH hStack, void *pOperator ) = 0;
	virtual int GetSoundEventFieldValueStrings( SoundEventGuid_t guid, const char *pFieldName, CUtlVector< CUtlString > &values ) = 0;
	virtual int GetStackFieldValueStrings( HSOSLIBSTACKHASH hStack, const char *pFieldName, CUtlVector< CUtlString > &values ) = 0;
	virtual int GetActiveSoundEventFieldStrings( void *pContext, int nIndex, const char *pFieldName, CUtlVector< CUtlString > &values ) = 0;
	virtual bool IsStackFieldPublic( HSOSLIBSTACKHASH hStack, const char *pFieldName ) = 0;
	virtual bool IsStackFieldEditable( HSOSLIBSTACKHASH hStack, const char *pFieldName ) = 0;
	virtual bool GetStackPublicFieldString( HSOSLIBSTACKHASH hStack, const char *pFieldName, CUtlString &sValue, int nIndex ) = 0;
	virtual bool GetStackFieldStringByIndex( HSOSLIBSTACKHASH hStack, int nField, CUtlString &sValue, int nIndex ) = 0;
	virtual const char *GetStackPublicFieldNameByIndex( HSOSLIBSTACKHASH hStack, int nField ) = 0;
	virtual void *GetStackFieldData( HSOSLIBSTACKHASH hStack, int *pCount ) = 0;
	virtual void *FindStackOperator( HSOSLIBSTACKHASH hStack, const char *pOperatorName ) = 0;
	virtual SOFieldDataType_t GetStackFieldType( HSOSLIBSTACKHASH hStack, uint32 nFieldToken ) = 0;
	virtual CSosOperatorStack *GetStack( HSOSLIBSTACKHASH hStack ) = 0;
	// Text of a stack: whole stack, its operators, its fields
	virtual void SerializeStack( HSOSLIBSTACKHASH hStack, CUtlString &sText ) = 0;
	virtual void SerializeStackOperators( HSOSLIBSTACKHASH hStack, CUtlString &sText ) = 0;
	virtual void SerializeStackFields( HSOSLIBSTACKHASH hStack, CUtlString &sText ) = 0;
	// Stops every sound event and reloads the sound system data
	virtual void ReloadSoundSystemData() = 0;
	virtual void FieldValueToString( CUtlString &sValue, const char **ppValue, SOFieldDataType_t nType ) = 0;
	virtual const char *GetSoundDataPath() = 0;
	virtual const char *GetSoundScriptPath() = 0;
	// "scripts/soundmixers.txt"
	virtual void ReloadMixGroups() = 0;
	virtual void *GetEditInterface() = 0;
	virtual void GetMixGraphNames( CUtlVector< CUtlString > &names ) = 0;
	virtual int FindEditFieldIndex( const char *pFieldName ) = 0;
	// "AddStackKV3: Cannot find "operators" array in stack %s"
	virtual void AddStackKV3( KeyValues3 *pStack, int nFlags ) = 0;
	virtual void FlushEdits() = 0;
	virtual void *GetStackList() = 0;
};

#endif // ISOUNDOPSYSTEMEDIT_H
