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
#include "tier0/basetypes.h"
#include "tier0/utlstringtoken.h"

class CSoundEvent;
class CSosOperatorStack;
class CUtlString;
class CBufferString;
class KeyValues;
class KeyValues3;
struct ResourceBindingBase_t;

typedef uint32 HSOSLIBSTACKHASH;

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
	SOFTYPE_FLOAT_MAX_SPEAKERS,
	SOFTYPE_STRING_HANDLE,
};

enum SOAtomicDataType_t : int32
{
	SOATYPE_INVALID = 0,
	SOATYPE_NUMERIC = 1,
	SOATYPE_INTEGER = 3,
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


// 4-byte value the sound op system returns by value (a class in the game: MSVC returns it through a hidden pointer)
class SosReturnValue_t
{
public:
	SosReturnValue_t() : m_nValue( 0 ) {}

	int32 m_nValue;
};

// Sound event definitions (CSoundEventManager). Overloads are declared in the Linux order; MSVC reverses
// each overload set. Names from the game's log strings where they exist, otherwise by what the code does.
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
	// Whether the resource of the sound event binding is loaded
	virtual bool HasSoundEventResource( const char *pSoundEventName ) = 0;
	virtual void ListSoundEvents( CUtlVector< const char * > &list ) const = 0;
	virtual void ListSoundEvents( CUtlVector< CUtlString > &list ) const = 0;
	// Sound events added with pBinding
	virtual void ListSoundEventsForResource( CUtlVector< const char * > &list, const ResourceBindingBase_t *pBinding ) = 0;
	// Case-insensitive substring match over the sound event names
	virtual void FindSoundEvents( const char *pSubString, CUtlVector< CUtlString > &list ) = 0;
	// Resolves "type", "base", "event_name", "field_name" paths into field tokens
	virtual void ParseSoundEventFieldPaths( const char **ppPaths, int *pTokens, int *pCount ) = 0;
	virtual void DereferenceSoundEvent( CSoundEvent *pSoundEvent ) = 0;
	virtual void RemoveSoundEvent( const char *pSoundEventName ) = 0;
	virtual void RemoveAllSoundEvents() = 0;
	virtual uint32 GetSoundEventUpdateStackHash( uint32 nSoundEvent ) const = 0;
	virtual uint32 GetSoundEventUpdateStackHash( const char *pSoundEventName ) const = 0;
	virtual void GetSoundEventUpdateStackName( uint32 nSoundEvent, CUtlString &sName ) = 0;
	// ISoundOpSystem::GetStackData of the update stack
	virtual void *GetSoundEventUpdateStackData( uint32 nSoundEvent ) = 0;
	virtual bool IsSoundEventResolved( uint32 nSoundEvent ) = 0;
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
	// Calls ISoundSystem::PreloadSoundEvent
	virtual void PreloadSoundEvent( uint32 nSoundEvent, uint16 nPreloadCount ) = 0;
	virtual void PreloadSoundEvent( const char *pSoundEventName, uint16 nPreloadCount ) = 0;
	virtual bool AddSoundEvent( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding, bool bUpdate ) = 0;
	virtual void AddSoundEvents( KeyValues3 *pKV, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void RemoveTimedOutDeferredSoundEvent() = 0;
	// Adds pSoundEventName as a copy of pSourceSoundEventName
	virtual bool CopySoundEvent( const char *pSoundEventName, const char *pSourceSoundEventName, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void AddNewSoundEvent( const char *pSoundEventName, const ResourceBindingBase_t *pBinding ) = 0;
	// Sets the update stack of the sound event from the stack name
	virtual bool SetSoundEventUpdateStack( const char *pSoundEventName, const char *pStackName ) = 0;
	virtual bool SetSoundEventFieldFromString( const char *pSoundEventName, const char *pFieldName, const char *pValue ) = 0;
	virtual bool SetSoundEventFieldFromValue( const char *pSoundEventName, const char *pFieldName, const void *pValue ) = 0;
	virtual bool HasSoundEventField( CSoundEvent *pSoundEvent, uint32 nFieldToken, int nIndex ) = 0;
	virtual bool HasSoundEventField( const char *pSoundEventName, const char *pFieldName, int nIndex ) = 0;
	virtual void SetSoundEventDefinitionField( uint32 nSoundEvent, uint32 nFieldToken, const void *pData, int nIndex ) = 0;
	// "CFieldSelectionWordAllocator: Total number of array selection bytes on soundevent exceeded"
	virtual bool AllocateSoundEventFieldSelection( CSoundEvent *pSoundEvent, uint32 nFieldToken, int nCount ) = 0;
	virtual bool SetSoundEventDefinitionFieldString( uint32 nSoundEvent, uint32 nFieldToken, const char *pValue, int nIndex ) = 0;
	virtual void *GetSoundEventDefinitionFieldInfo( uint32 nSoundEvent, uint32 nFieldToken, bool bResolveBases ) = 0;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, uint32 nFieldToken, void **ppData, int nIndex, bool bResolveBases ) = 0;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, uint32 nFieldToken, CUtlString &sValue, int nIndex, bool bResolveBases ) = 0;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, uint32 nFieldToken, void *pData, int nIndex, bool bResolveBases ) = 0;
	// Set and clear a flag of the manager around bulk updates
	virtual void BeginSoundEventUpdate() = 0;
	virtual void EndSoundEventUpdate() = 0;
	virtual void RebuildSoundEventFieldSelection() = 0;
	virtual void SoundEventKVToKV3( KeyValues *pKV, KeyValues3 *pKV3 ) = 0;
	virtual void CompareSoundEvents( const char *pSoundEventA, const char *pSoundEventB ) = 0;
	virtual bool GetSoundEventTypeName( uint32 nSoundEvent, CUtlString &sValue ) = 0;
	virtual void ListDeferredSoundEvents() = 0;
	virtual void RemoveAllDeferredSoundEvents() = 0;
	virtual bool AddSoundEventFieldFromString( const char *pSoundEventName, const char *pFieldName, const char *pValue ) = 0;
	virtual bool GetSoundEventFloatField( const char *pSoundEventName, int *pFieldToken, float *pValue ) = 0;
};

