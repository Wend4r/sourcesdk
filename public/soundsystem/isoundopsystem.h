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
#include "tier1/utlvector.h"

class CSoundEvent;
class CUtlString;
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

struct SoundEventGuid_t {
    SoundEventGuid_t()
		: m_Data(0)
    {
    }
	SoundEventGuid_t(const int32 id)
		: m_Data(id)
    {
    }

	int32 m_Data;
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
	virtual uint32			GetSoundEventHash( const char *pSoundEventName ) const = 0;
	virtual bool			IsValidSoundEventHash( uint32 nSoundEvent ) const = 0;
	virtual const char *GetSoundEventName( uint32 nSoundEvent ) const = 0;
	virtual bool			HasSoundEvent( const char *pSoundEventName ) const = 0;
	virtual CSoundEvent *GetSoundEvent( uint32 nSoundEvent ) const = 0;
	virtual CSoundEvent *GetSoundEvent( const char *pSoundEventName ) const = 0;

private:
	virtual void			unk06() = 0;
	virtual void			unk07() = 0;
	virtual void			unk08() = 0;

public:
	virtual void			ListSoundEvents( CUtlVector< const char * > &list ) const = 0;

private:
	virtual void			unk10() = 0;
	virtual void			unk11() = 0;
	virtual void			unk12() = 0;

public:
	virtual void			DereferenceSoundEvent( CSoundEvent *pSoundEvent ) = 0;
	virtual void			RemoveSoundEvent( const char *pSoundEventName ) = 0;
	virtual void			RemoveAllSoundEvents() = 0;
	virtual uint32			GetSoundEventUpdateStackHash( uint32 nSoundEvent ) const = 0;
	virtual uint32			GetSoundEventUpdateStackHash( const char *pSoundEventName ) const = 0;
	virtual void			GetSoundEventUpdateStackName( uint32 nSoundEvent, CUtlString &sName ) = 0;

private:
	virtual void			unk19() = 0;
	virtual void			unk20() = 0;

public:
	virtual uint32			GetSoundEventBaseHash( uint32 nSoundEvent ) const = 0;
	virtual uint32			GetSoundEventBaseHash( const char *pSoundEventName ) const = 0;
	virtual void			GetSoundEventBaseName( uint32 nSoundEvent, CUtlString &sName ) = 0;
	virtual void			GetSoundEventDefinitionBaseField( uint32 nSoundEvent ) = 0;
	virtual void			GetSoundEventDefinitionBaseCount( uint32 nSoundEvent ) = 0;
	virtual void			GetSoundEventDefinitionBaseCount( const char *pSoundEventName ) = 0;
	virtual bool			SoundEventHasPreloadVsnd() = 0;
	virtual uint8 *GetSoundEventUpdateGroups( uint32 nSoundEvent ) = 0;
	virtual uint8 *GetSoundEventUpdateGroups( const char *pSoundEventName ) = 0;
	virtual float			GetSoundDuration( const char *pSoundEventName, const char *pReason, float flDefault = 0.0f ) = 0;
	virtual float			GetSoundDuration( uint32 nSoundEvent, const char *pReason, float flDefault = 0.0f ) = 0;
	virtual const char *GetVSndNameForSoundEvent( const char *pSoundEventName, bool bFromSymbolTable ) = 0;
	virtual void			GetVSndNameListForSoundEvent( const char *pSoundEventName, CUtlVector< const char * > &list, bool bFromSymbolTable ) = 0;
	virtual void			PreloadSoundEventByName( const char *pSoundEventName ) = 0;
	virtual void			PreloadSoundEventByHash( uint32 nSoundEvent ) = 0;
	virtual bool			AddSoundEvent( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding, bool bUpdate ) = 0;
	virtual void			AddSoundEvents( KeyValues3 *pKV, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void			RemoveTimedOutDeferredSoundEvent() = 0;

private:
	virtual void			unk39() = 0;

public:
	virtual void			AddNewSoundEvent( const char *pSoundEventName, const ResourceBindingBase_t *pBinding ) = 0;

private:
	virtual void			unk41() = 0;
	virtual void			unk42() = 0;
	virtual void			unk43() = 0;
	virtual void			unk44() = 0;
	virtual void			unk45() = 0;

public:
	virtual void			SetSoundEventDefinitionField() = 0;

private:
	virtual void			unk47() = 0;
	virtual void			unk48() = 0;
	virtual void			unk49() = 0;
	virtual void			unk50() = 0;
	virtual void			unk51() = 0;
	virtual void			unk52() = 0;
	virtual void			unk53() = 0;
	virtual void			unk54() = 0;
	virtual void			unk55() = 0;

public:
	virtual void			SoundEventKVToKV3( KeyValues *pKV, KeyValues3 *pKV3 ) = 0;
	virtual void			CompareSoundEvents( const char *pSoundEventA, const char *pSoundEventB ) = 0;

private:
	virtual void			unk58() = 0;

public:
	virtual void			ListDeferredSoundEvents() = 0;

private:
	virtual void			unk60() = 0;
	virtual void			unk61() = 0;
	virtual void			unk62() = 0;
};

class ISoundOpSystem : public IAppSystem, public ISoundEventManager
{
};

#endif // ISOUNDOPSYSTEM_H
