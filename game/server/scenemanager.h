//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The scene manager: the running choreo scenes and the scene sounds to restore.
// Valve keeps it in game/server/sceneentity.cpp; game/server/sceneentity.h holds the flex scene
// file manager of another group, so this game system has a header of its own.
//
//=============================================================================//

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <entity2/entityidentity.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>

class CBaseFlex;
class CSceneEntity;
class CBasePlayerPawn;

//-----------------------------------------------------------------------------
// The interface the scene functions of the game call. No virtual destructor. QueueRestoredSound
// takes no sound level, RemoveActorFromScenes takes a scene to keep and the
// IsRunningScriptedScene* methods an output scene.
//-----------------------------------------------------------------------------
abstract_class ISceneManager
{
public:
	virtual void AddSceneEntity( CSceneEntity *pScene ) = 0;
	virtual void RemoveSceneEntity( CSceneEntity *pScene ) = 0;
	virtual void QueueRestoredSound( CBaseFlex *pActor, const char *pszSoundName, float flTimeInPast ) = 0;
	// Plays the queued sounds (and empties the queue) when a client becomes active
	virtual void OnClientActive( CBasePlayerPawn *pPlayer ) = 0;
	// pIgnoreScene: a scene that is not removed (named by what the code does)
	virtual void RemoveActorFromScenes( CBaseFlex *pActor, bool bInstancedOnly, bool bNonIdleOnly, const char *pszThisSceneOnly, CSceneEntity *pIgnoreScene ) = 0;
	virtual void PauseActorsScenes( CBaseFlex *pActor, bool bInstancedOnly ) = 0;
	virtual bool IsInInterruptableScenes( CBaseFlex *pActor ) = 0;
	virtual void ResumeActorsScenes( CBaseFlex *pActor, bool bInstancedOnly ) = 0;
	virtual void QueueActorsScenesToResume( CBaseFlex *pActor, bool bInstancedOnly ) = 0;
	// ppScene receives the scene found (optional)
	virtual bool IsRunningScriptedScene( CBaseFlex *pActor, bool bIgnoreInstancedScenes, CSceneEntity **ppScene ) = 0;
	virtual bool IsRunningScriptedSceneAndNotPaused( CBaseFlex *pActor, bool bIgnoreInstancedScenes, CSceneEntity **ppScene ) = 0;
	virtual bool IsRunningScriptedSceneWithSpeech( CBaseFlex *pActor, bool bIgnoreInstancedScenes, CSceneEntity **ppScene ) = 0;
	virtual bool IsRunningScriptedSceneWithSpeechAndNotPaused( CBaseFlex *pActor, bool bIgnoreInstancedScenes, CSceneEntity **ppScene ) = 0;
	// Named by what it does: the highest priority among the running scenes the
	// actor is in
	virtual int GetHighestScenePriority( CBaseFlex *pActor ) = 0;
};

//-----------------------------------------------------------------------------
// Registered as "CSceneManager" (reallocating factory). 80 bytes on both platforms.
//-----------------------------------------------------------------------------
class CSceneManager : public CAutoGameSystem, public ISceneManager
{
public:
	DECLARE_GAME_SYSTEM();

	// The sound name is a CUtlString; there is no sound level
	struct CRestoreSceneSound
	{
		CEntityHandle actor;
		CUtlString soundname;
		float time_in_past;
	};

	virtual ~CSceneManager();

	// Both empty
	GS_EVENT( GameInit );
	GS_EVENT( GameShutdown );
	// Thinks the active scenes
	GS_EVENT( ServerPreEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// ISceneManager
	virtual void AddSceneEntity( CSceneEntity *pScene ) override;
	virtual void RemoveSceneEntity( CSceneEntity *pScene ) override;
	virtual void QueueRestoredSound( CBaseFlex *pActor, const char *pszSoundName, float flTimeInPast ) override;
	virtual void OnClientActive( CBasePlayerPawn *pPlayer ) override;
	virtual void RemoveActorFromScenes( CBaseFlex *pActor, bool bInstancedOnly, bool bNonIdleOnly, const char *pszThisSceneOnly, CSceneEntity *pIgnoreScene ) override;
	virtual void PauseActorsScenes( CBaseFlex *pActor, bool bInstancedOnly ) override;
	virtual bool IsInInterruptableScenes( CBaseFlex *pActor ) override;
	virtual void ResumeActorsScenes( CBaseFlex *pActor, bool bInstancedOnly ) override;
	virtual void QueueActorsScenesToResume( CBaseFlex *pActor, bool bInstancedOnly ) override;
	virtual bool IsRunningScriptedScene( CBaseFlex *pActor, bool bIgnoreInstancedScenes, CSceneEntity **ppScene ) override;
	virtual bool IsRunningScriptedSceneAndNotPaused( CBaseFlex *pActor, bool bIgnoreInstancedScenes, CSceneEntity **ppScene ) override;
	virtual bool IsRunningScriptedSceneWithSpeech( CBaseFlex *pActor, bool bIgnoreInstancedScenes, CSceneEntity **ppScene ) override;
	virtual bool IsRunningScriptedSceneWithSpeechAndNotPaused( CBaseFlex *pActor, bool bIgnoreInstancedScenes, CSceneEntity **ppScene ) override;
	virtual int GetHighestScenePriority( CBaseFlex *pActor ) override;

public:
	CUtlVector< CRestoreSceneSound > m_QueuedSceneSounds;
	// CHandle< CSceneEntity >
	CUtlVector< CEntityHandle > m_ActiveScenes;
};

#endif // SCENEMANAGER_H
