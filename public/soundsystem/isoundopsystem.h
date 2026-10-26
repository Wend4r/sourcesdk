//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef ISOUNDOPSYSTEM_H
#define ISOUNDOPSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlhashtable.h"
#include "tier0/murmurhash3.h"
#include "tier0/basetypes.h"
#include "tier2/tier2.h"
#include "entity2/entityindex.h"

class CSoundEvent;
class CSosOperatorStack;
class CUtlString;
class CUtlStringToken;
class CVSoundStackScriptTypeManager;
class CVSoundEventScriptTypeManager;
class KeyValues;
class KeyValues3;
struct ResourceBindingBase_t;
// Defined in soundsystem/isoundsystem.h
struct SoundGuid_t;
enum sound_voice_layer_t : uint8;

typedef uint32 HSOSLIBSTACKHASH;
typedef uint32 HSOSOPFIELDHASH;
typedef int16 SOSRANDSEED;
// Symbol of a string in the string table of the sound op system, the value of SOFTYPE_STRING_HANDLE fields
typedef short SosStringHandle_t;
// Memory of field values
typedef uint8 SosFieldValueMem_t;

// Bits of the flags of an operator stack (ISoundOpSystem::GetStackFlags), set in tools mode by its operators
// A start voice operator of the stack starts a vsnd
#define SOS_STACK_FLAG_STARTS_VSND ( 1 << 0 )
// A start sound event operator of the stack starts sound events
#define SOS_STACK_FLAG_STARTS_SOUNDEVENTS ( 1 << 1 )

enum SOFieldDataType_t : int8
{
	SOFTYPE_INVALID = -1,
	SOFTYPE_NONE = 0,
	SOFTYPE_BOOL,
	SOFTYPE_INT,
	SOFTYPE_UINT32,
	SOFTYPE_UINT64,
	SOFTYPE_VSND,
	SOFTYPE_TOKEN,
	SOFTYPE_ENUM,
	SOFTYPE_FLOAT,
	SOFTYPE_FLOAT_2,
	SOFTYPE_FLOAT_3,
	SOFTYPE_FLOAT_4,
	SOFTYPE_FLOAT_6,
	SOFTYPE_FLOAT_8,
	SOFTYPE_FLOAT_12,
	SOFTYPE_FLOAT_MAX_SPEAKERS,
	SOFTYPE_STRING_HANDLE,
	SOFTYPE_FLOAT_16,
	SOFTYPE_DSPPRESET,
	SOFTYPE_SNDEVT,
	SOFTYPE_MIXGROUP,
	SOFTYPE_PUBLIC_OPVAR,
};

// How the elements of a field data type are stored (GetAtomicDataType)
enum SOAtomicDataType_t : int32
{
	SOATYPE_INVALID = 0,
	// Float elements, also the bool, enum and float types
	SOATYPE_NUMERIC = 1,
	// 16-bit symbol of the string table of the operator system: string, dsppreset, sndevt, mixgroup and public_opvar
	SOATYPE_SYMBOL = 2,
	SOATYPE_INTEGER = 3,
	SOATYPE_UINT32 = 4,
	SOATYPE_UINT64 = 5,
	// Symbol of the vsnd name, see GetVSndSymbol
	SOATYPE_VSND = 6,
	SOATYPE_STRING_TOKEN = 7,
};

// Values of the "array_selection_type" enum field of the opvar array index operators
enum SosArraySelectionType_t
{
	// SelectIndex picks nothing, the operator keeps its input_index
	SOS_ARRAY_SELECTION_INDEX = 0,
	SOS_ARRAY_SELECTION_FORWARD,
	SOS_ARRAY_SELECTION_BACKWARD,
	SOS_ARRAY_SELECTION_RANDOM,
	// Random among the indices whose bit in the selection state is clear
	SOS_ARRAY_SELECTION_RANDOM_EXCLUSIVE,
	SOS_ARRAY_SELECTION_RANDOM_WEIGHTED,
	SOS_ARRAY_SELECTION_MAX_VALUE,
};

struct CSosFieldData
{
	SOAtomicDataType_t	m_nAtomicDataType;
	SOFieldDataType_t	m_fieldDataType;
	uint32				m_nFieldDataSize;
	uint32				m_nAllocSize;

	union
	{
		bool	m_bValue;
		int32	m_nValue;
		uint32	m_uValue;
		uint64	m_ulValue;
		float	m_flValue;
		void	*m_pData;
	};
};

struct SndOpEventGuid_t {
    SndOpEventGuid_t() :
        m_hStackHash(-1)
    {
    }


	SoundEventGuid_t m_nGuid;
	HSOSLIBSTACKHASH m_hStackHash;
};

#pragma pack( push, 1 )
struct StartSoundEventInfo_t
{
    StartSoundEventInfo_t() :
        m_nFlags(0),
		m_nRecipients(0ull)
    {
    }

	SndOpEventGuid_t m_nSndOpEventGuid;
	uint32 m_nFlags;
	uint64 m_nRecipients;
};
#pragma pack( pop )

struct SosStartSoundEventValue_t
{
	uint64	m_nData;
	int32	m_nType;
};

// Arguments of the StartSoundEvent overloads that take them as one struct
struct SosStartSoundEventParams_t
{
	// Copied into the 64 characters of the queued start
	const char *m_pszSourceClassName;
	// Packed field data of m_nFieldDataSize bytes
	const void *m_pFieldData;
	uint32 m_nSoundEventHash;
	CEntityIndex m_SourceEntity;
	// Zero for a new guid
	SoundEventGuid_t m_nGuid;
	int m_nFieldDataSize;
	// -1 for the next random seed of the operator system
	SOSRANDSEED m_nSeed;
	sound_voice_layer_t m_nLayer;
	// The voices of the sound event are mixed at the volume of the tool layer
	bool m_bIsToolVoice;
	// Only the server traces soundscapes
	bool m_bTraceSoundscapes;
};

