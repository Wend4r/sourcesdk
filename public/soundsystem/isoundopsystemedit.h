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
#include "mathlib/vector2d.h"
#include "soundsystem/isoundopsystem.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"

#include "schemacompiler/schemaon.h"

class CKV3MemberName;
class CSosOperatorSystem;
class CUtlString;
class CUtlSymbolTable;
class KeyValues;
class KeyValues3;
struct ResourceBindingBase_t;
struct grouprule_t;

schema enum SosEditItemType_t
{
	SOS_EDIT_ITEM_TYPE_SOUNDEVENTS = 0,
	SOS_EDIT_ITEM_TYPE_SOUNDEVENT,
	SOS_EDIT_ITEM_TYPE_LIBRARYSTACKS,
	SOS_EDIT_ITEM_TYPE_STACK,
	SOS_EDIT_ITEM_TYPE_OPERATOR,
	SOS_EDIT_ITEM_TYPE_FIELD,
};

schema struct SosEditItemInfo_t
{
	DECLARE_SCHEMA_DATA_CLASS( SosEditItemInfo_t );

	SosEditItemType_t itemType;
	CUtlString itemName;
	CUtlString itemTypeName;
	noschema KeyValues *pItemKV;
	CUtlString itemKVString;
	Vector2D itemPos;
};

schema struct SelectedEditItemInfo_t
{
	DECLARE_SCHEMA_DATA_CLASS( SelectedEditItemInfo_t );

	CUtlVector< SosEditItemInfo_t > m_EditItems;
};

// Field of an operator of a stack, as the edit interface lists it
struct SosEditFieldInfo_t
{
	const char *m_pszName;
	SOFieldDataType_t m_nType;
	uint8 m_nFlags;
	// Value of the field as text
	CUtlString m_sValue;
	// Offset of the field in the data of its operator
	int m_nOperatorOffset;
	// Offset of the field in the field data of the stack
	int m_nStackOffset;
	// Stack offset of the output the input field is connected to, -1 when it is not connected
	int m_nConnectedOffset;
};

enum MXRMixGroupFields_t
{
	MXR_MIXGROUP_NONE = 0,
	MXR_MIXGROUP_VOL,
	MXR_MIXGROUP_LEVEL,
	MXR_MIXGROUP_DSP,
	MXR_MIXGROUP_SOLO,
	MXR_MIXGROUP_MUTE,
};

// Ducking values of the rule of a mix group, as UpdateAllDuckerVolumes leaves them
struct MixGroupDuckState_t
{
	// Sum of the mix trigger volumes of the group when the rule has causes_ducking, the group ducks the groups of
	// lower priority above its ducker_threshold
	float m_flTotalVolume;
	// Sum of the mix trigger volumes of the group, their maximum with max_trigger_amounts
	float m_flTriggerVolume;
	// Current duck volume, ramped towards the duck_to_percent of the rule while a group of higher priority ducks it
	float m_flDuckVolume;
	// is_ducked of the rule
	uint16 m_nIsDucked;
};

