#ifndef AI_SPEECH_H
#define AI_SPEECH_H

#pragma once

#include "baseentity.h"

//-----------------------------------------------------------------------------
// CAI_ExpresserHost< T > is the class template between CBasePlayerPawn and
// CCSPlayerPawnBase and between the hostage and its base. It adds 5 methods to T, for a
// pawn Linux slots 401-405, Windows 400-404; the two overloads of a pair swap places on
// Windows. It also overrides NoteSpeaking, GetResponseSystem and DispatchResponse.
//-----------------------------------------------------------------------------
template < class TBase >
abstract_class CAI_ExpresserHostVTable : public TBase
{
public:
	// Hands the concept and the criteria to the expresser
	virtual bool Speak( const void *pConcept, void *pCriteria, void *pUnknown1, void *pUnknown2 ) = 0;
	// Gathers the criteria out of the modifiers first
	virtual bool Speak( const void *pConcept, const char *pszModifiers, void *pUnknown1, void *pUnknown2 ) = 0;
	// Speaks only when the expresser allows it: the host is alive, the concept may be spoken and, with bRespondingToPlayer, the speech timers have run out. CAI_PlayerAlly::SpeakIfAllowed of Source SDK 2013
	virtual bool SpeakIfAllowed( const void *pConcept, void *pCriteria, bool bRespondingToPlayer, void *pUnknown ) = 0;
	// Gathers the criteria out of the modifiers first
	virtual bool SpeakIfAllowed( const void *pConcept, const char *pszModifiers, bool bRespondingToPlayer, void *pUnknown ) = 0;
	// Empty in every class
	virtual void Stub_404() = 0;
};

#endif // AI_SPEECH_H