enum SosMessageQueueInfoType_t
{
	SOSMSGQ_START = 0,
	SOSMSGQ_STOP,
	SOSMSGQ_STOP_HASH,
	SOSMSGQ_SOUND_FIELDS,
	SOSMSGQ_LIBRARY_FIELDS,
};

// Message of the queues of the operator system
class CSosEventInfo_t
{
public:
	virtual ~CSosEventInfo_t() {}

	SosMessageQueueInfoType_t m_msgType;
};

class CSosStopSoundEventInfo_t : public CSosEventInfo_t
{
public:
	SoundEventGuid_t m_nGuid;
};

template < class T >
class CSosMessageQueue
{
public:
	struct Entry_t
	{
		T key;
		CSosEventInfo_t *pMessage;
	};

	CUtlVector< Entry_t > m_messages;
};

// Receives the messages the operator system processes, see ISoundOpSystem::AddSoundEventListener
abstract_class ISoundEventListener
{
public:
	// Messages that start sound events
	virtual void OnStartSoundEventMessages( const CSosMessageQueue< SoundEventGuid_t > &messages ) = 0;
	// Sound events stopped by StopAllSoundEvents
	virtual void OnSoundEventsStopped( const CUtlVector< SoundEventGuid_t > &guids ) = 0;
	// Messages that set fields of sound events
	virtual void OnSoundEventFieldsMessages( const CSosMessageQueue< SoundEventGuid_t > &messages ) = 0;
	// Messages that set fields of library stacks
	virtual void OnLibraryStackFieldsMessages( const CSosMessageQueue< HSOSLIBSTACKHASH > &messages ) = 0;
	// Sounds the mixer started since the last update, a CUtlVector of 12-byte entries that hold the guid of the sound event,
	// the guid of the sound and a value of its sfx
	virtual void OnSoundsStarted( const void *pList ) = 0;
	// Called for every sound event the operator system starts
	virtual void OnSoundEventStarted( SoundEventGuid_t guid, SoundEventGuid_t parentGuid, SoundEventGuid_t rootGuid ) = 0;
};

