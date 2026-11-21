//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Player-driven voting system: the base class of the vote issues
//
//=============================================================================//

#ifndef VOTE_CONTROLLER_H
#define VOTE_CONTROLLER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "playerslot.h"
#include "schemasystem/schematypes.h"
#include "shareddefs.h"

#include <tier0/utlvector.h>

#include "schemacompiler/schemaon.h"

class CVoteController;

#define MAX_COMMAND_LENGTH 64
#define MAX_CREATE_ERROR_STRING 96

// The length of the details buffers of the issues: MAX_PATH of the platform the game was built
// for. sourcesdk's MAX_PATH is 260 everywhere, the Linux game uses 4096 (PATH_MAX).
#ifdef _WIN32
#define VOTE_ISSUE_MAX_PATH 260
#else
#define VOTE_ISSUE_MAX_PATH 4096
#endif

// The type strings of the issues: the first argument of callvote
#define VOTEISSUE_NAME_KICK "Kick"
#define VOTEISSUE_NAME_CHANGELEVEL "ChangeLevel"
#define VOTEISSUE_NAME_NEXTLEVEL "NextLevel"
#define VOTEISSUE_NAME_SWAPTEAMS "SwapTeams"
#define VOTEISSUE_NAME_SCRAMBLE "ScrambleTeams"
#define VOTEISSUE_NAME_RESTARTGAME "RestartGame"
#define VOTEISSUE_NAME_SURRENDER "Surrender"
#define VOTEISSUE_NAME_PAUSEMATCH "PauseMatch"
#define VOTEISSUE_NAME_UNPAUSEMATCH "UnpauseMatch"
#define VOTEISSUE_NAME_LOADBACKUP "LoadBackup"
#define VOTEISSUE_NAME_READYFORMATCH "ReadyForMatch"
#define VOTEISSUE_NAME_NOTREADYFORMATCH "NotReadyForMatch"
#define VOTEISSUE_NAME_STARTTIMEOUT "StartTimeOut"