// Sound op system (CSoundOpSystem). Slots 0-10 IAppSystem, 11-100 below, then ISoundEventManager at +8.
class ISoundOpSystem : public IAppSystem, public ISoundEventManager
{
public:
	// nSource is -1 when the sound event has no source
	virtual SndOpEventGuid_t StartSoundEvent( const char *pSoundEventName, uint32 nSource, bool bForce ) = 0;
	virtual SndOpEventGuid_t StartSoundEvent( const char *pSoundEventName, void *pParams ) = 0;
	virtual SndOpEventGuid_t StartSoundEvent( void *pParams ) = 0;
	virtual SndOpEventGuid_t StartStack( const char *pStackName, int nSource, bool bForce ) = 0;
	// Queues a CSosStopSoundEventInfo_t
	virtual void QueueStopSoundEvent( SoundEventGuid_t guid ) = 0;
	virtual bool StopSoundEvent( SoundEventGuid_t guid ) = 0;
	// nSource -1 stops every instance of the sound event
	virtual void StopSoundEvent( uint32 nSoundEvent, int nSource ) = 0;
	virtual bool StopSoundEvent( const char *pSoundEventName, int nSource ) = 0;
	virtual bool CanStartSoundEvent( uint32 nSoundEvent ) = 0;
	virtual void UpdateSoundEvents( bool bForce ) = 0;
	// bStopSounds also stops every sound of the sound system
	virtual void StopAllSoundEvents( bool bStopSounds ) = 0;
	virtual bool PauseSoundEvent( SoundEventGuid_t guid ) = 0;
	virtual void PauseSoundEvents( uint32 nSoundEvent, int nSource ) = 0;
	virtual bool PauseSoundEvents( const char *pSoundEventName, int nSource ) = 0;
	virtual void SetSoundEventsPaused( int8 nLayer, int8 nPaused ) = 0;
	virtual bool UnPauseSoundEvent( SoundEventGuid_t guid ) = 0;
	virtual void UnPauseSoundEvents( uint32 nSoundEvent, int nSource ) = 0;
	virtual bool UnPauseSoundEvents( const char *pSoundEventName, int nSource ) = 0;
	virtual void ResumeAllSoundEvents() = 0;
	virtual CSosOperatorStack *GetStack( HSOSLIBSTACKHASH hStack ) = 0;
	virtual void *GetStackData( HSOSLIBSTACKHASH hStack ) = 0;
	virtual bool SetSoundEventParam( SoundEventGuid_t guid, uint32 nFieldToken, const void *pData, int16 nIndex ) = 0;
	// CSoundEventFieldsQueue::SetSoundEventFields: packed fields of a sound event
	virtual bool SetSoundEventFields( SoundEventGuid_t guid, const void *pData, uint32 nSize ) = 0;
	// Field of the "public" operator ("_builtins" for names that start with an underscore)
	virtual bool SetSoundEventPublicParam( SoundEventGuid_t guid, const char *pFieldName, const void *pData, int16 nIndex ) = 0;
	// Not overloads: the name version comes first on both platforms
	virtual bool SetLibraryStackField( const char *pStackName, const char *pOperatorName, const char *pFieldName, const void *pData, int16 nIndex ) = 0;
	virtual bool SetLibraryStackFieldByToken( HSOSLIBSTACKHASH hStack, uint32 nFieldToken, const void *pData, int16 nIndex ) = 0;
	// CLibStackFieldsQueue::SetLibraryStackFields: packed fields of a library stack
	virtual bool SetLibraryStackFields( HSOSLIBSTACKHASH hStack, const void *pData, uint32 nSize ) = 0;
	virtual bool SetLibraryStackPublicField( const char *pStackName, const char *pFieldName, const void *pData, int16 nIndex ) = 0;
	// pStackField holds the stack hash followed by the field token
	virtual bool GetLibraryStackField( const uint32 *pStackField, void *pData, int16 nIndex ) = 0;
	virtual bool GetLibraryStackField( HSOSLIBSTACKHASH hStack, uint32 nFieldToken, void *pData, int16 nIndex ) = 0;
	virtual bool GetLibraryStackField( const char *pStackName, const char *pOperatorName, const char *pFieldName, void *pData, int16 nIndex ) = 0;
	virtual bool GetLibraryStackPublicField( const char *pStackName, const char *pFieldName, void *pData, int16 nIndex ) = 0;
	virtual SOAtomicDataType_t GetOpFieldAtomicType( HSOSLIBSTACKHASH hStack, uint32 nFieldToken ) = 0;
	// Field token from the operator and field names
	virtual CUtlStringToken GetFieldToken( const char *pOperatorName, const char *pFieldName ) = 0;
	virtual CUtlStringToken GetPublicFieldToken( const char *pFieldName ) = 0;
	virtual HSOSLIBSTACKHASH GetStackHash( const char *pStackName ) = 0;
	// 0 when stopped, 3 when paused
	virtual int GetSoundEventPlayState( SoundEventGuid_t guid ) = 0;
	virtual bool IsSoundEventPlaying( SoundEventGuid_t guid ) = 0;
	virtual bool IsSoundEventPaused( SoundEventGuid_t guid ) = 0;
	// -1 for an unknown guid
	virtual float GetSoundEventElapsedTime( SoundEventGuid_t guid ) = 0;
	virtual double GetSoundEventStartTime( SoundEventGuid_t guid ) = 0;
	virtual double GetSoundEventDuration( SoundEventGuid_t guid ) = 0;
	virtual bool FindSoundEventStackField( const char *pSoundEventName, const char *pFieldName, void *pFieldInfo ) = 0;
	// Reads a field of a library stack, converting it to the requested type
	virtual bool GetLibraryStackFieldValue( HSOSLIBSTACKHASH hStack, uint32 nFieldToken, void *pData, void *pDataType ) = 0;
	virtual bool GetLibraryStackFieldValue( HSOSLIBSTACKHASH hStack, const char *pOperatorName, const char *pFieldName, void *pData, void *pDataType ) = 0;
	virtual int GetFieldDataTypeAlignment( SOFieldDataType_t nType ) = 0;
	virtual int GetFieldDataTypeSize( SOFieldDataType_t nType ) = 0;
	virtual int GetFieldDataTypeElementSize( SOFieldDataType_t nType ) = 0;
	virtual CSosFieldData *CreateFieldDataFromKV3( const KeyValues3 *pKV ) = 0;
	virtual bool GetSoundEventStackFieldString( uint32 nSoundEvent, const char *pFieldName, CBufferString *pValue ) = 0;
	virtual bool GetSoundEventStackFieldString( const char *pSoundEventName, const char *pFieldName, CBufferString *pValue ) = 0;
	virtual CSosFieldData *AllocFieldData( SOFieldDataType_t nType, const void *pData ) = 0;
	virtual uint16 AddSymbol( const char *pString ) = 0;
	virtual uint16 FindSymbol( const char *pString ) = 0;
	virtual const char *GetSymbolString( uint16 nSymbol ) = 0;
	virtual void FlushStackCache() = 0;
	// Next serial for a started sound event
	virtual uint16 GetNextSoundEventSerial() = 0;
	virtual const char *GetSoundEventNameForGuid( SoundEventGuid_t guid ) = 0;
	// nOperation 1 increments, 2 decrements (both wrap at nMax), 3 picks a random value
	virtual int ApplySelectionOperation( void *pContext, uint8 *pValue, int nOperation, int nMax ) = 0;
	virtual const char *GetStackNameByIndex( int nIndex ) = 0;
	virtual void GetActiveSoundEventGuids( CUtlVector< SoundEventGuid_t > &list ) = 0;
	virtual bool IsSoundEventTracingEnabled() = 0;
	virtual void SetSoundEventTracingEnabled( bool bEnabled ) = 0;
	virtual int GetActiveSoundEventCount() = 0;
	virtual void SetRandomSeed( uint32 nSeed ) = 0;
	virtual void FlushVSounds() = 0;
	virtual bool ConvertStackKVToKV3( KeyValues *pKV, KeyValues3 *pKV3 ) = 0;
	virtual bool ConvertStackKV3ToKV( KeyValues3 *pKV3, KeyValues *pKV ) = 0;
	// "opFields" of a library stack
	virtual bool GetStackOpFieldsKV3( HSOSLIBSTACKHASH hStack, KeyValues3 *pKV ) = 0;
	virtual bool SetStackOpFieldsKV3( HSOSLIBSTACKHASH hStack, KeyValues3 *pKV ) = 0;
	virtual void AddSoundEventListener( void *pListener ) = 0;
	virtual void RemoveSoundEventListener( void *pListener ) = 0;
	virtual void *GetStackFieldInfo( HSOSLIBSTACKHASH hStack, uint32 nFieldToken ) = 0;
	virtual void *GetSoundEventFieldInfo( SoundEventGuid_t guid, uint32 nFieldToken ) = 0;
	virtual const char *GetStackName( HSOSLIBSTACKHASH hStack ) = 0;
	// Source of the sound event, -1 when it has none
	virtual SosReturnValue_t GetSoundEventSourceIndex( SoundEventGuid_t guid ) = 0;
	virtual bool GetSoundEventParam( SoundEventGuid_t guid, uint32 nFieldToken, void *pData, int16 nIndex ) = 0;
	virtual bool GetSoundEventPublicParam( SoundEventGuid_t guid, const char *pFieldName, void *pData, int16 nIndex ) = 0;
	virtual uint16 GetSoundEventSerial( SoundEventGuid_t guid ) = 0;
	// -2 for an unknown guid
	virtual int8 GetSoundEventLayer( SoundEventGuid_t guid ) = 0;
	virtual SosReturnValue_t GetSoundEventOwner( SoundEventGuid_t guid ) = 0;
	virtual SosReturnValue_t GetSoundEventSourceHandle( SoundEventGuid_t guid ) = 0;
	virtual SosReturnValue_t GetSoundEventStackInstance( SoundEventGuid_t guid ) = 0;
	virtual bool HasSoundEventList( void **ppList ) = 0;
	virtual const char *GetSoundEventNameFromList( void **ppList, int nIndex ) = 0;
	virtual void CollectSoundEventFields( const char *pSoundEventName, void *pFields, void *pContext ) = 0;
	// Non-zero makes StartSoundEvent and StartStack fail
	virtual void SetStartSoundEventBlocked( int nBlocked ) = 0;
	virtual void LockSoundEvents() = 0;
	virtual void UnlockSoundEvents() = 0;
	// "ParseStackKV3ForErrors: Cannot find "operators" array in stack %s"
	virtual void ParseStackKV3ForErrors( KeyValues3 *pStacks, bool bLog ) = 0;
};

#endif // ISOUNDOPSYSTEM_H
