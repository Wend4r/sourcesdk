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

abstract_class ISoundOpSystemEdit : public IAppSystem
{
public:
	virtual SndOpEventGuid_t StartSoundEvent( const char *pSoundEventName ) = 0;
	virtual void unk012() = 0;
	virtual void unk013() = 0;
	virtual bool IsSoundEventPlaying( SoundEventGuid_t guid ) = 0;
	virtual void unk015() = 0;
	virtual void unk016() = 0;
	virtual void unk017() = 0;
	virtual void unk018() = 0;
	virtual void FixupResourceName( const char *pName, CUtlString &sName ) = 0;
	virtual void unk020() = 0;
	virtual void unk021() = 0;
	virtual bool unk022( void *p, const char *pName, uint32 nUnk ) = 0;
	virtual bool unk022( void *p1, void *p2, const char *pName, uint32 nUnk ) = 0;
	virtual void unk024() = 0;
	virtual void unk025() = 0;
	virtual void unk026() = 0;
	virtual void unk027() = 0;
	virtual void unk028() = 0;
	virtual bool SoundEventHasPreloadVsnd( const char *pSoundEventName ) = 0;
	virtual void unk030() = 0;
	virtual void unk031() = 0;
	virtual void unk032() = 0;
	virtual void unk033() = 0;
	virtual void unk034() = 0;
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
	virtual void unk045() = 0;
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
	virtual void unk056() = 0;
	virtual void unk057( void *p1, void *p2, void *p3, void *p4 ) = 0;
	virtual bool unk060( void *p, uint32 nUnk1, uint32 nUnk2 ) = 0;
	virtual void unk057( void *p1, uint32 nUnk1, int nUnk2, void *p2, void *p3 ) = 0;
	virtual bool unk060( void *p1, uint32 nUnk1, int nUnk2, void *p2 ) = 0;
	// Reports a missing field as EditGetDomainAttrInfo
	virtual void unk057( const char *pSoundEventName, uint32 nUnk, void *p1, void *p2 ) = 0;
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
