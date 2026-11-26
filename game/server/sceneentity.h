//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The flex scene file manager. Factory "FlexSceneFileManager". Only this
//			game system is declared here.
//
//=============================================================================//

#ifndef SCENEENTITY_H
#define SCENEENTITY_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "tier0/utldict.h"

class CBaseFlex;
struct flexsettinghdr_t;

abstract_class IFlexSceneFileManager
{
public:
	// Returns the loaded .vfe file (flexsettinghdr_t), loading it when bAllowBlockingIO is set
	virtual const void *FindSceneFile( CBaseFlex *pInstance, const char *pFilename, bool bAllowBlockingIO ) = 0;
};

//-----------------------------------------------------------------------------
// Static instance, 72 bytes on both platforms. libserver.so repeats the
// FindSceneFile override at the end of the main table (slot 67).
//-----------------------------------------------------------------------------
class CFlexSceneFileManager : public CAutoGameSystem, public IFlexSceneFileManager
{
public:
	DECLARE_GAME_SYSTEM();

	// Loads the phoneme files ("phonemes", "phonemes_weak", "phonemes_strong"), then every
	// .vfe under "expressions"
	virtual bool Init() override;
	// Releases the loaded files and empties m_FileList
	virtual void Shutdown() override;

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// Loads every .vfe file under the folder (NULL: "expressions") and its subfolders
	virtual bool InitRecursive( const char *pFolder );

	// IFlexSceneFileManager
	virtual const void *FindSceneFile( CBaseFlex *pInstance, const char *pFilename, bool bAllowBlockingIO ) override;

public:
	// File name (allocated copy) -> loaded file
	CUtlDict< const flexsettinghdr_t *, int > m_FileList;
};

// BEGIN ENTITY VTABLES: generated, do not edit

#include "baseentity.h"
#include "../shared/ichoreoeventcallback.h"

class CChoreoActor;
class CChoreoEvent;
class CChoreoScene;
class CSceneEntity;
class CUtlSymbolLarge;

