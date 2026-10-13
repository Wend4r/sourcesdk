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
#include "tier2/tier2.h"

class CSoundEvent;
class CSosOperatorStack;
class CUtlString;
class CUtlStringToken;
class CVSoundStackScriptTypeManager;
class CVSoundEventScriptTypeManager;
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
	virtual void unk12() = 0;
	virtual void DereferenceSoundEvent( CSoundEvent *pSoundEvent ) = 0;
	virtual void RemoveSoundEvent( const char *pSoundEventName ) = 0;
	virtual void RemoveAllSoundEvents() = 0;
	virtual uint32 GetSoundEventUpdateStackHash( uint32 nSoundEvent ) const = 0;
	virtual uint32 GetSoundEventUpdateStackHash( const char *pSoundEventName ) const = 0;
	virtual void GetSoundEventUpdateStackName( uint32 nSoundEvent, CUtlString &sName ) = 0;
	// Stack flags of the update stack of the sound event, as returned by ISoundOpSystem::unk031
	virtual void *unk19( uint32 nSoundEvent ) = 0;
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
	virtual bool AddSoundEvent( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding, bool bUpdate ) = 0;
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
	virtual bool HasSoundEventField( CSoundEvent *pSoundEvent, uint32 nFieldToken, int nIndex ) = 0;
	virtual bool HasSoundEventField( const char *pSoundEventName, const char *pFieldName, int nIndex ) = 0;
	virtual bool SetSoundEventDefinitionField( CSoundEvent *pSoundEvent, uint32 nFieldToken, const CSosFieldData *pData, int nIndex ) = 0;
	// Selects the element of an array field
	virtual bool unk47( CSoundEvent *pSoundEvent, uint32 nFieldToken, int nIndex ) = 0;
	// Element count of the field, pbArray tells whether it is an array field
	virtual int unk48( uint32 nSoundEvent, uint32 nFieldToken, bool *pbArray, bool bUnk ) = 0;
	// Element count of the field
	virtual int unk49( uint32 nSoundEvent, uint32 nFieldToken, bool bUnk ) = 0;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, uint32 nFieldToken, void **ppData, int nIndex, bool bUnk ) = 0;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, uint32 nFieldToken, CUtlString &sValue, int nIndex, bool bUnk ) = 0;
	// Writes the value to pKV
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, uint32 nFieldToken, KeyValues3 *pKV, int nIndex, bool bUnk ) = 0;
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
	// nSource is -1 when the sound event has no source
	virtual SndOpEventGuid_t StartSoundEvent( const char *pSoundEventName, uint32 nSource, int8 nUnk ) = 0;
	virtual SndOpEventGuid_t StartSoundEvent( const char *pSoundEventName, void *pParams ) = 0;
	virtual SndOpEventGuid_t StartSoundEvent( void *pParams ) = 0;
	virtual SndOpEventGuid_t StartStack( const char *pStackName, int nSource, int8 nUnk ) = 0;
	// Queues the stop of the sound event
	virtual void unk015( SoundEventGuid_t guid ) = 0;
	virtual bool StopSoundEvent( SoundEventGuid_t guid ) = 0;
	// nSource -1 stops every instance of the sound event
	virtual void StopSoundEvent( uint32 nSoundEvent, int nSource ) = 0;
	virtual bool StopSoundEvent( const char *pSoundEventName, int nSource ) = 0;
	virtual bool unk019( SoundEventGuid_t guid ) = 0;
	// Stops the sound events started with nUnk, -1 stops every sound event
	virtual void unk020( int8 nUnk ) = 0;
	// bStopSounds also stops every sound of the sound system
	virtual void StopAllSoundEvents( bool bStopSounds ) = 0;
	virtual bool PauseSoundEvent( SoundEventGuid_t guid ) = 0;
	virtual void PauseSoundEvents( uint32 nSoundEvent, int nSource ) = 0;
	virtual bool PauseSoundEvents( const char *pSoundEventName, int nSource ) = 0;
	virtual void unk025( int8 nUnk1, int8 nUnk2 ) = 0;
	virtual bool UnPauseSoundEvent( SoundEventGuid_t guid ) = 0;
	virtual void UnPauseSoundEvents( uint32 nSoundEvent, int nSource ) = 0;
	virtual bool UnPauseSoundEvents( const char *pSoundEventName, int nSource ) = 0;
	// Unpauses every paused sound event
	virtual void unk029() = 0;
	virtual CSosOperatorStack *GetStack( HSOSLIBSTACKHASH hStack ) = 0;
	// Stack flags, bit 0 is set when the stack starts a vsnd and bit 1 when it starts sound events
	virtual void *unk031( HSOSLIBSTACKHASH hStack ) = 0;
	virtual bool SetSoundEventParam( SoundEventGuid_t guid, uint32 nFieldToken, const CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool unk033( SoundEventGuid_t guid, const void *pFields, int nUnk ) = 0;
	virtual bool unk034( SoundEventGuid_t guid, const char *pFieldName, const CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool SetLibraryStackField( HSOSLIBSTACKHASH hStack, uint32 nFieldToken, const CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool unk037( HSOSLIBSTACKHASH hStack, const void *pFields, int nUnk ) = 0;
	virtual bool SetLibraryStackField( const char *pStackName, const char *pOperatorName, const char *pFieldName, const CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool unk038( const char *pStackName, const char *pFieldName, const CSosFieldData *pData, int16 nIndex ) = 0;
	// pStackField holds the stack hash followed by the field token
	virtual bool GetLibraryStackField( const uint32 *pStackField, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool GetLibraryStackField( HSOSLIBSTACKHASH hStack, uint32 nFieldToken, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool GetLibraryStackField( const char *pStackName, const char *pOperatorName, const char *pFieldName, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool unk042( const char *pStackName, const char *pFieldName, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual SOFieldDataType_t unk043( const char *pStackName, const char *pOperatorName, const char *pFieldName ) = 0;
	// Field token from the operator and field names
	virtual CUtlStringToken Unk_GetFieldToken( const char *pOperatorName, const char *pFieldName ) = 0;
	// Field token of a public field, or of a builtin one when the name starts with an underscore
	virtual CUtlStringToken unk045( const char *pFieldName ) = 0;
	virtual HSOSLIBSTACKHASH GetStackHash( const char *pStackName ) = 0;
	virtual int unk047( SoundEventGuid_t guid ) = 0;
	virtual bool IsSoundEventPlaying( SoundEventGuid_t guid ) = 0;
	virtual bool IsSoundEventPaused( SoundEventGuid_t guid ) = 0;
	virtual float unk050( SoundEventGuid_t guid ) = 0;
	virtual double unk051( SoundEventGuid_t guid ) = 0;
	virtual double unk052( SoundEventGuid_t guid ) = 0;
	virtual void unk053() = 0;
	virtual bool GetSoundEventParam( SoundEventGuid_t guid, uint32 nFieldToken, CSosFieldData *pData, int *pnCount ) = 0;
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
	// Symbol table of the operator system
	virtual uint16 unk063( const char *pString ) = 0;
	virtual uint16 unk064( const char *pString ) = 0;
	virtual const char *unk065( uint16 nSymbol ) = 0;
	virtual bool unk066( uint32 nUnk ) = 0;
	// Next random seed for a started sound event
	virtual uint16 unk067() = 0;
	// Name of the sound event
	virtual const char *unk068( SoundEventGuid_t guid ) = 0;
	virtual void unk069() = 0;
	// Name of the update group
	virtual const char *unk070( int8 nGroup ) = 0;
	// Guids of the active sound events
	virtual void unk071( CUtlVector< SoundEventGuid_t > &list ) = 0;
	virtual bool unk072() = 0;
	virtual void unk073( bool bUnk ) = 0;
	virtual void *unk074( uint32 nUnk ) = 0;
	virtual void unk075( int nUnk ) = 0;
	virtual void unk076() = 0;
	virtual bool unk077( KeyValues *pKV, KeyValues3 *pKV3 ) = 0;
	virtual bool unk078( const char *pName, KeyValues *pKV ) = 0;
	// Writes the operator fields of the stack to pKV
	virtual void unk079( HSOSLIBSTACKHASH hStack, KeyValues3 *pKV ) = 0;
	// Reads the operator fields of the stack from pKV
	virtual void unk080( HSOSLIBSTACKHASH hStack, KeyValues3 *pKV ) = 0;
	virtual void unk081() = 0;
	virtual void unk082( void *p ) = 0;
	virtual void unk083() = 0;
	virtual void unk084() = 0;
	virtual void unk085() = 0;
	virtual void unk086() = 0;
	virtual bool unk087( SoundEventGuid_t guid, uint32 nFieldToken, CSosFieldData *pData, int16 nIndex ) = 0;
	virtual bool unk088( SoundEventGuid_t guid, const char *pFieldName, CSosFieldData *pData, int16 nIndex ) = 0;
	// Random seed of the sound event
	virtual uint16 unk089( SoundEventGuid_t guid ) = 0;
	// nUnk the sound event was started with
	virtual int8 unk090( SoundEventGuid_t guid ) = 0;
	virtual void unk091() = 0;
	virtual void unk092() = 0;
	virtual void unk093() = 0;
	virtual void unk094() = 0;
	virtual void unk095() = 0;
	virtual void unk096() = 0;
	// Non-zero makes StartSoundEvent and StartStack fail
	virtual void unk097( int nUnk ) = 0;
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
	virtual void unk12() override;
	virtual void DereferenceSoundEvent( CSoundEvent *pSoundEvent ) override;
	virtual void RemoveSoundEvent( const char *pSoundEventName ) override;
	virtual void RemoveAllSoundEvents() override;
	virtual uint32 GetSoundEventUpdateStackHash( uint32 nSoundEvent ) const override;
	virtual uint32 GetSoundEventUpdateStackHash( const char *pSoundEventName ) const override;
	virtual void GetSoundEventUpdateStackName( uint32 nSoundEvent, CUtlString &sName ) override;
	virtual void *unk19( uint32 nSoundEvent ) override;
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
	virtual bool AddSoundEvent( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding, bool bUpdate ) override;
	virtual void AddSoundEvents( KeyValues3 *pKV, const ResourceBindingBase_t *pBinding ) override;
	virtual void RemoveTimedOutDeferredSoundEvent() override;
	virtual bool unk39( const char *pSoundEventName, const char *pSourceSoundEventName, const ResourceBindingBase_t *pBinding ) override;
	virtual void AddNewSoundEvent( const char *pSoundEventName, const ResourceBindingBase_t *pBinding ) override;
	virtual bool unk41( const char *pSoundEventName, const char *pStackName ) override;
	virtual bool unk42( const char *pSoundEventName, const char *pFieldName, const CSosFieldData *pData ) override;
	virtual bool unk43( const char *pSoundEventName, const char *pKey, const char *pValue ) override;
	virtual bool HasSoundEventField( CSoundEvent *pSoundEvent, uint32 nFieldToken, int nIndex ) override;
	virtual bool HasSoundEventField( const char *pSoundEventName, const char *pFieldName, int nIndex ) override;
	virtual bool SetSoundEventDefinitionField( CSoundEvent *pSoundEvent, uint32 nFieldToken, const CSosFieldData *pData, int nIndex ) override;
	virtual bool unk47( CSoundEvent *pSoundEvent, uint32 nFieldToken, int nIndex ) override;
	virtual int unk48( uint32 nSoundEvent, uint32 nFieldToken, bool *pbArray, bool bUnk ) override;
	virtual int unk49( uint32 nSoundEvent, uint32 nFieldToken, bool bUnk ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, uint32 nFieldToken, void **ppData, int nIndex, bool bUnk ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, uint32 nFieldToken, CUtlString &sValue, int nIndex, bool bUnk ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, uint32 nFieldToken, KeyValues3 *pKV, int nIndex, bool bUnk ) override;
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

	uint8 m_unk008[ 0x58 ];
	uint8 m_unk060[ 0x8 ];
	// Deferred sound events, 0xa0 bytes each
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
	virtual void unk12() override;
	virtual void DereferenceSoundEvent( CSoundEvent *pSoundEvent ) override;
	virtual void RemoveSoundEvent( const char *pSoundEventName ) override;
	virtual void RemoveAllSoundEvents() override;
	virtual void *unk19( uint32 nSoundEvent ) override;
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
	virtual int unk48( uint32 nSoundEvent, uint32 nFieldToken, bool *pbArray, bool bUnk ) override;
	virtual int unk49( uint32 nSoundEvent, uint32 nFieldToken, bool bUnk ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, uint32 nFieldToken, void **ppData, int nIndex, bool bUnk ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, uint32 nFieldToken, CUtlString &sValue, int nIndex, bool bUnk ) override;
	virtual bool GetSoundEventDefinitionField( uint32 nSoundEvent, uint32 nFieldToken, KeyValues3 *pKV, int nIndex, bool bUnk ) override;
	virtual float GetSoundDuration( const char *pSoundEventName ) override;
	virtual float GetSoundDuration( uint32 nSoundEvent ) override;
	virtual const char *GetVSndNameForSoundEvent( const char *pSoundEventName, bool bFromSymbolTable ) override;
	virtual void GetVSndNameListForSoundEvent( const char *pSoundEventName, CUtlVector< const char * > &list, bool bFromSymbolTable ) override;
	virtual void PreloadSoundEvent( uint32 nSoundEvent, uint16 nUnk ) override;
	virtual void PreloadSoundEvent( const char *pSoundEventName, uint16 nUnk ) override;
	virtual bool AddSoundEvent( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding, bool bUpdate ) override;
	virtual void AddSoundEvents( KeyValues3 *pKV, const ResourceBindingBase_t *pBinding ) override;
	virtual void RemoveTimedOutDeferredSoundEvent() override;
	virtual bool unk39( const char *pSoundEventName, const char *pSourceSoundEventName, const ResourceBindingBase_t *pBinding ) override;
	virtual void AddNewSoundEvent( const char *pSoundEventName, const ResourceBindingBase_t *pBinding ) override;
	virtual bool unk41( const char *pSoundEventName, const char *pStackName ) override;
	virtual bool unk42( const char *pSoundEventName, const char *pFieldName, const CSosFieldData *pData ) override;
	virtual bool unk43( const char *pSoundEventName, const char *pKey, const char *pValue ) override;
	virtual bool HasSoundEventField( CSoundEvent *pSoundEvent, uint32 nFieldToken, int nIndex ) override;
	virtual bool HasSoundEventField( const char *pSoundEventName, const char *pFieldName, int nIndex ) override;
	virtual bool SetSoundEventDefinitionField( CSoundEvent *pSoundEvent, uint32 nFieldToken, const CSosFieldData *pData, int nIndex ) override;
	virtual bool unk47( CSoundEvent *pSoundEvent, uint32 nFieldToken, int nIndex ) override;
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