class ISoundEventManager
{
public:
	virtual uint32 GetSoundEventHash( const char *pSoundEventName ) const = 0;
	virtual bool IsValidSoundEventHash( uint32 nSoundEvent ) const = 0;
	virtual const char *GetSoundEventName( uint32 nSoundEvent ) const = 0;
	virtual bool HasSoundEvent( const char *pSoundEventName ) const = 0;
	// bWarn logs a warning when no sound event has the hash
	virtual CSoundEvent *GetSoundEvent( uint32 nSoundEvent, bool bWarn ) const = 0;
	virtual CSoundEvent *GetSoundEvent( const char *pSoundEventName ) const = 0;
	// Binding the sound event was added with
	virtual const ResourceBindingBase_t *GetSoundEventResourceBinding( const char *pSoundEventName ) = 0;
	// Looks up the resource of the sound event binding in the asset system; true when the asset exists and the mod index of
	// its files is 0 (IAsset slot 66)
	virtual bool IsSoundEventAssetInFirstMod( const char *pSoundEventName ) = 0;
	virtual void ListSoundEvents( CUtlVector< const char * > &list ) const = 0;
	virtual void ListSoundEvents( CUtlVector< CUtlString > &list ) const = 0;
	// Sound events added with pBinding
	virtual void ListSoundEventsForResource( CUtlVector< const char * > &list, const ResourceBindingBase_t *pBinding ) = 0;
	// Case-insensitive substring match over the sound event names
	virtual void FindSoundEvents( const char *pSubString, CUtlVector< CUtlString > &list ) = 0;
	// Writes the definitions (base, type, fields, groups, custom stack, preload vsnds) of the sound events into the KeyValues3
	// table of pTree, keyed by the name with the prefix prepended
	virtual void WriteSoundEventsKV3( const CUtlString &sPrefix, void *pTree, const CUtlVector< const char * > &soundEventNames ) = 0;
	virtual void DereferenceSoundEvent( CSoundEvent *pSoundEvent ) = 0;
	virtual void RemoveSoundEvent( const char *pSoundEventName ) = 0;
	virtual void RemoveAllSoundEvents() = 0;
	virtual uint32 GetSoundEventUpdateStackHash( uint32 nSoundEvent ) const = 0;
	virtual uint32 GetSoundEventUpdateStackHash( const char *pSoundEventName ) const = 0;
	virtual void GetSoundEventUpdateStackName( uint32 nSoundEvent, CUtlString &sName ) = 0;
	// SOS_STACK_FLAG_* bits of the update stack of the sound event, as returned by ISoundOpSystem::GetStackFlags
	virtual uint8 *GetSoundEventUpdateStackFlags( uint32 nSoundEvent ) = 0;
	// Written as "custom_stack" by WriteSoundEventsKV3
	virtual bool SoundEventHasCustomStack( uint32 nSoundEvent ) = 0;
	virtual uint32 GetSoundEventDefinitionBaseHash( uint32 nSoundEvent, int nBase ) = 0;
	virtual uint32 GetSoundEventDefinitionBaseHash( const char *pSoundEventName, int nBase ) = 0;
	virtual void GetSoundEventDefinitionBaseName( uint32 nSoundEvent, CUtlString &sName, int nBase ) = 0;
	virtual void GetSoundEventDefinitionBaseField( uint32 nSoundEvent, CUtlString &sField, int nBase ) = 0;
	virtual int GetSoundEventDefinitionBaseCount( uint32 nSoundEvent ) = 0;
	virtual int GetSoundEventDefinitionBaseCount( const char *pSoundEventName ) = 0;
	virtual bool SoundEventHasPreloadVsnd( const char *pSoundEventName ) = 0;
	virtual uint8 *GetSoundEventUpdateGroups( uint32 nSoundEvent ) = 0;
	virtual uint8 *GetSoundEventUpdateGroups( const char *pSoundEventName ) = 0;
	// Returns -1 for an unknown sound event
	virtual float GetSoundDuration( const char *pSoundEventName ) = 0;
	virtual float GetSoundDuration( uint32 nSoundEvent ) = 0;
	virtual const char *GetVSndNameForSoundEvent( const char *pSoundEventName, bool bFromSymbolTable ) = 0;
	virtual void GetVSndNameListForSoundEvent( const char *pSoundEventName, CUtlVector< const char * > &list, bool bFromSymbolTable ) = 0;
	virtual void PreloadSoundEvent( uint32 nSoundEvent, int16 nPriority ) = 0;
	virtual void PreloadSoundEvent( const char *pSoundEventName, int16 nPriority ) = 0;
	// bUpdate replaces an existing sound event; without bNoDefer a sound event whose base or stack is missing is deferred;
	// bUnused is read by neither build
	virtual bool AddSoundEvent( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding, bool bUpdate, bool bNoDefer, bool bUnused ) = 0;
	virtual void AddSoundEvents( KeyValues3 *pKV, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void RemoveTimedOutDeferredSoundEvent() = 0;
	// Adds pSoundEventName as a copy of pSourceSoundEventName
	virtual bool CopySoundEvent( const char *pSoundEventName, const char *pSourceSoundEventName, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void AddNewSoundEvent( const char *pSoundEventName, const ResourceBindingBase_t *pBinding ) = 0;
	// Sets the update stack of the sound event from the stack name
	virtual bool SetSoundEventUpdateStack( const char *pSoundEventName, const char *pStackName ) = 0;
	// Adds the field to the definition of the sound event
	virtual bool AddSoundEventDefinitionField( const char *pSoundEventName, const char *pFieldName, const CSosFieldData *pData ) = 0;
	// Sets a string in the KeyValues3 table of the sound event
	virtual bool SetSoundEventKeyValue( const char *pSoundEventName, const char *pKey, const char *pValue ) = 0;
	virtual bool HasSoundEventField( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, int nIndex ) = 0;
	virtual bool HasSoundEventField( const char *pSoundEventName, const char *pFieldName, int nIndex ) = 0;
	virtual bool SetSoundEventDefinitionField( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, const CSosFieldData *pData, int nIndex ) = 0;
	// Selects the element of an array field
	virtual bool SetSoundEventDefinitionFieldIndex( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, int nIndex ) = 0;
	// Element count of the field, or of the array the definition sets for an array field; pbArray tells whether it is an array field
	virtual int GetSoundEventDefinitionFieldCount( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, bool *pbArray, bool bIgnoreBases ) = 0;
	// Element count of the field value GetSoundEventDefinitionField finds
	virtual int GetSoundEventDefinitionFieldElementCount( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, bool bIgnoreBases ) = 0;
	// Fields the sound event does not set are looked up in its bases and then in its update stack, unless bIgnoreBases is set
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, SosFieldValueMem_t **ppData, int nIndex, bool bIgnoreBases ) = 0;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, CUtlString &sValue, int nIndex, bool bIgnoreBases ) = 0;
	// Writes the value to pKV
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, KeyValues3 *pKV, int nIndex, bool bIgnoreBases ) = 0;
	// BeginSoundEventOverrides and EndSoundEventOverrides set and clear the flag that makes AddSoundEvent keep the sound events it
	// replaces, RestoreOverriddenSoundEvents puts the kept sound events back; the addon manager of the client calls them when an addon
	// is activated and deactivated
	virtual void BeginSoundEventOverrides() = 0;
	virtual void EndSoundEventOverrides() = 0;
	virtual void RestoreOverriddenSoundEvents() = 0;
	virtual void SoundEventKVToKV3( KeyValues *pKV, KeyValues3 *pKV3 ) = 0;
	virtual void CompareSoundEvents( const char *pSoundEventA, const char *pSoundEventB ) = 0;
	// Value of the _builtins._voicegraph field
	virtual bool GetSoundEventVoiceGraph( uint32 nSoundEvent, CUtlString &sValue ) = 0;
	virtual void ListDeferredSoundEvents() = 0;
	virtual void ClearDeferredSoundEvents() = 0;
	// Reads a string from the KeyValues3 table of the sound event
	virtual bool GetSoundEventKeyValue( const char *pSoundEventName, const char *pKey, CUtlString &sValue ) = 0;
	// Names of the vsnds of the sound event and the longest duration; both outputs are optional
	virtual bool GetSoundEventVSnds( const char *pSoundEventName, CUtlVector< CUtlString > *pVSndNames, float *pflMaxDuration ) = 0;
};