//-----------------------------------------------------------------------------
// CSceneEntity adds 61 methods to CPointEntity (through CPointEntity, which adds no method), Windows slots 238-298.
// The Linux table has 67, slots 239-305: 6 override methods of IChoreoEventCallback, which
// the Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CSceneEntityVTable : public TBase, public IChoreoEventCallback
{
public:
	virtual void OnLoaded() = 0;
	virtual void OnSceneFinished( bool bCanceled, bool bFireOutput ) = 0;
	// Overrides IChoreoEventCallback: a slot of this table on Linux only
	virtual void StartEvent( float flCurrentTime, CChoreoScene *pScene, CChoreoEvent *pEvent ) override = 0;
	// Overrides IChoreoEventCallback: a slot of this table on Linux only
	virtual void EndEvent( float flCurrentTime, CChoreoScene *pScene, CChoreoEvent *pEvent ) override = 0;
	// Overrides IChoreoEventCallback: a slot of this table on Linux only
	virtual void ProcessEvent( float flCurrentTime, CChoreoScene *pScene, CChoreoEvent *pEvent ) override = 0;
	// False while a requirement entry of the scene holds the event back; the events of type CChoreoEvent_BaseGlobalEvent never wait. Overrides IChoreoEventCallback: a slot of this table on Linux only
	virtual bool CanStartEvent( float flCurrentTime, CChoreoScene *pScene, CChoreoEvent *pEvent ) override = 0;
	// False while an unflagged requirement entry that starts no later than the event is still pending. Overrides IChoreoEventCallback: a slot of this table on Linux only
	virtual bool CanStopEvent( float flCurrentTime, CChoreoScene *pScene, CChoreoEvent *pEvent ) override = 0;
	// Finds the actor of the event ("unable to find actor") and asks its choreo component whether the event has completed. Overrides IChoreoEventCallback: a slot of this table on Linux only
	virtual bool CheckEvent( float flCurrentTime, CChoreoScene *pScene, CChoreoEvent *pEvent ) override = 0;
	virtual void DoThink( float flFrameTime ) = 0;
	virtual void PauseThink() = 0;
	virtual void ClearInterrupt() = 0;
	virtual void CheckInterruptCompletion() = 0;
	virtual bool InterruptThisScene( CSceneEntity *pOtherScene ) = 0;
	virtual void NotifyOfCompletion( CSceneEntity *pInterruptor ) = 0;
	virtual void StartPlayback() = 0;
	virtual void PausePlayback() = 0;
	virtual void ResumePlayback() = 0;
	virtual void CancelPlayback() = 0;
	virtual void InputCancelAtNextInterrupt( InputData_t *pInput ) = 0;
	virtual void PitchShiftPlayback( float flPitch ) = 0;
	// StartEvent or EndEvent routes the events of type vcd_pulse_requirement here; fires m_OnPulseRequirement
	virtual void DispatchStartVcdPulseRequirement( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the events of type external_animgraph here
	virtual void DispatchStartExternalAnimGraph( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the event type here
	virtual void DispatchEndExternalAnimGraph( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the events of type external_animgraph_control here
	virtual void DispatchStartExternalAnimGraphControl( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the event type here
	virtual void DispatchEndExternalAnimGraphControl( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchStartExpression( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchEndExpression( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchStartFlexAnimation( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchEndFlexAnimation( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchStartGesture( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchEndGesture( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchStartSpeak( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchEndSpeak( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchStartFace( CChoreoScene *pScene, CBaseEntity *pActor, CBaseEntity *pTarget, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchEndFace( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchStartSequence( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchEndSequence( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the events of type animclip here
	virtual void DispatchStartAnimClip( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the event type here
	virtual void DispatchEndAnimClip( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the events of type animgraphcontroller here; sets a graph parameter
	virtual void DispatchStartAnimGraphController( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the event type here
	virtual void DispatchEndAnimGraphController( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the events of type mood here; e_face_idle
	virtual void DispatchStartMood( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the event type here
	virtual void DispatchEndMood( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the events of type moodbody here; e_body_idle
	virtual void DispatchStartMoodBody( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the event type here
	virtual void DispatchEndMoodBody( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchStartInterrupt( CChoreoScene *pScene, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchEndInterrupt( CChoreoScene *pScene, CChoreoEvent *pEvent ) = 0;
	// Stops the event on the choreo component of the actor, the body of the other DispatchEnd methods; StartEvent and EndEvent route no event type here any more, and nothing overrides it
	virtual void DispatchEndGeneric( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the events of type locator here
	virtual void DispatchStartLocator( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the event type here
	virtual void DispatchEndLocator( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the events of type playresponse here; empty
	virtual void DispatchStartPlayResponse( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchStartPermitResponses( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchEndPermitResponses( CChoreoScene *pScene, CBaseEntity *pActor, CChoreoEvent *pEvent ) = 0;
	// EndEvent routes loop events here
	virtual void DispatchProcessLoop( CChoreoScene *pScene, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchPauseScene( CChoreoScene *pScene, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the events of type requirements here
	virtual void DispatchStartRequirements( CChoreoScene *pScene, CChoreoEvent *pEvent ) = 0;
	// StartEvent or EndEvent routes the event type here
	virtual void DispatchEndRequirements( CChoreoScene *pScene, CChoreoEvent *pEvent ) = 0;
	virtual void DispatchStopPoint( CChoreoScene *pScene ) = 0;
	virtual float EstimateLength() = 0;
	virtual float GetPostSpeakDelay() = 0;
	// MSVC orders the overload set in reverse
	virtual CBaseEntity *FindNamedActor( int nIndex ) = 0;
	virtual CBaseEntity *FindNamedActor( CChoreoActor *pChoreoActor ) = 0;
	virtual CBaseEntity *FindNamedActor( const char *pszName ) = 0;
	// Looks the actor up in the actor dictionary for a target name, with a special case for !player, then finds that entity; asks FindMappedActor first
	virtual CBaseEntity *Unk_295( CChoreoActor *pActor ) = 0;
	virtual CBaseEntity *FindNamedEntity( const char *pszName, CBaseEntity *pActor, bool bBaseFlexOnly, bool bUseClear ) = 0;
	virtual CBaseEntity *FindNamedTarget( CUtlSymbolLarge iszTarget, bool bBaseFlexOnly ) = 0;
	// Looks pszName up in m_ActorMap. Named by what it does
	virtual CBaseEntity *FindMappedActor( const char *pszName ) = 0;
};

//-----------------------------------------------------------------------------
// CInstancedSceneEntity adds 3 methods to CSceneEntity: Linux slots 306-308, Windows 299-301.
//-----------------------------------------------------------------------------
template < class TBase = CSceneEntityVTable<> >
abstract_class CInstancedSceneEntityVTable : public TBase
{
public:
	// m_hOwner when it is a combat character, else the actor map, else the FindNamedEntity of CSceneEntity
	virtual CBaseEntity *FindNamedEntity( const char *pszName ) = 0;
	virtual float GetPreDelay() = 0;
	virtual void SetPreDelay( float flDelay ) = 0;
};

// END ENTITY VTABLES

#endif // SCENEENTITY_H