// Sound mixers, mix layers and mix groups of the sound system (128 mix groups, 32 mix layers)
abstract_class ISoundMixGroupSystem
{
public:
	virtual const char *GetCurrentSoundMixer() = 0;
	virtual void SetCurrentSoundMixer( const char *pszSoundMixer ) = 0;
	virtual void SetMixGroupOfMixLayer( int nMixGroupIndex, int nMixLayerIndex, MXRMixGroupFields_t nMixGroupField, float flValue ) = 0;
	// Volume a mix trigger operator of the sound event adds to the eight mix groups of pMixGroupIds each ducker update,
	// as if a sound of that volume played in them; stopping the sound event removes it
	virtual void SetMixTrigger( SoundEventGuid_t guid, float flVolume, const int *pMixGroupIds ) = 0;
	virtual void RemoveMixTrigger( SoundEventGuid_t guid ) = 0;
	// Runs at most at the update rate of its convar
	virtual void UpdateAllDuckerVolumes( float flFrameTime ) = 0;
	// Mix groups the current sound mixer sets
	virtual void GetMixGroupNames( CUtlVector< CUtlString > &list ) = 0;
	// Lowest duck volume of the mix groups, pMixGroupIds holds eight ids terminated by -1
	virtual void GetDuckVolume( const int *pMixGroupIds, float *pflVolume ) = 0;
	// Fills the eight mix group ids of the rules that match the sound name and the entity class name
	virtual void GetMixGroupsFromSoundSource( int *pMixGroupIds, const char *pszSoundName, const char *pszClassName, bool bLocalPlayer ) = 0;
	// Fills ten floats: the values of the mix group in the current sound mixer and its mix layers
	virtual void GetValuesFromMixGroupIndex( float *pflValues, int nMixGroupIndex ) = 0;
	virtual grouprule_t *GetMixGroupRule( int nMixGroupId ) = 0;
	// -1 when no rule has the name; GetMixgroupFromName also warns
	virtual int GetMixGroupIdFromName( const char *pszGroupName ) = 0;
	virtual int GetMixLayerIndexFromName( const char *pszMixLayerName ) = 0;
	virtual void SetMixLayerLevel( int nIndex, float flLevel ) = 0;
	virtual int GetMixgroupFromName( const char *pszGroupName ) = 0;
	virtual float GetMixLayerLevel( int nIndex ) = 0;
	virtual void GetMixGroupDuckState( int nMixGroupId, MixGroupDuckState_t *pState ) = 0;
	// pflValues holds the volume, level, dsp, solo and mute of the mix group
	virtual void SetMixGroupOfSoundMixer( const char *pszSoundMixer, int nMixGroupIndex, const float *pflValues ) = 0;
	// Directory name of the rule of the mix group
	virtual void GetMixGroupDirName( const char *pszGroupName, CUtlString &sDirName ) = 0;
};

class CSoundMixGroupSystem : public ISoundMixGroupSystem
{
};