class ISoundOpSystem : public IAppSystem, public ISoundEventManager
{
public:
	// nSource is the entity the sound event is started for, invalid when it has none
	virtual SndOpEventGuid_t StartSoundEvent( const char *pSoundEventName, CEntityIndex nSource, sound_voice_layer_t nLayer ) = 0;
	// The overload with the name sets m_nSoundEventHash of pParams
	virtual SndOpEventGuid_t StartSoundEvent( const char *pSoundEventName, SosStartSoundEventParams_t *pParams ) = 0;
	virtual SndOpEventGuid_t StartSoundEvent( SosStartSoundEventParams_t *pParams ) = 0;
	virtual SndOpEventGuid_t StartStack( const char *pStackName, CEntityIndex nSource, sound_voice_layer_t nLayer ) = 0;
	// Queues the stop of the sound event as a CSosStopSoundEventInfo_t message
	virtual void QueueStopSoundEvent( SoundEventGuid_t guid ) = 0;
	virtual bool StopSoundEvent( SoundEventGuid_t guid ) = 0;
	// An invalid nSource stops every instance of the sound event
	virtual void StopSoundEvent( uint32 nSoundEvent, CEntityIndex nSource ) = 0;
	virtual bool StopSoundEvent( const char *pSoundEventName, CEntityIndex nSource ) = 0;
	// Pauses the voices of the sound event before stopping it, fails when the sound system is disabled
	virtual bool StopSoundEventImmediately( SoundEventGuid_t guid ) = 0;
	// Stops the sound events started on nLayer, VOICE_LAYER_ALL stops every sound event
	virtual void StopSoundEventsOnLayer( sound_voice_layer_t nLayer ) = 0;
	// bStopSounds also stops every sound of the sound system
	virtual void StopAllSoundEvents( bool bStopSounds ) = 0;
	virtual bool PauseSoundEvent( SoundEventGuid_t guid ) = 0;
	virtual void PauseSoundEvents( uint32 nSoundEvent, CEntityIndex nSource ) = 0;
	virtual bool PauseSoundEvents( const char *pSoundEventName, CEntityIndex nSource ) = 0;
	// Pauses by voice layer, VOICE_LAYER_ALL with VOICE_LAYER_NONE pauses every sound event
	virtual void PauseSoundEventsOnLayer( sound_voice_layer_t nLayer, sound_voice_layer_t nLayer2 ) = 0;
	virtual bool UnPauseSoundEvent( SoundEventGuid_t guid ) = 0;
	virtual void UnPauseSoundEvents( uint32 nSoundEvent, CEntityIndex nSource ) = 0;
	virtual bool UnPauseSoundEvents( const char *pSoundEventName, CEntityIndex nSource ) = 0;
	// Decrements the pause count of every paused sound event
	virtual void UnPauseAllSoundEvents() = 0;
	virtual CSosOperatorStack *GetStack( HSOSLIBSTACKHASH hStack ) = 0;
	// SOS_STACK_FLAG_* bits of the stack
	virtual uint8 *GetStackFlags( HSOSLIBSTACKHASH hStack ) = 0;
	virtual bool SetSoundEventParam( SoundEventGuid_t guid, HSOSOPFIELDHASH nFieldToken, const CSosFieldData *pData, int16 nIndex ) = 0;
	// pData holds nSize bytes of packed field data, queued for the sound event or the library stack
	virtual bool SetSoundEventFields( SoundEventGuid_t guid, const void *pData, int nSize ) = 0;
	// Public field names starting with an underscore are builtin fields
	virtual bool SetSoundEventPublicParam( SoundEventGuid_t guid, const char *pFieldName, const CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool SetLibraryStackField( HSOSLIBSTACKHASH hStack, HSOSOPFIELDHASH nFieldToken, const CSosFieldData *pData, int16 nIndex ) = 0;
	// Declared between the SetLibraryStackField overloads
	virtual bool SetLibraryStackFields( HSOSLIBSTACKHASH hStack, const void *pData, int nSize ) = 0;
	virtual bool SetLibraryStackField( const char *pStackName, const char *pOperatorName, const char *pFieldName, const CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool SetLibraryStackPublicField( const char *pStackName, const char *pFieldName, const CSosFieldData *pData, int16 nIndex ) = 0;
	// pStackField holds the stack hash followed by the field token
	virtual bool GetLibraryStackField( const uint32 *pStackField, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool GetLibraryStackField( HSOSLIBSTACKHASH hStack, HSOSOPFIELDHASH nFieldToken, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool GetLibraryStackField( const char *pStackName, const char *pOperatorName, const char *pFieldName, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool GetLibraryStackPublicField( const char *pStackName, const char *pFieldName, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual SOFieldDataType_t GetLibraryStackFieldType( const char *pStackName, const char *pOperatorName, const char *pFieldName ) = 0;
	// Field token from the operator and field names
	virtual CUtlStringToken GetFieldToken( const char *pOperatorName, const char *pFieldName ) = 0;
	// Field token of a public field, or of a builtin one when the name starts with an underscore
	virtual CUtlStringToken GetPublicFieldToken( const char *pFieldName ) = 0;
	virtual HSOSLIBSTACKHASH GetStackHash( const char *pStackName ) = 0;
	// 0 for an unknown sound event, 1 while its start is queued, otherwise 2 plus the state of the active sound event
	virtual int GetSoundEventState( SoundEventGuid_t guid ) = 0;
	virtual bool IsSoundEventPlaying( SoundEventGuid_t guid ) = 0;
	virtual bool IsSoundEventPaused( SoundEventGuid_t guid ) = 0;
	// Time the voice of the sound event was started late by (the start voice and track queue operators set it), -1 when it is
	// not active; the movie recorder records the negated value as the start time
	virtual float GetSoundEventStartOffset( SoundEventGuid_t guid ) = 0;
	// Values of the voice of the sound event that the vmix_voice_time operator outputs as output_voice_elapsed_time and
	// output_voice_seek_time, -1 when it is not active
	virtual float GetSoundEventElapsedTime( SoundEventGuid_t guid ) = 0;
	virtual float GetSoundEventSeekTime( SoundEventGuid_t guid ) = 0;
	// Public field names of the update stack of the sound event
	virtual void GetSoundEventPublicFieldNames( const char *pSoundEventName, CUtlVector< const char * > &list ) = 0;
	// Whether pData can be converted to the type of the field of the stack, pnCount receives the element count of the field;
	// the server checks the fields of its sound event and library stack messages with it
	virtual bool ValidateStackFieldData( HSOSLIBSTACKHASH hStack, HSOSOPFIELDHASH nFieldToken, const CSosFieldData *pData, int *pnCount ) = 0;
	virtual bool ValidateStackFieldData( HSOSLIBSTACKHASH hStack, const char *pOperatorName, const char *pFieldName, const CSosFieldData *pData, int *pnCount ) = 0;
	virtual SOAtomicDataType_t GetAtomicDataType( SOFieldDataType_t nType ) = 0;
	virtual int GetFieldDataTypeSize( SOFieldDataType_t nType ) = 0;
	// Element count of the type, GetFieldDataTypeSize is the element count times the element size
	virtual int GetFieldDataTypeCount( SOFieldDataType_t nType ) = 0;
	// Allocates field data holding the value of pKV
	virtual CSosFieldData *CreateFieldDataFromKV3( KeyValues3 *pKV ) = 0;
	// Sets the field of the sound event definition from its text form
	virtual bool SetSoundEventDefinitionFieldFromString( uint32 nSoundEvent, const char *pFieldName, const char *pValue ) = 0;
	virtual bool SetSoundEventDefinitionFieldFromString( const char *pSoundEventName, const char *pFieldName, const char *pValue ) = 0;
	// Allocates field data of the type, pData may be null for a zero value
	virtual CSosFieldData *CreateFieldData( SOFieldDataType_t nType, const void *pData ) = 0;
	// String table of the operator system
	// AddString adds the string, FindString only looks it up and GetString returns the string of the handle
	virtual SosStringHandle_t AddString( const char *pString ) = 0;
	virtual SosStringHandle_t FindString( const char *pString ) = 0;
	virtual const char *GetString( SosStringHandle_t hString ) = 0;
	// Whether a stack has the hash
	virtual bool IsValidStack( HSOSLIBSTACKHASH hStack ) = 0;
	// Next random seed for a started sound event
	virtual SOSRANDSEED GetNextRandomSeed() = 0;
	// Name of the active sound event
	virtual const char *GetActiveSoundEventName( SoundEventGuid_t guid ) = 0;
	// Picks an index up to nMax; for FORWARD and BACKWARD pState holds the last index, for RANDOM_EXCLUSIVE a bit per index,
	// RANDOM_WEIGHTED without weights selects like RANDOM_EXCLUSIVE
	virtual int SelectIndex( CSosOperatorStack *pStack, uint8 *pState, SosArraySelectionType_t nMode, int nMax, CUtlVector< float > *pWeights ) = 0;
	// Name of the update group
	virtual const char *GetUpdateGroupName( int8 nGroup ) = 0;
	// Guids of the active sound events
	virtual void GetActiveSoundEvents( CUtlVector< SoundEventGuid_t > &list ) = 0;
	// Detailed tool debugging flag, SetDetailedToolDebugging only sets it while snd_sos_tools_detailed_debugging is on
	virtual bool IsDetailedToolDebuggingEnabled() = 0;
	virtual void SetDetailedToolDebugging( bool bEnable ) = 0;
	// History entry kept for the sound event: its stack hash and the field data recorded while it played
	virtual void *GetSoundEventHistory( SoundEventGuid_t guid ) = 0;
	// Sound event whose history is kept after it stops; the history of the previous one is removed unless it still plays
	virtual void KeepSoundEventHistory( SoundEventGuid_t guid ) = 0;
	// Symbol of the vsnd name in the vsnd resource type manager, used as the value of SOFTYPE_VSND fields
	virtual uint32 GetVSndSymbol( const char *pVsndName ) = 0;
	// Convert a stack between KeyValues and its KeyValues3 form with the "operators" array
	virtual bool StackKVToKV3( KeyValues *pKV, KeyValues3 *pKV3 ) = 0;
	virtual bool StackKV3ToKV( KeyValues3 *pKV3, KeyValues *pKV ) = 0;
	// Writes the operator fields of the stack to pKV
	virtual void WriteStackFieldsKV3( HSOSLIBSTACKHASH hStack, KeyValues3 *pKV ) = 0;
	// Reads the operator fields of the stack from pKV
	virtual void ReadStackFieldsKV3( HSOSLIBSTACKHASH hStack, KeyValues3 *pKV ) = 0;
	// The movie recorder adds itself while it records
	virtual void AddSoundEventListener( ISoundEventListener *pListener ) = 0;
	virtual void RemoveSoundEventListener( ISoundEventListener *pListener ) = 0;
	// String of the field of the sound event or of the library stack
	virtual const char *GetFieldValueString( SoundEventGuid_t guid, HSOSOPFIELDHASH nFieldToken ) = 0;
	virtual const char *GetFieldValueString( HSOSLIBSTACKHASH hStack, HSOSOPFIELDHASH nFieldToken ) = 0;
	// Name of the library stack
	virtual const char *GetStackName( HSOSLIBSTACKHASH hStack ) = 0;
	// Source entity the sound event was started with
	virtual CEntityIndex GetSoundEventSourceEntity( SoundEventGuid_t guid ) = 0;
	// Field values of the active sound event, the counterparts of SetSoundEventParam and SetSoundEventPublicParam
	virtual bool GetSoundEventParam( SoundEventGuid_t guid, HSOSOPFIELDHASH nFieldToken, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool GetSoundEventPublicParam( SoundEventGuid_t guid, const char *pFieldName, CSosFieldData *pData, int16 nIndex ) = 0;
	// Random seed of the sound event
	virtual SOSRANDSEED GetSoundEventSeed( SoundEventGuid_t guid ) = 0;
	// Voice layer the sound event was started on, VOICE_LAYER_NONE when it is not active
	virtual sound_voice_layer_t GetSoundEventLayer( SoundEventGuid_t guid ) = 0;
	// Guid of the sound event that started this one, and of the first sound event of that chain; zero for none
	virtual SoundEventGuid_t GetSoundEventParentGuid( SoundEventGuid_t guid ) = 0;
	virtual SoundEventGuid_t GetSoundEventRootGuid( SoundEventGuid_t guid ) = 0;
	// Guid of the sound system voice the sound event plays, zero for none
	virtual SoundGuid_t GetSoundEventVoiceGuid( SoundEventGuid_t guid ) = 0;
	// Count and string of an element of a list of symbols of the sound event manager string table
	virtual int GetSymbolListCount( const CUtlLeanVector< uint32 > *const *ppList ) = 0;
	virtual const char *GetSymbolListString( const CUtlLeanVector< uint32 > *const *ppList, int nIndex ) = 0;
	// Names of the fields of the sound event that have the type
	virtual void GetSoundEventFieldNamesOfType( const char *pSoundEventName, CUtlVector< const char * > &list, SOFieldDataType_t nType ) = 0;
	// Non-zero makes StartSoundEvent and StartStack fail, the engine sets it while a demo skips ahead
	virtual void SetSuppressStartSoundEvent( int nSuppress ) = 0;
	// Locks and unlocks the operator system
	virtual void LockOperatorSystem() = 0;
	virtual void UnlockOperatorSystem() = 0;
	// Loads the stacks of the default manifest with LoadManifestStacksForParsing and checks each of them. pErrors may be null;
	// otherwise it is an object (type not recovered) whose IErrorListener base 8 bytes in receives each error as an
	// ERROR_LISTENER_SEVERITY_ERROR message
	virtual void ParseManifestStacksForErrors( KeyValues3 *pStacks, void *pErrors ) = 0;
};

class CSoundEventManager : public ISoundEventManager
{
public:
	// Sound event whose base or stack was missing when it was added, AddSoundEvent retries it later
	struct DeferredSoundEvent_t
	{
		char m_szName[ 128 ];
		KeyValues3 *m_pKV;
		const ResourceBindingBase_t *m_pBinding;
		bool m_bDone;
		// Time the sound event was deferred at, RemoveTimedOutDeferredSoundEvent drops it after a while
		double m_flTime;
	};

	virtual uint32 GetSoundEventHash( const char *pSoundEventName ) const override;
	virtual bool IsValidSoundEventHash( uint32 nSoundEvent ) const override;
	virtual const char *GetSoundEventName( uint32 nSoundEvent ) const override;
	virtual bool HasSoundEvent( const char *pSoundEventName ) const override;
	virtual CSoundEvent *GetSoundEvent( uint32 nSoundEvent, bool bWarn ) const override;
	virtual CSoundEvent *GetSoundEvent( const char *pSoundEventName ) const override;
	virtual const ResourceBindingBase_t *GetSoundEventResourceBinding( const char *pSoundEventName ) override;
	virtual bool IsSoundEventAssetInFirstMod( const char *pSoundEventName ) override;
	virtual void ListSoundEvents( CUtlVector< const char * > &list ) const override;
	virtual void ListSoundEvents( CUtlVector< CUtlString > &list ) const override;
	virtual void ListSoundEventsForResource( CUtlVector< const char * > &list, const ResourceBindingBase_t *pBinding ) override;
	virtual void FindSoundEvents( const char *pSubString, CUtlVector< CUtlString > &list ) override;
	virtual void WriteSoundEventsKV3( const CUtlString &sPrefix, void *pTree, const CUtlVector< const char * > &soundEventNames ) override;
	virtual void DereferenceSoundEvent( CSoundEvent *pSoundEvent ) override;
	virtual void RemoveSoundEvent( const char *pSoundEventName ) override;
	virtual void RemoveAllSoundEvents() override;
	virtual uint32 GetSoundEventUpdateStackHash( uint32 nSoundEvent ) const override;
	virtual uint32 GetSoundEventUpdateStackHash( const char *pSoundEventName ) const override;
	virtual void GetSoundEventUpdateStackName( uint32 nSoundEvent, CUtlString &sName ) override;
	virtual uint8 *GetSoundEventUpdateStackFlags( uint32 nSoundEvent ) override;
	virtual bool SoundEventHasCustomStack( uint32 nSoundEvent ) override;
	virtual uint32 GetSoundEventDefinitionBaseHash( uint32 nSoundEvent, int nBase ) override;
	virtual uint32 GetSoundEventDefinitionBaseHash( const char *pSoundEventName, int nBase ) override;
	virtual void GetSoundEventDefinitionBaseName( uint32 nSoundEvent, CUtlString &sName, int nBase ) override;
	virtual void GetSoundEventDefinitionBaseField( uint32 nSoundEvent, CUtlString &sField, int nBase ) override;
	virtual int GetSoundEventDefinitionBaseCount( uint32 nSoundEvent ) override;
	virtual int GetSoundEventDefinitionBaseCount( const char *pSoundEventName ) override;
	virtual bool SoundEventHasPreloadVsnd( const char *pSoundEventName ) override;
	virtual uint8 *GetSoundEventUpdateGroups( uint32 nSoundEvent ) override;
	virtual uint8 *GetSoundEventUpdateGroups( const char *pSoundEventName ) override;
	virtual float GetSoundDuration( const char *pSoundEventName ) override;
	virtual float GetSoundDuration( uint32 nSoundEvent ) override;
	virtual const char *GetVSndNameForSoundEvent( const char *pSoundEventName, bool bFromSymbolTable ) override;
	virtual void GetVSndNameListForSoundEvent( const char *pSoundEventName, CUtlVector< const char * > &list, bool bFromSymbolTable ) override;
	virtual void PreloadSoundEvent( uint32 nSoundEvent, int16 nPriority ) override;
	virtual void PreloadSoundEvent( const char *pSoundEventName, int16 nPriority ) override;
	virtual bool AddSoundEvent( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding, bool bUpdate, bool bNoDefer, bool bUnused ) override;
	virtual void AddSoundEvents( KeyValues3 *pKV, const ResourceBindingBase_t *pBinding ) override;
	virtual void RemoveTimedOutDeferredSoundEvent() override;
	virtual bool CopySoundEvent( const char *pSoundEventName, const char *pSourceSoundEventName, const ResourceBindingBase_t *pBinding ) override;
	virtual void AddNewSoundEvent( const char *pSoundEventName, const ResourceBindingBase_t *pBinding ) override;
	virtual bool SetSoundEventUpdateStack( const char *pSoundEventName, const char *pStackName ) override;
	virtual bool AddSoundEventDefinitionField( const char *pSoundEventName, const char *pFieldName, const CSosFieldData *pData ) override;
	virtual bool SetSoundEventKeyValue( const char *pSoundEventName, const char *pKey, const char *pValue ) override;
	virtual bool HasSoundEventField( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, int nIndex ) override;
	virtual bool HasSoundEventField( const char *pSoundEventName, const char *pFieldName, int nIndex ) override;
	virtual bool SetSoundEventDefinitionField( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, const CSosFieldData *pData, int nIndex ) override;
	virtual bool SetSoundEventDefinitionFieldIndex( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, int nIndex ) override;
	virtual int GetSoundEventDefinitionFieldCount( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, bool *pbArray, bool bIgnoreBases ) override;
	virtual int GetSoundEventDefinitionFieldElementCount( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, bool bIgnoreBases ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, SosFieldValueMem_t **ppData, int nIndex, bool bIgnoreBases ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, CUtlString &sValue, int nIndex, bool bIgnoreBases ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, KeyValues3 *pKV, int nIndex, bool bIgnoreBases ) override;
	virtual void BeginSoundEventOverrides() override;
	virtual void EndSoundEventOverrides() override;
	virtual void RestoreOverriddenSoundEvents() override;
	virtual void SoundEventKVToKV3( KeyValues *pKV, KeyValues3 *pKV3 ) override;
	virtual void CompareSoundEvents( const char *pSoundEventA, const char *pSoundEventB ) override;
	virtual bool GetSoundEventVoiceGraph( uint32 nSoundEvent, CUtlString &sValue ) override;
	virtual void ListDeferredSoundEvents() override;
	virtual void ClearDeferredSoundEvents() override;
	virtual bool GetSoundEventKeyValue( const char *pSoundEventName, const char *pKey, CUtlString &sValue ) override;
	virtual bool GetSoundEventVSnds( const char *pSoundEventName, CUtlVector< CUtlString > *pVSndNames, float *pflMaxDuration ) override;

	// Sound events by hash
	CUtlHashtable< uint32, CSoundEvent *, MurmurHash3IntFunctor > m_SoundEvents;
	// Sound events taken out of m_SoundEvents that are still referenced, deleted by DereferenceSoundEvent
	CUtlVector< CSoundEvent * > m_ReleasedSoundEvents;
	// Set by BeginSoundEventOverrides and cleared by EndSoundEventOverrides
	bool m_bKeepOverriddenSoundEvents;
	// Sound events that AddSoundEvent replaced while m_bKeepOverriddenSoundEvents was set, RestoreOverriddenSoundEvents puts them
	// back into m_SoundEvents
	CUtlHashtable< uint32, CSoundEvent *, MurmurHash3IntFunctor > m_OverriddenSoundEvents;
	CUtlVector< DeferredSoundEvent_t > m_DeferredSoundEvents;
	// Time of the last check for timed out deferred sound events
	double m_flLastDeferredSoundEventCheck;
};

class CSoundOpSystem : public CTier2AppSystem< ISoundOpSystem >
{
public:
	virtual uint32 GetSoundEventHash( const char *pSoundEventName ) const override;
	virtual bool IsValidSoundEventHash( uint32 nSoundEvent ) const override;
	virtual const char *GetSoundEventName( uint32 nSoundEvent ) const override;
	virtual bool HasSoundEvent( const char *pSoundEventName ) const override;
	virtual CSoundEvent *GetSoundEvent( uint32 nSoundEvent, bool bWarn ) const override;
	virtual CSoundEvent *GetSoundEvent( const char *pSoundEventName ) const override;
	virtual const ResourceBindingBase_t *GetSoundEventResourceBinding( const char *pSoundEventName ) override;
	virtual bool IsSoundEventAssetInFirstMod( const char *pSoundEventName ) override;
	virtual bool GetSoundEventVoiceGraph( uint32 nSoundEvent, CUtlString &sValue ) override;
	virtual void ListSoundEvents( CUtlVector< const char * > &list ) const override;
	virtual void ListSoundEvents( CUtlVector< CUtlString > &list ) const override;
	virtual void ListSoundEventsForResource( CUtlVector< const char * > &list, const ResourceBindingBase_t *pBinding ) override;
	virtual void FindSoundEvents( const char *pSubString, CUtlVector< CUtlString > &list ) override;
	virtual void WriteSoundEventsKV3( const CUtlString &sPrefix, void *pTree, const CUtlVector< const char * > &soundEventNames ) override;
	virtual void DereferenceSoundEvent( CSoundEvent *pSoundEvent ) override;
	virtual void RemoveSoundEvent( const char *pSoundEventName ) override;
	virtual void RemoveAllSoundEvents() override;
	virtual uint8 *GetSoundEventUpdateStackFlags( uint32 nSoundEvent ) override;
	virtual uint32 GetSoundEventUpdateStackHash( uint32 nSoundEvent ) const override;
	virtual uint32 GetSoundEventUpdateStackHash( const char *pSoundEventName ) const override;
	virtual void GetSoundEventUpdateStackName( uint32 nSoundEvent, CUtlString &sName ) override;
	virtual bool SoundEventHasCustomStack( uint32 nSoundEvent ) override;
	virtual uint32 GetSoundEventDefinitionBaseHash( uint32 nSoundEvent, int nBase ) override;
	virtual uint32 GetSoundEventDefinitionBaseHash( const char *pSoundEventName, int nBase ) override;
	virtual void GetSoundEventDefinitionBaseName( uint32 nSoundEvent, CUtlString &sName, int nBase ) override;
	virtual void GetSoundEventDefinitionBaseField( uint32 nSoundEvent, CUtlString &sField, int nBase ) override;
	virtual int GetSoundEventDefinitionBaseCount( uint32 nSoundEvent ) override;
	virtual int GetSoundEventDefinitionBaseCount( const char *pSoundEventName ) override;
	virtual bool SoundEventHasPreloadVsnd( const char *pSoundEventName ) override;
	virtual uint8 *GetSoundEventUpdateGroups( uint32 nSoundEvent ) override;
	virtual uint8 *GetSoundEventUpdateGroups( const char *pSoundEventName ) override;
	virtual int GetSoundEventDefinitionFieldCount( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, bool *pbArray, bool bIgnoreBases ) override;
	virtual int GetSoundEventDefinitionFieldElementCount( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, bool bIgnoreBases ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, SosFieldValueMem_t **ppData, int nIndex, bool bIgnoreBases ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, CUtlString &sValue, int nIndex, bool bIgnoreBases ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, KeyValues3 *pKV, int nIndex, bool bIgnoreBases ) override;
	virtual float GetSoundDuration( const char *pSoundEventName ) override;
	virtual float GetSoundDuration( uint32 nSoundEvent ) override;
	virtual const char *GetVSndNameForSoundEvent( const char *pSoundEventName, bool bFromSymbolTable ) override;
	virtual void GetVSndNameListForSoundEvent( const char *pSoundEventName, CUtlVector< const char * > &list, bool bFromSymbolTable ) override;
	virtual void PreloadSoundEvent( uint32 nSoundEvent, int16 nPriority ) override;
	virtual void PreloadSoundEvent( const char *pSoundEventName, int16 nPriority ) override;
	virtual bool AddSoundEvent( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding, bool bUpdate, bool bNoDefer, bool bUnused ) override;
	virtual void AddSoundEvents( KeyValues3 *pKV, const ResourceBindingBase_t *pBinding ) override;
	virtual void RemoveTimedOutDeferredSoundEvent() override;
	virtual bool CopySoundEvent( const char *pSoundEventName, const char *pSourceSoundEventName, const ResourceBindingBase_t *pBinding ) override;
	virtual void AddNewSoundEvent( const char *pSoundEventName, const ResourceBindingBase_t *pBinding ) override;
	virtual bool SetSoundEventUpdateStack( const char *pSoundEventName, const char *pStackName ) override;
	virtual bool AddSoundEventDefinitionField( const char *pSoundEventName, const char *pFieldName, const CSosFieldData *pData ) override;
	virtual bool SetSoundEventKeyValue( const char *pSoundEventName, const char *pKey, const char *pValue ) override;
	virtual bool HasSoundEventField( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, int nIndex ) override;
	virtual bool HasSoundEventField( const char *pSoundEventName, const char *pFieldName, int nIndex ) override;
	virtual bool SetSoundEventDefinitionField( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, const CSosFieldData *pData, int nIndex ) override;
	virtual bool SetSoundEventDefinitionFieldIndex( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, int nIndex ) override;
	virtual void BeginSoundEventOverrides() override;
	virtual void EndSoundEventOverrides() override;
	virtual void RestoreOverriddenSoundEvents() override;
	virtual void SoundEventKVToKV3( KeyValues *pKV, KeyValues3 *pKV3 ) override;
	virtual void CompareSoundEvents( const char *pSoundEventA, const char *pSoundEventB ) override;
	virtual void ListDeferredSoundEvents() override;
	virtual void ClearDeferredSoundEvents() override;
	virtual bool GetSoundEventKeyValue( const char *pSoundEventName, const char *pKey, CUtlString &sValue ) override;
	virtual bool GetSoundEventVSnds( const char *pSoundEventName, CUtlVector< CUtlString > *pVSndNames, float *pflMaxDuration ) override;
	// Uses the default manifest
	virtual void LoadManifestStacksForParsing( KeyValues3 *pStacks, void *pErrors );
	virtual void LoadManifestStacksForParsing( const char *pManifestName, KeyValues3 *pStacks, void *pErrors );

	// Incremented by Init and decremented by Shutdown
	int m_nInitCount;
	CSoundEventManager m_SoundEventManager;
	// Resource type managers of the sound stack scripts and the sound event scripts
	CVSoundStackScriptTypeManager *m_pStackScriptTypeManager;
	CVSoundEventScriptTypeManager *m_pEventScriptTypeManager;
};

#endif // ISOUNDOPSYSTEM_H
