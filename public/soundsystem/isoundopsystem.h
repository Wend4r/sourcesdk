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
class ISoundEventListener;
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
	// Looks up the resource of the sound event binding by name
	virtual bool unk07( const char *pSoundEventName ) = 0;
	virtual void ListSoundEvents( CUtlVector< const char * > &list ) const = 0;
	virtual void ListSoundEvents( CUtlVector< CUtlString > &list ) const = 0;
	// Sound events added with pBinding
	virtual void ListSoundEventsForResource( CUtlVector< const char * > &list, const ResourceBindingBase_t *pBinding ) = 0;
	// Case-insensitive substring match over the sound event names
	virtual void FindSoundEvents( const char *pSubString, CUtlVector< CUtlString > &list ) = 0;
	// Builds the edit tree of the sound events, the prefix is prepended to every name
	virtual void unk12( const CUtlString &sPrefix, void *pTree, const CUtlVector< const char * > &soundEventNames ) = 0;
	virtual void DereferenceSoundEvent( CSoundEvent *pSoundEvent ) = 0;
	virtual void RemoveSoundEvent( const char *pSoundEventName ) = 0;
	virtual void RemoveAllSoundEvents() = 0;
	virtual uint32 GetSoundEventUpdateStackHash( uint32 nSoundEvent ) const = 0;
	virtual uint32 GetSoundEventUpdateStackHash( const char *pSoundEventName ) const = 0;
	virtual void GetSoundEventUpdateStackName( uint32 nSoundEvent, CUtlString &sName ) = 0;
	// Stack flags of the update stack of the sound event, as returned by ISoundOpSystem::unk031
	virtual uint8 *unk19( uint32 nSoundEvent ) = 0;
	virtual bool unk20( uint32 nSoundEvent ) = 0;
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
	virtual void PreloadSoundEvent( uint32 nSoundEvent, uint16 nUnk ) = 0;
	virtual void PreloadSoundEvent( const char *pSoundEventName, uint16 nUnk ) = 0;
	// bUpdate replaces an existing sound event; without bNoDefer a sound event whose base or stack is missing is deferred;
	// bUnused is read by neither build
	virtual bool AddSoundEvent( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding, bool bUpdate, bool bNoDefer, bool bUnused ) = 0;
	virtual void AddSoundEvents( KeyValues3 *pKV, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void RemoveTimedOutDeferredSoundEvent() = 0;
	// Adds pSoundEventName as a copy of pSourceSoundEventName
	virtual bool unk39( const char *pSoundEventName, const char *pSourceSoundEventName, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void AddNewSoundEvent( const char *pSoundEventName, const ResourceBindingBase_t *pBinding ) = 0;
	// Sets the update stack of the sound event from the stack name
	virtual bool unk41( const char *pSoundEventName, const char *pStackName ) = 0;
	// Adds the field to the definition of the sound event
	virtual bool unk42( const char *pSoundEventName, const char *pFieldName, const CSosFieldData *pData ) = 0;
	// Sets a string in the KeyValues3 table of the sound event
	virtual bool unk43( const char *pSoundEventName, const char *pKey, const char *pValue ) = 0;
	virtual bool HasSoundEventField( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, int nIndex ) = 0;
	virtual bool HasSoundEventField( const char *pSoundEventName, const char *pFieldName, int nIndex ) = 0;
	virtual bool SetSoundEventDefinitionField( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, const CSosFieldData *pData, int nIndex ) = 0;
	// Selects the element of an array field
	virtual bool unk47( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, int nIndex ) = 0;
	// Element count of the field, pbArray tells whether it is an array field
	virtual int unk48( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, bool *pbArray, bool bIgnoreBases ) = 0;
	// Element count of the field
	virtual int unk49( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, bool bIgnoreBases ) = 0;
	// Fields the sound event does not set are looked up in its bases and then in its update stack, unless bIgnoreBases is set
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, SosFieldValueMem_t **ppData, int nIndex, bool bIgnoreBases ) = 0;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, CUtlString &sValue, int nIndex, bool bIgnoreBases ) = 0;
	// Writes the value to pKV
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, KeyValues3 *pKV, int nIndex, bool bIgnoreBases ) = 0;
	// unk53 and unk54 set and clear the flag that makes AddSoundEvent keep the sound events it replaces,
	// unk55 puts the kept sound events back
	virtual void unk53() = 0;
	virtual void unk54() = 0;
	virtual void unk55() = 0;
	virtual void SoundEventKVToKV3( KeyValues *pKV, KeyValues3 *pKV3 ) = 0;
	virtual void CompareSoundEvents( const char *pSoundEventA, const char *pSoundEventB ) = 0;
	// Value of the _builtins._voicegraph field
	virtual bool unk58( uint32 nSoundEvent, CUtlString &sValue ) = 0;
	virtual void ListDeferredSoundEvents() = 0;
	virtual void ClearDeferredSoundEvents() = 0;
	// Reads a string from the KeyValues3 table of the sound event
	virtual bool unk61( const char *pSoundEventName, const char *pKey, CUtlString &sValue ) = 0;
	// Both outputs are optional
	virtual bool unk62( const char *pSoundEventName, CUtlVector< CUtlString > *pVSndNames, float *pflMaxDuration ) = 0;
};

