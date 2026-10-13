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
class KeyValues3;
struct ResourceBindingBase_t;

abstract_class ISoundOpSystemEdit : public IAppSystem
{
public:
	virtual SndOpEventGuid_t StartSoundEvent( const char *pSoundEventName ) = 0;
	virtual bool StopSoundEvent( SoundEventGuid_t guid ) = 0;
	// Stops every sound event
	virtual void unk013() = 0;
	virtual bool IsSoundEventPlaying( SoundEventGuid_t guid ) = 0;
	// Forward to ISoundEventManager::AddSoundEvent, AddNewSoundEvent, ListSoundEventsForResource and ListSoundEvents
	virtual bool unk015( const char *pSoundEventName, KeyValues3 *pKV, const ResourceBindingBase_t *pBinding, bool bUpdate ) = 0;
	virtual void unk016( const char *pSoundEventName, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void unk017( CUtlVector< const char * > &list, const ResourceBindingBase_t *pBinding ) = 0;
	virtual void unk018( CUtlVector< const char * > &list ) = 0;
	virtual void FixupResourceName( const char *pName, CUtlString &sName ) = 0;
	// Sets the update stack of the sound event from the stack name
	virtual void unk020( const char *pSoundEventName, const char *pStackName ) = 0;
	// Base count of the sound event definition
	virtual int unk021( const char *pSoundEventName ) = 0;
	// Inserts a base sound event at nBase
	virtual bool unk022( const char *pSoundEventName, const char *pBaseSoundEventName, int nBase ) = 0;
	virtual bool unk022( const char *pSoundEventName, const char *pFieldName, const char *pBaseSoundEventName, int nBase ) = 0;
	// Formats the name and the field of the base as "name : field"
	virtual void unk024( const char *pSoundEventName, CUtlString &sName, int nBase ) = 0;
	// Inserts an empty base, removes a base and swaps two bases
	virtual void unk025( const char *pSoundEventName, int nBase ) = 0;
	virtual void unk026( const char *pSoundEventName, int nBase ) = 0;
	virtual void unk027( const char *pSoundEventName, int nBase1, int nBase2 ) = 0;
	virtual void unk028( const char *pSoundEventName, bool bPreloadVsnd ) = 0;
	virtual bool SoundEventHasPreloadVsnd( const char *pSoundEventName ) = 0;
	// Names of every update group
	virtual void unk030( CUtlVector< CUtlString > &list ) = 0;
	// Update groups of the sound event: count, set at nIndex, names and name at nIndex
	virtual int unk031( const char *pSoundEventName ) = 0;
	virtual void unk032( const char *pSoundEventName, const char *pGroupName, int nIndex ) = 0;
	virtual void unk033( const char *pSoundEventName, CUtlVector< const char * > &list ) = 0;
	virtual const char *unk034( const char *pSoundEventName, int nIndex ) = 0;
	// Names of the vsnds of the sound event
	virtual void unk035( const char *pSoundEventName, CUtlVector< const char * > &list ) = 0;
	// Public field names of the update stack of the sound event, then the names of its own fields
	virtual void unk036( const char *pSoundEventName, CUtlVector< CUtlString > *pList ) = 0;
	// First name of unk036 whose field has the type
	virtual bool unk037( const char *pSoundEventName, SOFieldDataType_t nType, CUtlString &sFieldName ) = 0;
	// Field editing of the sound event, unk038 adds the field with the values of its stack
	virtual void unk038( const char *pSoundEventName, const char *pFieldName ) = 0;
	virtual void unk039( const char *pSoundEventName, const char *pFieldName ) = 0;
	// Field names of the definition of the sound event
	virtual void unk040( const char *pSoundEventName, CUtlVector< CUtlString > *pList ) = 0;
	// Keys of the field table of the sound event
	virtual void unk041( const char *pSoundEventName, CUtlVector< CUtlString > *pList ) = 0;
	virtual void unk042( const char *pSoundEventName, const char *pFieldName, int16 nCount ) = 0;
	virtual void unk043( const char *pSoundEventName, const char *pFieldName ) = 0;
	// Sets an element of the field from its text form
	virtual bool unk044( const char *pSoundEventName, const char *pFieldName, const CUtlString &sValue, int16 nIndex ) = 0;
	// Element count of the field, pbArray tells whether it is an array field
	virtual int unk045( const char *pSoundEventName, const char *pFieldName, bool *pbArray ) = 0;
	// Element editing of an array field: resize, insert, remove and swap
	virtual void unk046( const char *pSoundEventName, const char *pFieldName, int nCount ) = 0;
	virtual void unk047( const char *pSoundEventName, const char *pFieldName, int nIndex ) = 0;
	virtual void unk048( const char *pSoundEventName, const char *pFieldName, int nIndex ) = 0;
	virtual void unk049( const char *pSoundEventName, const char *pFieldName, int nIndex1, int nIndex2 ) = 0;
	// Value names of an enum or mix group field of the update stack of the sound event
	virtual void unk050( const char *pSoundEventName, const char *pFieldName, CUtlVector< CUtlString > *pList ) = 0;
	// State of the field for the sound event (0 to 5), pnCount receives its element count
	virtual int unk051( const char *pSoundEventName, const char *pFieldName, int *pnCount, int nIndex ) = 0;
	// Calls unk044 when the state of unk051 allows it
	virtual bool unk052( const char *pSoundEventName, const char *pFieldName, const CUtlString &sValue, int16 nIndex ) = 0;
	virtual void unk053( const char *pSoundEventName, const char *pFieldName ) = 0;
	// Value of a public field of the stack and its element count
	virtual int unk054( HSOSLIBSTACKHASH hStack, const char *pFieldName, void **ppData, int nIndex ) = 0;
	// Element removal, depending on the state of unk051
	virtual void unk055( const char *pSoundEventName, const char *pFieldName, int nIndex ) = 0;
	// Calls ISoundOpSystem::SetSoundEventParam
	virtual bool unk056( SoundEventGuid_t guid, uint32 nFieldToken, const CSosFieldData *pData, int16 nIndex ) = 0;
	virtual void EditGetDomainAttrInfo( const char *pSoundEventName, uint32 nUnk1, int nUnk2, void *p2 ) = 0;
	virtual bool unk060( const char *pSoundEventName, uint32 nUnk1, uint32 nUnk2 ) = 0;
	virtual void EditGetDomainAttrInfo( const char *pSoundEventName, uint32 nUnk1, int nUnk2, void *p2, CUtlVector< CUtlString > *pList ) = 0;
	virtual bool unk060( const char *pSoundEventName, uint32 nUnk1, int nUnk2, CUtlVector< CUtlString > *pList ) = 0;
	virtual void EditGetDomainAttrInfo( const char *pSoundEventName, uint32 nUnk, void *p1, void *p2 ) = 0;
	virtual bool unk062( const char *pName1, const char *pName2, CUtlString &sValue ) = 0;
	virtual bool unk063( const char *pName1, const char *pName2, const char *pName3, CUtlString &sValue ) = 0;
	virtual bool unk063( const char *pName1, const char *pName2, const char *pName3 ) = 0;
	virtual void unk065() = 0;
	virtual void unk066() = 0;
	virtual void unk067() = 0;
	virtual void unk068() = 0;
	virtual void unk069() = 0;
	virtual void unk070() = 0;
	virtual void unk071() = 0;
	virtual void unk072() = 0;
	virtual void unk073() = 0;
	virtual void unk074() = 0;
	virtual void unk075() = 0;
	virtual void unk076() = 0;
	// Public field names of the stack, unk078 filters them
	virtual void unk077( HSOSLIBSTACKHASH hStack, CUtlVector< const char * > *pList ) = 0;
	virtual void unk078( HSOSLIBSTACKHASH hStack, CUtlVector< const char * > *pList ) = 0;
	// Text of a public field of the stack, nField indexes the list of unk077
	virtual void unk079( HSOSLIBSTACKHASH hStack, int nField, CUtlString &sValue, int nIndex ) = 0;
	virtual void unk079( HSOSLIBSTACKHASH hStack, const char *pFieldName, CUtlString &sValue, int nIndex ) = 0;
	// Element count of a public field of the stack
	virtual int unk081( HSOSLIBSTACKHASH hStack, int nField ) = 0;
	// Field data of the stack and its size
	virtual void *unk082( HSOSLIBSTACKHASH hStack, int *pnSize ) = 0;
	virtual bool unk083( HSOSLIBSTACKHASH hStack, uint32 nFieldToken ) = 0;
	virtual bool unk083( const char *pStackName, const char *pFieldName ) = 0;
	virtual void unk085() = 0;
	virtual void unk086() = 0;
	virtual void unk087() = 0;
	virtual void unk088() = 0;
	virtual void unk089() = 0;
	virtual void unk090() = 0;
	virtual void unk091() = 0;
	virtual void unk092() = 0;
	virtual void unk093() = 0;
	virtual void unk094() = 0;
	virtual void unk095() = 0;
	virtual void unk096() = 0;
	virtual void unk097() = 0;
	virtual void unk098() = 0;
	virtual void unk099() = 0;
};

class CSosEdit : public CTier2AppSystem< ISoundOpSystemEdit >
{
};

#endif // ISOUNDOPSYSTEMEDIT_H
