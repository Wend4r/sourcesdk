#ifndef AI_SPEECH_H
#define AI_SPEECH_H

#pragma once

#include "baseentity.h"
#include "responserules/response_types.h"

class CAI_Concept;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CRR_Response;
class CResponseCriteriaSet;
class IRecipientFilter;

//-----------------------------------------------------------------------------
// Receives the notifications of the expresser of its host. The second base of
// CAI_ExpresserHost< T >; no class of the server overrides its methods. Four slots on both
// platforms, no virtual destructor. No data: 8 bytes, the virtual table pointer.
//-----------------------------------------------------------------------------
abstract_class CAI_ExpresserSink
{
public:
	// Returns false; no class overrides it
	virtual bool Unk_0() = 0;
	// Empty
	virtual void OnSpokeConcept( CAI_Concept concept, CRR_Response *pResponse ) = 0;
	// Empty
	virtual void OnStartSpeaking() = 0;
	// Returns true
	virtual bool UseSemaphore() = 0;
};

//-----------------------------------------------------------------------------
// CAI_ExpresserHost< T > is the class template between CBasePlayerPawn and
// CCSPlayerPawnBase and between the hostage and its base. It adds 5 methods to T, for a
// pawn Linux slots 401-405, Windows 400-404; the two overloads of a pair swap places on
// Windows. It also overrides NoteSpeaking, GetResponseSystem and DispatchResponse.
// No data of its own: its only addition is the second base CAI_ExpresserSink (8 bytes, the
// virtual table pointer), which sits right after T; the members of the class below start after it
// (after its padding to 16 on Windows). The second base sits at offset 8 here, not where the game
// has it: do not convert a pointer to it.
//-----------------------------------------------------------------------------
template < class TBase >
abstract_class CAI_ExpresserHost : public TBase, public CAI_ExpresserSink
{
public:
	// Hands the concept and the criteria to the expresser
	virtual bool Speak( const void *pConcept, void *pCriteria, void *pUnknown1, void *pUnknown2 ) = 0;
	// Gathers the criteria out of the modifiers first
	virtual bool Speak( const void *pConcept, const char *pszModifiers, void *pUnknown1, void *pUnknown2 ) = 0;
	// Speaks only when the expresser allows it: the host is alive, the concept may be spoken and, with bRespondingToPlayer, the speech timers have run out.
	virtual bool SpeakIfAllowed( const void *pConcept, void *pCriteria, bool bRespondingToPlayer, void *pUnknown ) = 0;
	// Gathers the criteria out of the modifiers first
	virtual bool SpeakIfAllowed( const void *pConcept, const char *pszModifiers, bool bRespondingToPlayer, void *pUnknown ) = 0;
	// Empty in every class
	virtual void Stub_404() = 0;
};

//-----------------------------------------------------------------------------
// The virtual table of CAI_Expresser, the speech of an entity (160 bytes), a schema class: a
// module declares its own CAI_Expresser, so this one carries the VTable suffix and declares
// virtual methods only. Implements IResponseFilter and adds ten slots, Linux 1-10, Windows
// 1-9. The game
// derives CAI_ExpresserWithFollowup from it (overrides SpeakDispatchResponse and
// OnSpeechFinished) and CMultiplayer_Expresser from that (overrides IsSpeaking); neither adds a
// virtual method.
//-----------------------------------------------------------------------------
abstract_class CAI_ExpresserVTable : public IResponseFilter
{
public:
	// Named by what it does: whether the respeak time of the concept has passed in the concept history
	virtual bool CanSpeakConcept( const CAI_Concept &concept ) = 0;
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual ~CAI_ExpresserVTable() = 0;
	virtual bool SpeakDispatchResponse( CAI_Concept &concept, CRR_Response *pResponse, CResponseCriteriaSet *pCriteria, IRecipientFilter *pFilter ) = 0;
	// Empty here
	virtual void OnSpeechFinished() = 0;
	virtual bool IsSpeaking() = 0;
	// Named by what they do, as the KV3 transfer methods of other schema classes
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	// Named by what it does: returns "CAI_Expresser"
	virtual const char *GetClassName() const = 0;
};

#endif // AI_SPEECH_H