abstract_class ISoundOpSystemEdit : public IAppSystem
{
public:
	virtual SndOpEventGuid_t StartSoundEvent( const char *pSoundEventName ) = 0;
	virtual bool StopSoundEvent( SoundEventGuid_t guid ) = 0;
	// Stops every sound event
	virtual void StopAllSoundEvents() = 0;
	virtual bool IsSoundEventPlaying( SoundEventGuid_t guid ) = 0;
	// Forward to ISoundEventManager, AddSoundEvent with bUpdate and without bNoDefer
	virtual bool AddSoundEvent( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void AddNewSoundEvent( const char *pSoundEventName, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void ListSoundEventsForResource( CUtlVector< const char * > &list, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void ListSoundEvents( CUtlVector< const char * > &list ) = 0;
	virtual void FixupResourceName( const char *pName, CUtlString &sName ) = 0;
	// Sets the update stack of the sound event from the stack name
	virtual void SetSoundEventUpdateStack( const char *pSoundEventName, const char *pStackName ) = 0;
	// Base count of the sound event definition
	virtual int GetSoundEventDefinitionBaseCount( const char *pSoundEventName ) = 0;
	// Inserts a base sound event at nBase
	virtual bool InsertSoundEventDefinitionBase( const char *pSoundEventName, const char *pBaseSoundEventName, int nBase ) = 0;
	virtual bool InsertSoundEventDefinitionBase( const char *pSoundEventName, const char *pFieldName, const char *pBaseSoundEventName, int nBase ) = 0;
	// Formats the name and the field of the base as "name : field"
	virtual void GetSoundEventDefinitionBaseLabel( const char *pSoundEventName, CUtlString &sName, int nBase ) = 0;
	// Inserts an empty base, removes a base and swaps two bases
	virtual void AddEmptySoundEventDefinitionBase( const char *pSoundEventName, int nBase ) = 0;
	virtual void RemoveSoundEventDefinitionBase( const char *pSoundEventName, int nBase ) = 0;
	virtual void SwapSoundEventDefinitionBases( const char *pSoundEventName, int nBase1, int nBase2 ) = 0;
	virtual void SetSoundEventPreloadVsnd( const char *pSoundEventName, bool bPreloadVsnd ) = 0;
	virtual bool SoundEventHasPreloadVsnd( const char *pSoundEventName ) = 0;
	// Names of every update group
	virtual void GetUpdateGroupNames( CUtlVector< CUtlString > &list ) = 0;
	// Update groups of the sound event: count, set at nIndex, names and name at nIndex
	virtual int GetSoundEventUpdateGroupCount( const char *pSoundEventName ) = 0;
	virtual void SetSoundEventUpdateGroup( const char *pSoundEventName, const char *pGroupName, int nIndex ) = 0;
	virtual void GetSoundEventUpdateGroupNames( const char *pSoundEventName, CUtlVector< const char * > &list ) = 0;
	virtual const char *GetSoundEventUpdateGroupName( const char *pSoundEventName, int nIndex ) = 0;
	// Names of the vsnds of the sound event
	virtual void GetSoundEventVSndNames( const char *pSoundEventName, CUtlVector< const char * > &list ) = 0;
	// Public field names of the update stack of the sound event, then the names of its own fields
	virtual void GetSoundEventAllFieldNames( const char *pSoundEventName, CUtlVector< CUtlString > *pList ) = 0;
	// First name of GetSoundEventAllFieldNames whose field has the type
	virtual bool FindSoundEventFieldOfType( const char *pSoundEventName, SOFieldDataType_t nType, CUtlString &sFieldName ) = 0;
	// Field editing of the sound event, AddSoundEventField adds the field with the values of its stack
	virtual void AddSoundEventField( const char *pSoundEventName, const char *pFieldName ) = 0;
	virtual void RemoveSoundEventField( const char *pSoundEventName, const char *pFieldName ) = 0;
	// Field names of the definition of the sound event
	virtual void GetSoundEventDefinitionFieldNames( const char *pSoundEventName, CUtlVector< CUtlString > *pList ) = 0;
	// Array fields the sound event sets, their element counts and removal
	virtual void GetSoundEventArrayFieldNames( const char *pSoundEventName, CUtlVector< CUtlString > *pList ) = 0;
	virtual void AddSoundEventArrayField( const char *pSoundEventName, const char *pFieldName, int16 nCount ) = 0;
	virtual void RemoveSoundEventArrayField( const char *pSoundEventName, const char *pFieldName ) = 0;
	// Sets an element of the field from its text form
	virtual bool SetSoundEventFieldValue( const char *pSoundEventName, const char *pFieldName, const CUtlString &sValue, int16 nIndex ) = 0;
	// Element count of the field, pbArray tells whether it is an array field
	virtual int GetSoundEventFieldCount( const char *pSoundEventName, const char *pFieldName, bool *pbArray ) = 0;
	// Element editing of an array field: resize, insert, remove and swap
	virtual void SetSoundEventArrayFieldCount( const char *pSoundEventName, const char *pFieldName, int nCount ) = 0;
	virtual void InsertSoundEventArrayFieldElement( const char *pSoundEventName, const char *pFieldName, int nIndex ) = 0;
	virtual void RemoveSoundEventArrayFieldElement( const char *pSoundEventName, const char *pFieldName, int nIndex ) = 0;
	virtual void SwapSoundEventArrayFieldElements( const char *pSoundEventName, const char *pFieldName, int nIndex1, int nIndex2 ) = 0;
	// Value names of an enum or mix group field of the update stack of the sound event
	virtual void GetSoundEventFieldValueNames( const char *pSoundEventName, const char *pFieldName, CUtlVector< CUtlString > *pList ) = 0;
	// State of the field for the sound event: 0 or 1 whether the definition sets the scalar field, 2 or 3 whether it sets the array
	// field, 4 for an index past the element count and 5 for a field the update stack does not have; pnCount receives the element count
	virtual int GetSoundEventFieldState( const char *pSoundEventName, const char *pFieldName, int *pnCount, int nIndex ) = 0;
	// Calls SetSoundEventFieldValue when the state of GetSoundEventFieldState allows it
	virtual bool TrySetSoundEventFieldValue( const char *pSoundEventName, const char *pFieldName, const CUtlString &sValue, int16 nIndex ) = 0;
	// Removes the field from the definition, depending on the state of GetSoundEventFieldState
	virtual void ResetSoundEventField( const char *pSoundEventName, const char *pFieldName ) = 0;
	// Value of a public field of the stack and its element count
	virtual int GetStackPublicFieldData( HSOSLIBSTACKHASH hStack, const char *pFieldName, SosFieldValueMem_t **ppData, int nIndex ) = 0;
	// Element removal, depending on the state of GetSoundEventFieldState
	virtual void RemoveSoundEventFieldElement( const char *pSoundEventName, const char *pFieldName, int nIndex ) = 0;
	// Calls ISoundOpSystem::SetSoundEventParam and counts the change for the editor
	virtual bool SetSoundEventParam( SoundEventGuid_t guid, HSOSOPFIELDHASH nFieldToken, const CSosFieldData *pData, int16 nIndex ) = 0;
	// Attribute info and field type of a field of the stack; nField indexes the names of GetSoundEventAllFieldNames, which fill pList when it is empty.
	// pInfo receives the field type, the name, the display and property groups, the help text and the flags of the field
	virtual void EditGetDomainAttrInfo( const char *pSoundEventName, HSOSLIBSTACKHASH hStack, int nField, void *pInfo ) = 0;
	virtual SOFieldDataType_t EditGetDomainAttrType( const char *pSoundEventName, HSOSLIBSTACKHASH hStack, int nField ) = 0;
	virtual void EditGetDomainAttrInfo( const char *pSoundEventName, HSOSLIBSTACKHASH hStack, int nField, void *pInfo, CUtlVector< CUtlString > *pList ) = 0;
	virtual SOFieldDataType_t EditGetDomainAttrType( const char *pSoundEventName, HSOSLIBSTACKHASH hStack, int nField, CUtlVector< CUtlString > *pList ) = 0;
	virtual void EditGetDomainAttrInfo( const char *pSoundEventName, HSOSLIBSTACKHASH hStack, void *pInfo, const CKV3MemberName *pFieldName ) = 0;
	// Values of the "_stack_properties" table: a key of the stack, and a key of a field of the stack from its "_field_properties";
	// the overload without sValue tells whether the key exists
	virtual bool GetStackProperty( const char *pStackName, const char *pKey, CUtlString &sValue ) = 0;
	virtual bool GetStackFieldProperty( const char *pStackName, const char *pFieldName, const char *pKey, CUtlString &sValue ) = 0;
	virtual bool GetStackFieldProperty( const char *pStackName, const char *pFieldName, const char *pKey ) = 0;
	// Mod the library stack was loaded from
	virtual void GetStackModName( const char *pStackName, CUtlString &sValue ) = 0;
	// Lists of names, the char * elements are copies the caller deletes: library stacks, operator types
	// (GetOperatorTypes) with "operator_override", the same with their indices, and the field data types ("none", "bool", ... "public_opvar")
	virtual void CopyLibraryStackNames( CUtlVector< char * > &list ) = 0;
	virtual void CopyOperatorTypeNames( CUtlVector< char * > &list ) = 0;
	virtual void CopyOperatorTypeNamesAndIndices( CUtlVector< int > &indices, CUtlVector< char * > &names ) = 0;
	virtual void CopyFieldDataTypeNames( CUtlVector< char * > &list ) = 0;
	// Names of the library stacks
	virtual void GetLibraryStackNames( CUtlVector< const char * > &list ) = 0;
	// Library stacks whose "display_game" value is the game name
	virtual void GetGameLibraryStackNames( CUtlVector< CUtlString > &list ) = 0;
	// Names of the operators of the stack
	virtual void GetStackOperatorNames( HSOSLIBSTACKHASH hStack, CUtlVector< CUtlString > &list ) = 0;
	// Type name of the operator, such as "vmix_voice_time"
	virtual CUtlString GetStackOperatorTypeName( HSOSLIBSTACKHASH hStack, const char *pOperatorName ) = 0;
	// Fields of the operator of the library stack or of the active sound event
	virtual void GetOperatorFields( HSOSLIBSTACKHASH hStack, const char *pOperatorName, CUtlVector< SosEditFieldInfo_t > *pFields ) = 0;
	virtual void GetOperatorFields( SoundEventGuid_t guid, const char *pOperatorName, CUtlVector< SosEditFieldInfo_t > *pFields ) = 0;
	// Values of the fields of the operator for the stack at nIndex (from the end when negative) of ISoundOpSystem::GetSoundEventHistory
	virtual void GetSoundEventHistoryOperatorFieldValues( SoundEventGuid_t guid, int nIndex, const char *pOperatorName, CUtlVector< CUtlString > *pList ) = 0;
	// Public field names of the stack, GetStackPublicOpvarNames lists only the SOFTYPE_PUBLIC_OPVAR ones
	virtual void GetStackPublicFieldNames( HSOSLIBSTACKHASH hStack, CUtlVector< const char * > *pList ) = 0;
	virtual void GetStackPublicOpvarNames( HSOSLIBSTACKHASH hStack, CUtlVector< const char * > *pList ) = 0;
	// Text of a public field of the stack, nField indexes the list of GetStackPublicFieldNames
	virtual void GetStackPublicFieldString( HSOSLIBSTACKHASH hStack, int nField, CUtlString &sValue, int nIndex ) = 0;
	virtual void GetStackPublicFieldString( HSOSLIBSTACKHASH hStack, const char *pFieldName, CUtlString &sValue, int nIndex ) = 0;
	// Element count of a public field of the stack
	virtual int GetStackPublicFieldCount( HSOSLIBSTACKHASH hStack, int nField ) = 0;
	// Field data of the stack and its size
	virtual SosFieldValueMem_t *GetStackData( HSOSLIBSTACKHASH hStack, int *pnSize ) = 0;
	virtual bool StackHasField( HSOSLIBSTACKHASH hStack, HSOSOPFIELDHASH nFieldToken ) = 0;
	virtual bool StackHasField( const char *pStackName, const char *pFieldName ) = 0;
	// Fills a field entry of GetOperatorFields, the lookup is empty and it returns false
	virtual bool GetStackFieldInfo( HSOSLIBSTACKHASH hStack, uint32 nUnk, const void *pUnk, SosEditFieldInfo_t *pField ) = 0;
	// Names of the inputs, the outputs and the other fields (such as "voice_layer") of the operator type, the char * elements
	// are copies the caller deletes
	virtual void GetOperatorInputNames( const char *pName, CUtlVector< char * > &list ) = 0;
	virtual void GetOperatorOutputNames( const char *pName, CUtlVector< char * > &list ) = 0;
	virtual void GetOperatorFieldNames( const char *pName, CUtlVector< char * > &list ) = 0;
	// Stops every sound event, empties the library stacks and returns the SoundSystem/UsePlatTime setting
	virtual bool ClearLibraryStacks() = 0;
	// Text of a field value, characters other than digits, '-', '.' and ' ' become spaces for the float types
	virtual CUtlString SanitizeFieldValueString( const CUtlString &sValue, SOFieldDataType_t nType ) = 0;
	// Listener array of the audio state of the sound system (60 bytes per listener) and the vectors the mixer derives from it
	virtual void *GetAudioStateListeners() = 0;
	virtual void *GetAudioStateListenerVectors() = 0;
	virtual bool LoadAllSoundMixers() = 0;
	virtual CSoundMixGroupSystem *GetSoundMixGroupSystem() = 0;
	// Adds the names of the voice graphs that do not start with an underscore
	virtual void GetVoiceGraphNames( CUtlSymbolTable *pNames ) = 0;
	// Number of submixes of the voice graph with the name, 0 when there is none
	virtual int GetVoiceGraphSubmixCount( const char *pName ) = 0;
	// Replaces the library stack with the one in pStack
	virtual bool ReplaceLibraryStack( const char *pStackName, KeyValues3 *pStack ) = 0;
	virtual CSosOperatorSystem *GetOperatorSystem() = 0;
	// Table of the operator types
	virtual void *GetOperatorTypes() = 0;
};

class CSosEdit : public CTier2AppSystem< ISoundOpSystemEdit >
{
};

#include "schemacompiler/schemaoff.h"

#endif // ISOUNDOPSYSTEMEDIT_H
