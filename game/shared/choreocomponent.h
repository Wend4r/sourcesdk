//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The choreo component of an animated entity: plays the events of choreographed
//          scenes (sequences, gestures, animation clips, look at, ...) on its owner.
//
//=============================================================================//

#ifndef CHOREOCOMPONENT_H
#define CHOREOCOMPONENT_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/globalsymbol.h"
#include "schemasystem/schematypes.h"

class CBaseEntity;
class CBaseAnimGraph;
class CChoreoEvent;
class CChoreoScene;
class CEntityHandle;
class CRR_Response;
class CSceneEntity;
class CSceneEventInfo;
class IRecipientFilter;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;

//-----------------------------------------------------------------------------
// CChoreoComponent is a schema class (m_hOwner, m_sActiveExternalChoreoGraphSlotID); this class
// declares its virtual methods only (see playerpawn_component.h for the VTable pattern). It
// has no base and no virtual destructor.
//
// The owner's HandleSceneEvent sends each scene event here by its type: when the event starts
// (sequence, animation clip, gesture, face, locator, external animgraph), while it runs
// (expression, flex animation, sequence, animation clip, gesture, face, locator, external
// animgraph and its control) and when it is cleared (animation clip, locator, external
// animgraph and its control). The handlers take the scene event info (m_pEvent / m_pScene
// at 80 / 88) and its event.
//-----------------------------------------------------------------------------
abstract_class CChoreoComponentVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() = 0;
	// Stores the owner in m_hOwner and the chained network variables ("Choreo Component created
	// without a valid CBaseModelEntity." otherwise); the change callback of the owner's
	// m_pChoreoComponent calls it
	virtual void SetOwner( CBaseEntity *pOwner ) = 0;
	// Add / remove the scene of the scene entity to / from the list of active scenes
	virtual void StartChoreoScene( CSceneEntity *pSceneEntity ) = 0;
	virtual void RemoveChoreoScene( CSceneEntity *pSceneEntity, bool bCanceled ) = 0;
	// Returns false; nothing in the server calls or overrides it
	virtual bool Stub_8() = 0;
	// Processes the active scene events (only those whose type bit is set in the mask, all of
	// them for 0) through the owner and marks them started; the owner's scene event think
	// calls it with 0
	virtual void ProcessSceneEvents( uint64 nEventTypeMask ) = 0;
	// Whether a locator event is complete; returns true. CheckSceneEvent asks it for the
	// locator events that are requirements
	virtual bool CheckSceneEventCompletion( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	// Starting events. The sequence and gesture handlers look the sequence named by the event up
	// on the owner, store it in the info and defer the layer (-2), warning "unable to find
	// sequence" / "unable to find gesture" when there is none; the animation clip handler takes
	// the clip and its slot ("invalid vnmclip", "no channel name for vnmclip"); the face handler
	// needs a target. Named by what they do after their sequence and gesture neighbours
	virtual bool HandleStartSequenceSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	virtual bool HandleStartAnimClipSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	virtual bool HandleStartGestureSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	virtual bool StartFacingSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	// Locator events (the move-to events of a scene); returns true
	virtual bool StartMoveToSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	virtual bool StartExternalAnimGraphSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	// Running events. The facing and move-to handlers need a target; facing enters the scene
	// sequence first and holds still when the target is the owner
	virtual bool ProcessFlexAnimationSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	virtual bool ProcessFlexSettingSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	virtual bool ProcessSequenceSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	virtual bool ProcessAnimClipSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	// Allocates the deferred gesture layer first ("unable to find gesture" when the sequence is
	// missing), then plays the gesture at the event time and blends its weight
	virtual bool ProcessGestureSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	virtual bool ProcessFacingSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	virtual bool ProcessMoveToSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	// Returns true
	virtual bool ProcessExternalAnimGraphSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	// Finds the animgraph controller of the event
	virtual bool ProcessExternalAnimGraphControlSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	// Cleared events. Named by what they do after the handlers above; the external animgraph
	// handlers find and remove the animgraph controller of the event
	virtual bool ClearAnimClipSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	// Returns true
	virtual bool ClearMoveToSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	virtual bool ClearExternalAnimGraphSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	virtual bool ClearExternalAnimGraphControlSceneEvent( CSceneEventInfo *pInfo, CChoreoEvent *pEvent ) = 0;
	// Returns 3 when the animation clip slot of an active scene event is in use in the owner's
	// animgraph, 0 otherwise; nothing in the server calls or overrides it
	virtual int Stub_30() = 0;
	// Play a scene file / a scene made for a sound on the owner (an instanced scene entity) and
	// return its length
	virtual float PlayScene( const char *pszScene, float flDelay, CRR_Response *pResponse, IRecipientFilter *pFilter, CEntityHandle *phSceneEntity ) = 0;
	virtual float PlayAutoGeneratedSoundScene( const char *pszSoundName, CRR_Response *pResponse, IRecipientFilter *pFilter ) = 0;
	// Named by what it does: stores m_sActiveExternalChoreoGraphSlotID
	virtual void SetActiveExternalChoreoGraphSlotID( const CGlobalSymbol &slotID ) = 0;
	// Whether the owner may play the scene sequence of the event now; returns true. The facing
	// handler asks it with bRestart true
	virtual bool EnterSceneSequence( CBaseAnimGraph *pOwner, CChoreoScene *pScene, CChoreoEvent *pEvent, bool bRestart ) = 0;
};

#endif // CHOREOCOMPONENT_H