class ISoundOpSystem : public IAppSystem, public ISoundEventManager
{
public:
	// nSource is the entity the sound event is started for, invalid when it has none
	virtual SndOpEventGuid_t StartSoundEvent( const char *pSoundEventName, CEntityIndex nSource, sound_voice_layer_t nLayer ) = 0;
	virtual SndOpEventGuid_t StartSoundEvent( const char *pSoundEventName, void *pParams ) = 0;
	virtual SndOpEventGuid_t StartSoundEvent( void *pParams ) = 0;
	virtual SndOpEventGuid_t StartStack( const char *pStackName, CEntityIndex nSource, sound_voice_layer_t nLayer ) = 0;
	// Queues the stop of the sound event
	virtual void unk015( SoundEventGuid_t guid ) = 0;
	virtual bool StopSoundEvent( SoundEventGuid_t guid ) = 0;
	// An invalid nSource stops every instance of the sound event
	virtual void StopSoundEvent( uint32 nSoundEvent, CEntityIndex nSource ) = 0;
	virtual bool StopSoundEvent( const char *pSoundEventName, CEntityIndex nSource ) = 0;
	virtual bool unk019( SoundEventGuid_t guid ) = 0;
	// Stops the sound events started on nLayer, VOICE_LAYER_ALL stops every sound event
	virtual void unk020( sound_voice_layer_t nLayer ) = 0;
	// bStopSounds also stops every sound of the sound system
	virtual void StopAllSoundEvents( bool bStopSounds ) = 0;
	virtual bool PauseSoundEvent( SoundEventGuid_t guid ) = 0;
	virtual void PauseSoundEvents( uint32 nSoundEvent, CEntityIndex nSource ) = 0;
	virtual bool PauseSoundEvents( const char *pSoundEventName, CEntityIndex nSource ) = 0;
	// Pauses by voice layer, VOICE_LAYER_ALL with VOICE_LAYER_NONE pauses every sound event
	virtual void unk025( sound_voice_layer_t nLayer, sound_voice_layer_t nLayer2 ) = 0;
	virtual bool UnPauseSoundEvent( SoundEventGuid_t guid ) = 0;
	virtual void UnPauseSoundEvents( uint32 nSoundEvent, CEntityIndex nSource ) = 0;
	virtual bool UnPauseSoundEvents( const char *pSoundEventName, CEntityIndex nSource ) = 0;
	// Unpauses every paused sound event
	virtual void unk029() = 0;
	virtual CSosOperatorStack *GetStack( HSOSLIBSTACKHASH hStack ) = 0;
	// Stack flags, bit 0 is set when the stack starts a vsnd and bit 1 when it starts sound events
	virtual uint8 *unk031( HSOSLIBSTACKHASH hStack ) = 0;
	virtual bool SetSoundEventParam( SoundEventGuid_t guid, HSOSOPFIELDHASH nFieldToken, const CSosFieldData *pData, int16 nIndex ) = 0;
	// pData holds nSize bytes of packed field data, queued for the sound event or the library stack
	virtual bool SetSoundEventFields( SoundEventGuid_t guid, const void *pData, int nSize ) = 0;
	virtual bool unk034( SoundEventGuid_t guid, const char *pFieldName, const CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool SetLibraryStackField( HSOSLIBSTACKHASH hStack, HSOSOPFIELDHASH nFieldToken, const CSosFieldData *pData, int16 nIndex ) = 0;
	// Declared between the SetLibraryStackField overloads
	virtual bool SetLibraryStackFields( HSOSLIBSTACKHASH hStack, const void *pData, int nSize ) = 0;
	virtual bool SetLibraryStackField( const char *pStackName, const char *pOperatorName, const char *pFieldName, const CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool unk038( const char *pStackName, const char *pFieldName, const CSosFieldData *pData, int16 nIndex ) = 0;
	// pStackField holds the stack hash followed by the field token
	virtual bool GetLibraryStackField( const uint32 *pStackField, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool GetLibraryStackField( HSOSLIBSTACKHASH hStack, HSOSOPFIELDHASH nFieldToken, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool GetLibraryStackField( const char *pStackName, const char *pOperatorName, const char *pFieldName, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool unk042( const char *pStackName, const char *pFieldName, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual SOFieldDataType_t unk043( const char *pStackName, const char *pOperatorName, const char *pFieldName ) = 0;
	// Field token from the operator and field names
	virtual CUtlStringToken GetFieldToken( const char *pOperatorName, const char *pFieldName ) = 0;
	// Field token of a public field, or of a builtin one when the name starts with an underscore
	virtual CUtlStringToken unk045( const char *pFieldName ) = 0;
	virtual HSOSLIBSTACKHASH GetStackHash( const char *pStackName ) = 0;
	virtual int unk047( SoundEventGuid_t guid ) = 0;
	virtual bool IsSoundEventPlaying( SoundEventGuid_t guid ) = 0;
	virtual bool IsSoundEventPaused( SoundEventGuid_t guid ) = 0;
	virtual float unk050( SoundEventGuid_t guid ) = 0;
	virtual double unk051( SoundEventGuid_t guid ) = 0;
	virtual double unk052( SoundEventGuid_t guid ) = 0;
	// Public field names of the update stack of the sound event
	virtual void unk053( const char *pSoundEventName, CUtlVector< const char * > &list ) = 0;
	virtual bool GetSoundEventParam( SoundEventGuid_t guid, HSOSOPFIELDHASH nFieldToken, CSosFieldData *pData, int *pnCount ) = 0;
	virtual bool GetSoundEventParam( SoundEventGuid_t guid, const char *pOperatorName, const char *pFieldName, CSosFieldData *pData, int *pnCount ) = 0;
	virtual SOAtomicDataType_t unk056( SOFieldDataType_t nType ) = 0;
	virtual int GetFieldDataTypeSize( SOFieldDataType_t nType ) = 0;
	// Element count of the type
	virtual int unk058( SOFieldDataType_t nType ) = 0;
	// Allocates field data holding the value of pKV
	virtual CSosFieldData *unk059( KeyValues3 *pKV ) = 0;
	// Sets the field of the sound event definition from its text form
	virtual bool unk060( uint32 nSoundEvent, const char *pFieldName, const char *pValue ) = 0;
	virtual bool unk060( const char *pSoundEventName, const char *pFieldName, const char *pValue ) = 0;
	// Allocates field data of the type, pData may be null for a zero value
	virtual CSosFieldData *unk062( SOFieldDataType_t nType, const void *pData ) = 0;
	// String table of the operator system
	// unk063 adds the string, unk064 only looks it up
	virtual SosStringHandle_t unk063( const char *pString ) = 0;
	virtual SosStringHandle_t unk064( const char *pString ) = 0;
	virtual const char *unk065( SosStringHandle_t hString ) = 0;
	// Whether a library stack has the hash
	virtual bool unk066( HSOSLIBSTACKHASH hStack ) = 0;
	// Next random seed for a started sound event
	virtual SOSRANDSEED unk067() = 0;
	// Name of the sound event
	virtual const char *unk068( SoundEventGuid_t guid ) = 0;
	// Picks an index up to nMax: nMode 1 next, 2 previous, 3 random, 4 random without repeats (pState holds a bit per index),
	// 5 random by pWeights; for 1 and 2 pState holds the last index
	virtual int unk069( CSosOperatorStack *pStack, uint8 *pState, int nMode, int nMax, CUtlVector< float > *pWeights ) = 0;
	// Name of the update group
	virtual const char *unk070( int8 nGroup ) = 0;
	// Guids of the active sound events
	virtual void unk071( CUtlVector< SoundEventGuid_t > &list ) = 0;
	// Detailed tool debugging flag, unk073 only sets it while snd_sos_tools_detailed_debugging is on
	virtual bool unk072() = 0;
	virtual void unk073( bool bEnable ) = 0;
	// History entry kept for the sound event: its stack hash and the field data recorded while it played
	virtual void *unk074( SoundEventGuid_t guid ) = 0;
	// Sound event whose history is kept after it stops; the history of the previous one is removed unless it still plays
	virtual void unk075( SoundEventGuid_t guid ) = 0;
	// Symbol of the vsnd name in the vsnd resource type manager, used as the value of SOFTYPE_VSND fields
	virtual uint32 unk076( const char *pVsndName ) = 0;
	virtual bool unk077( KeyValues *pKV, KeyValues3 *pKV3 ) = 0;
	virtual bool unk078( const char *pName, KeyValues *pKV ) = 0;
	// Writes the operator fields of the stack to pKV
	virtual void unk079( HSOSLIBSTACKHASH hStack, KeyValues3 *pKV ) = 0;
	// Reads the operator fields of the stack from pKV
	virtual void unk080( HSOSLIBSTACKHASH hStack, KeyValues3 *pKV ) = 0;
	// Adds and removes a listener of the sound events
	virtual void unk081( ISoundEventListener *pListener ) = 0;
	virtual void unk082( ISoundEventListener *pListener ) = 0;
	// String of the field of the sound event or of the library stack
	virtual const char *unk083( SoundEventGuid_t guid, HSOSOPFIELDHASH nFieldToken ) = 0;
	virtual const char *unk083( HSOSLIBSTACKHASH hStack, HSOSOPFIELDHASH nFieldToken ) = 0;
	// Name of the library stack
	virtual const char *unk085( HSOSLIBSTACKHASH hStack ) = 0;
	// Source entity the sound event was started with
	virtual CEntityIndex unk086( SoundEventGuid_t guid ) = 0;
	virtual bool unk087( SoundEventGuid_t guid, HSOSOPFIELDHASH nFieldToken, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool unk088( SoundEventGuid_t guid, const char *pFieldName, CSosFieldData *pData, int16 nIndex ) = 0;
	// Random seed of the sound event
	virtual SOSRANDSEED unk089( SoundEventGuid_t guid ) = 0;
	// Voice layer the sound event was started on, VOICE_LAYER_NONE when it is not active
	virtual sound_voice_layer_t unk090( SoundEventGuid_t guid ) = 0;
	// Guid of the sound event that started this one, and of the first sound event of that chain; zero for none
	virtual SoundEventGuid_t unk091( SoundEventGuid_t guid ) = 0;
	virtual SoundEventGuid_t unk092( SoundEventGuid_t guid ) = 0;
	// Guid of the sound system voice the sound event plays, zero for none
	virtual SoundGuid_t unk093( SoundEventGuid_t guid ) = 0;
	// Count and element of a string list, ppList points to the list header
	virtual int unk094( void **ppList ) = 0;
	virtual const char *unk095( void **ppList, int nIndex ) = 0;
	// Names of the fields of the sound event that have the type
	virtual void unk096( const char *pSoundEventName, CUtlVector< const char * > &list, SOFieldDataType_t nType ) = 0;
	// Non-zero makes StartSoundEvent and StartStack fail, the engine sets it while a demo skips ahead
	virtual void unk097( int nSuppress ) = 0;
	// Locks and unlocks the operator system
	virtual void unk098() = 0;
	virtual void unk099() = 0;
	// Loads the stacks of the default manifest and reports their errors
	virtual void unk100( KeyValues3 *pStacks, void *pUnk ) = 0;
};

class CSoundEventManager : public ISoundEventManager
{
public:
	virtual uint32 GetSoundEventHash( const char *pSoundEventName ) const override;
	virtual bool IsValidSoundEventHash( uint32 nSoundEvent ) const override;
	virtual const char *GetSoundEventName( uint32 nSoundEvent ) const override;
	virtual bool HasSoundEvent( const char *pSoundEventName ) const override;
	virtual CSoundEvent *GetSoundEvent( uint32 nSoundEvent, bool bWarn ) const override;
	virtual CSoundEvent *GetSoundEvent( const char *pSoundEventName ) const override;
	virtual const ResourceBindingBase_t *GetSoundEventResourceBinding( const char *pSoundEventName ) override;
	virtual bool unk07( const char *pSoundEventName ) override;
	virtual void ListSoundEvents( CUtlVector< const char * > &list ) const override;
	virtual void ListSoundEvents( CUtlVector< CUtlString > &list ) const override;
	virtual void ListSoundEventsForResource( CUtlVector< const char * > &list, const ResourceBindingBase_t *pBinding ) override;
	virtual void FindSoundEvents( const char *pSubString, CUtlVector< CUtlString > &list ) override;
	virtual void unk12( const CUtlString &sPrefix, void *pTree, const CUtlVector< const char * > &soundEventNames ) override;
	virtual void DereferenceSoundEvent( CSoundEvent *pSoundEvent ) override;
	virtual void RemoveSoundEvent( const char *pSoundEventName ) override;
	virtual void RemoveAllSoundEvents() override;
	virtual uint32 GetSoundEventUpdateStackHash( uint32 nSoundEvent ) const override;
	virtual uint32 GetSoundEventUpdateStackHash( const char *pSoundEventName ) const override;
	virtual void GetSoundEventUpdateStackName( uint32 nSoundEvent, CUtlString &sName ) override;
	virtual uint8 *unk19( uint32 nSoundEvent ) override;
	virtual bool unk20( uint32 nSoundEvent ) override;
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
	virtual void PreloadSoundEvent( uint32 nSoundEvent, uint16 nUnk ) override;
	virtual void PreloadSoundEvent( const char *pSoundEventName, uint16 nUnk ) override;
	virtual bool AddSoundEvent( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding, bool bUpdate, bool bNoDefer, bool bUnused ) override;
	virtual void AddSoundEvents( KeyValues3 *pKV, const ResourceBindingBase_t *pBinding ) override;
	virtual void RemoveTimedOutDeferredSoundEvent() override;
	virtual bool unk39( const char *pSoundEventName, const char *pSourceSoundEventName, const ResourceBindingBase_t *pBinding ) override;
	virtual void AddNewSoundEvent( const char *pSoundEventName, const ResourceBindingBase_t *pBinding ) override;
	virtual bool unk41( const char *pSoundEventName, const char *pStackName ) override;
	virtual bool unk42( const char *pSoundEventName, const char *pFieldName, const CSosFieldData *pData ) override;
	virtual bool unk43( const char *pSoundEventName, const char *pKey, const char *pValue ) override;
	virtual bool HasSoundEventField( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, int nIndex ) override;
	virtual bool HasSoundEventField( const char *pSoundEventName, const char *pFieldName, int nIndex ) override;
	virtual bool SetSoundEventDefinitionField( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, const CSosFieldData *pData, int nIndex ) override;
	virtual bool unk47( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, int nIndex ) override;
	virtual int unk48( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, bool *pbArray, bool bIgnoreBases ) override;
	virtual int unk49( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, bool bIgnoreBases ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, SosFieldValueMem_t **ppData, int nIndex, bool bIgnoreBases ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, CUtlString &sValue, int nIndex, bool bIgnoreBases ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, KeyValues3 *pKV, int nIndex, bool bIgnoreBases ) override;
	virtual void unk53() override;
	virtual void unk54() override;
	virtual void unk55() override;
	virtual void SoundEventKVToKV3( KeyValues *pKV, KeyValues3 *pKV3 ) override;
	virtual void CompareSoundEvents( const char *pSoundEventA, const char *pSoundEventB ) override;
	virtual bool unk58( uint32 nSoundEvent, CUtlString &sValue ) override;
	virtual void ListDeferredSoundEvents() override;
	virtual void ClearDeferredSoundEvents() override;
	virtual bool unk61( const char *pSoundEventName, const char *pKey, CUtlString &sValue ) override;
	virtual bool unk62( const char *pSoundEventName, CUtlVector< CUtlString > *pVSndNames, float *pflMaxDuration ) override;

	// Sound events by hash
	CUtlHashtable< uint32, CSoundEvent *, MurmurHash3IntFunctor > m_unk008;
	// Sound events taken out of m_unk008 that are still referenced, deleted by DereferenceSoundEvent
	CUtlVector< CSoundEvent * > m_unk028;
	// Set by unk53 and cleared by unk54
	bool m_unk040;
	// Sound events that AddSoundEvent replaced while m_unk040 was set, unk55 puts them back into m_unk008
	CUtlHashtable< uint32, CSoundEvent *, MurmurHash3IntFunctor > m_unk048;
	// Deferred sound events, 0xa0 bytes each: the name (128 chars), its KeyValues3 and resource binding,
	// a done flag and the time it was deferred
	uint8 m_unk068[ 0x18 ];
	// Time of the last check for timed out deferred sound events
	double m_unk080;
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
	virtual bool unk07( const char *pSoundEventName ) override;
	virtual bool unk58( uint32 nSoundEvent, CUtlString &sValue ) override;
	virtual void ListSoundEvents( CUtlVector< const char * > &list ) const override;
	virtual void ListSoundEvents( CUtlVector< CUtlString > &list ) const override;
	virtual void ListSoundEventsForResource( CUtlVector< const char * > &list, const ResourceBindingBase_t *pBinding ) override;
	virtual void FindSoundEvents( const char *pSubString, CUtlVector< CUtlString > &list ) override;
	virtual void unk12( const CUtlString &sPrefix, void *pTree, const CUtlVector< const char * > &soundEventNames ) override;
	virtual void DereferenceSoundEvent( CSoundEvent *pSoundEvent ) override;
	virtual void RemoveSoundEvent( const char *pSoundEventName ) override;
	virtual void RemoveAllSoundEvents() override;
	virtual uint8 *unk19( uint32 nSoundEvent ) override;
	virtual uint32 GetSoundEventUpdateStackHash( uint32 nSoundEvent ) const override;
	virtual uint32 GetSoundEventUpdateStackHash( const char *pSoundEventName ) const override;
	virtual void GetSoundEventUpdateStackName( uint32 nSoundEvent, CUtlString &sName ) override;
	virtual bool unk20( uint32 nSoundEvent ) override;
	virtual uint32 GetSoundEventDefinitionBaseHash( uint32 nSoundEvent, int nBase ) override;
	virtual uint32 GetSoundEventDefinitionBaseHash( const char *pSoundEventName, int nBase ) override;
	virtual void GetSoundEventDefinitionBaseName( uint32 nSoundEvent, CUtlString &sName, int nBase ) override;
	virtual void GetSoundEventDefinitionBaseField( uint32 nSoundEvent, CUtlString &sField, int nBase ) override;
	virtual int GetSoundEventDefinitionBaseCount( uint32 nSoundEvent ) override;
	virtual int GetSoundEventDefinitionBaseCount( const char *pSoundEventName ) override;
	virtual bool SoundEventHasPreloadVsnd( const char *pSoundEventName ) override;
	virtual uint8 *GetSoundEventUpdateGroups( uint32 nSoundEvent ) override;
	virtual uint8 *GetSoundEventUpdateGroups( const char *pSoundEventName ) override;
	virtual int unk48( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, bool *pbArray, bool bIgnoreBases ) override;
	virtual int unk49( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, bool bIgnoreBases ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, SosFieldValueMem_t **ppData, int nIndex, bool bIgnoreBases ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, CUtlString &sValue, int nIndex, bool bIgnoreBases ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, HSOSOPFIELDHASH nFieldToken, KeyValues3 *pKV, int nIndex, bool bIgnoreBases ) override;
	virtual float GetSoundDuration( const char *pSoundEventName ) override;
	virtual float GetSoundDuration( uint32 nSoundEvent ) override;
	virtual const char *GetVSndNameForSoundEvent( const char *pSoundEventName, bool bFromSymbolTable ) override;
	virtual void GetVSndNameListForSoundEvent( const char *pSoundEventName, CUtlVector< const char * > &list, bool bFromSymbolTable ) override;
	virtual void PreloadSoundEvent( uint32 nSoundEvent, uint16 nUnk ) override;
	virtual void PreloadSoundEvent( const char *pSoundEventName, uint16 nUnk ) override;
	virtual bool AddSoundEvent( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding, bool bUpdate, bool bNoDefer, bool bUnused ) override;
	virtual void AddSoundEvents( KeyValues3 *pKV, const ResourceBindingBase_t *pBinding ) override;
	virtual void RemoveTimedOutDeferredSoundEvent() override;
	virtual bool unk39( const char *pSoundEventName, const char *pSourceSoundEventName, const ResourceBindingBase_t *pBinding ) override;
	virtual void AddNewSoundEvent( const char *pSoundEventName, const ResourceBindingBase_t *pBinding ) override;
	virtual bool unk41( const char *pSoundEventName, const char *pStackName ) override;
	virtual bool unk42( const char *pSoundEventName, const char *pFieldName, const CSosFieldData *pData ) override;
	virtual bool unk43( const char *pSoundEventName, const char *pKey, const char *pValue ) override;
	virtual bool HasSoundEventField( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, int nIndex ) override;
	virtual bool HasSoundEventField( const char *pSoundEventName, const char *pFieldName, int nIndex ) override;
	virtual bool SetSoundEventDefinitionField( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, const CSosFieldData *pData, int nIndex ) override;
	virtual bool unk47( CSoundEvent *pSoundEvent, HSOSOPFIELDHASH nFieldToken, int nIndex ) override;
	virtual void unk53() override;
	virtual void unk54() override;
	virtual void unk55() override;
	virtual void SoundEventKVToKV3( KeyValues *pKV, KeyValues3 *pKV3 ) override;
	virtual void CompareSoundEvents( const char *pSoundEventA, const char *pSoundEventB ) override;
	virtual void ListDeferredSoundEvents() override;
	virtual void ClearDeferredSoundEvents() override;
	virtual bool unk61( const char *pSoundEventName, const char *pKey, CUtlString &sValue ) override;
	virtual bool unk62( const char *pSoundEventName, CUtlVector< CUtlString > *pVSndNames, float *pflMaxDuration ) override;
	// Uses the default manifest
	virtual void LoadManifestStacksForParsing( KeyValues3 *pStacks, void *pUnk );
	virtual void LoadManifestStacksForParsing( const char *pManifestName, KeyValues3 *pStacks, void *pUnk );

	// Incremented by Init and decremented by Shutdown
	int m_unk030;
	CSoundEventManager m_unk038;
	CVSoundStackScriptTypeManager *m_unk0c0;
	CVSoundEventScriptTypeManager *m_unk0c8;
};

#endif // ISOUNDOPSYSTEM_H