//-----------------------------------------------------------------------------
// Abstract base class of all things that can be voted on. The game rules create one of each
// issue per vote controller; the constructor copies the type string and appends the issue to
// m_potentialIssues of the controller. A schema class (vtable, abstract): 376 bytes on Windows,
// 4216 on Linux. 34 virtual slots on Linux (two destructor slots), 33 on Windows.
//-----------------------------------------------------------------------------
schema class CBaseIssue
{
public:
	// Linux slot 0, Windows 0
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();
	// Frees the entries of m_FailedVotes. Linux slots 1-2, Windows 1
	virtual ~CBaseIssue();

	// m_szDetailsString. Linux slot 3, Windows 2
	virtual const char *GetDetailsString();
	// Copies the details part of the callvote command into m_szDetailsString. Linux slot 4, Windows 3
	virtual void SetIssueDetails( const char *pszDetails );
	// Adds or refreshes the entry of m_FailedVotes for the type and details string, locked out for
	// GetFailedVoteLockOutTime seconds. Linux slot 5, Windows 4
	virtual void OnVoteFailed();
	// Empty. Linux slot 6, Windows 5
	virtual void OnVoteStarted();
	// Returns false; every issue asks its sv_vote_issue_*_allowed convar. Linux slot 7, Windows 6
	virtual bool IsEnabled();
	// Returns false: whether the issue can be called in a queued matchmaking game. Linux slot 8, Windows 7
	virtual bool IsEnabledInQueuedMatchmaking();
	// Returns false: whether the issue can be called during the warmup. Linux slot 9, Windows 8
	virtual bool IsEnabledDuringWarmup();
	// The value of a convar: seconds between the vote passing and ExecuteCommand. Linux slot 10, Windows 9
	virtual float GetCommandDelay();
	// Returns false: whether the issue ignores the creation timer that stops callvote spam. Linux slot 11, Windows 10
	virtual bool ShouldIgnoreCreationTimer();
	// Returns true: whether a member of the team can call the issue. Linux slot 12, Windows 11
	virtual bool CanTeamCallVote( int iTeam ) const;
	// Checks the warmup, the team, the spectator restriction, queued matchmaking and the failed
	// vote lockouts; fills nFailCode and nTime when it refuses. Slot 99 (the server) always may.
	// Linux slot 13, Windows 12
	virtual bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime );
	// Returns false: whether only the team of the caller votes. Linux slot 14, Windows 13
	virtual bool IsAllyRestrictedVote();
	// Returns false; when true, a voter may cast another option (CVoteController::TryCastVote) and
	// the controller does not close the vote early. Named by what it does. Linux slot 15, Windows 14
	virtual bool IsVoteChangeAllowed();
	// Returns false: whether every potential voter must vote yes. Linux slot 16, Windows 15
	virtual bool IsUnanimousVoteToPass();
	// Returns false; when true, GetVotesRequiredToPass returns 1. Named by what it does. Linux slot 17, Windows 16
	virtual bool IsSingleVoteToPass();
	// Returns false: whether only spectators can call the issue. Linux slot 18, Windows 17
	virtual bool IsVoteCallExclusiveToSpectators();
	// The yes votes needed: a convar ratio of the potential voters, all of them for a
	// unanimous issue. Linux slot 19, Windows 18
	virtual int GetVotesRequiredToPass();
	// The string passed to the clients for display. Linux slot 20, Windows 19
	virtual const char *GetDisplayString() = 0;
	// The string shown to the other team of an ally restricted vote. Linux slot 21, Windows 20
	virtual const char *GetOtherTeamDisplayString() = 0;
	// Carries out the issue that passed. Linux slot 22, Windows 21
	virtual void ExecuteCommand() = 0;
	// Prints "callvote <type> ..." to the console of the player in the slot. Linux slot 23, Windows 22
	virtual void ListIssueDetails( CPlayerSlot nForWhom ) = 0;
	// Returns "Unknown vote passed.". Linux slot 24, Windows 23
	virtual const char *GetVotePassedString();
	// Counts the connected players that can vote on the issue. Linux slot 25, Windows 24
	virtual int CountPotentialVoters();
	// Returns 2: how many options the issue has. Linux slot 26, Windows 25
	virtual int GetNumberVoteOptions();
	// Returns true. Linux slot 27, Windows 26
	virtual bool IsYesNoVote();
	// Stores the three counts. Linux slot 28, Windows 27
	virtual void SetYesNoVoteCount( int iNumYesVotes, int iNumNoVotes, int iNumPotentialVotes );
	// Fills vecNames with "Yes" and "No" and returns true. Linux slot 29, Windows 28
	virtual bool GetVoteOptions( CUtlVector< const char * > &vecNames );
	// Returns VOTEISSUE_UNDEFINED; every issue returns its VOTEISSUE_ value. Linux slot 30, Windows 29
	virtual int GetVoteIssue();
	// The value of a convar: how long a failed vote stays locked out. Linux slot 31, Windows 30
	virtual float GetFailedVoteLockOutTime();
	// Returns eDefaultFailCode: the failure code sent to the clients. Linux slot 32, Windows 31
	virtual vote_create_failed_t MakeVoteFailErrorCodeForClients( vote_create_failed_t eDefaultFailCode );
	// Returns an invalid slot; CKickIssue returns the slot of the player to kick. The controller
	// does not let that player vote. Named by what it does. Linux slot 33, Windows 32
	virtual CPlayerSlot GetTargetPlayerSlot();

protected:
	// A vote that failed: the lockout of its type and details string (132 bytes)
	struct FailedVote
	{
		char szFailedVoteCommand[ MAX_COMMAND_LENGTH ];
		char szFailedVoteParameter[ MAX_VOTE_DETAILS_LENGTH ];
		float flLockoutTime;
	};

	noschema CUtlVector< FailedVote * > m_FailedVotes;

	// The type string: the first argument of callvote
	char m_szTypeString[ MAX_COMMAND_LENGTH ];
	// The details part of the callvote command
	char m_szDetailsString[ VOTE_ISSUE_MAX_PATH ];

	int32 m_iNumYesVotes;
	int32 m_iNumNoVotes;
	int32 m_iNumPotentialVotes;

	CVoteController *m_pVoteController;
};

#include "schemacompiler/schemaoff.h"

#endif // VOTE_CONTROLLER_H
