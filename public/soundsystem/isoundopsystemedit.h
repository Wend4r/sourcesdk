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
	virtual void unk035() = 0;
	virtual void unk036() = 0;
	virtual void unk037() = 0;
	virtual void unk038() = 0;
	virtual void unk039() = 0;
	virtual void unk040() = 0;
	virtual void unk041() = 0;
	virtual void unk042() = 0;
	virtual void unk043() = 0;
	virtual void unk044() = 0;
	// Element count of the field, pbArray tells whether it is an array field
	virtual int unk045( const char *pSoundEventName, const char *pFieldName, bool *pbArray ) = 0;
	virtual void unk046() = 0;
	virtual void unk047() = 0;
	virtual void unk048() = 0;
	virtual void unk049() = 0;
	virtual void unk050() = 0;
	virtual void unk051() = 0;
	virtual void unk052() = 0;
	virtual void unk053() = 0;
	virtual void unk054() = 0;
	virtual void unk055() = 0;
	// Calls ISoundOpSystem::SetSoundEventParam
	virtual bool unk056( SoundEventGuid_t guid, uint32 nFieldToken, const CSosFieldData *pData, int16 nIndex ) = 0;
	virtual void EditGetDomainAttrInfo( void *p1, void *p2, void *p3, void *p4 ) = 0;
	virtual bool unk060( void *p, uint32 nUnk1, uint32 nUnk2 ) = 0;
	virtual void EditGetDomainAttrInfo( void *p1, uint32 nUnk1, int nUnk2, void *p2, void *p3 ) = 0;
	virtual bool unk060( void *p1, uint32 nUnk1, int nUnk2, void *p2 ) = 0;
	virtual void EditGetDomainAttrInfo( const char *pSoundEventName, uint32 nUnk, void *p1, void *p2 ) = 0;
	virtual bool unk062( const char *pName1, const char *pName2, void *p ) = 0;
	virtual bool unk063( const char *pName1, const char *pName2, const char *pName3, void *p ) = 0;
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
	virtual void unk077() = 0;
	virtual void unk078() = 0;
	virtual void unk079() = 0;
	virtual void unk080() = 0;
	virtual void unk081() = 0;
	virtual void unk082() = 0;
	virtual void unk083() = 0;
	virtual void unk084() = 0;
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

#endif // ISOUNDOPSYSTEMEDIT_H
