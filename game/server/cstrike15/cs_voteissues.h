//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The Counter-Strike vote issues
//
//=============================================================================//

#ifndef CSTRIKE15_CS_VOTEISSUES_H
#define CSTRIKE15_CS_VOTEISSUES_H
#ifdef _WIN32
#pragma once
#endif

#include "../vote_controller.h"
#include "steam/steamclientpublic.h"

// Reasons of a kick or ban vote; do not re-order
enum
{
	kVoteKickBanPlayerReason_Other,
	kVoteKickBanPlayerReason_Cheating,
	kVoteKickBanPlayerReason_Idle,
	kVoteKickBanPlayerReason_Scamming,
};

//-----------------------------------------------------------------------------
// The base of the Counter-Strike issues: no fields of its own. The constructor is inline in
// every issue, so Windows emits no table for it. Overrides GetOtherTeamDisplayString
// ("#SFUI_otherteam_vote_unimplemented") and GetVoteIssue (VOTEISSUE_UNDEFINED).
//-----------------------------------------------------------------------------
class CBaseCSIssue : public CBaseIssue
{
public:
	const char *GetOtherTeamDisplayString() override;
	int GetVoteIssue() override;
};

//-----------------------------------------------------------------------------
// "RestartGame": restarts the game. Same size as CBaseIssue.
//-----------------------------------------------------------------------------
class CRestartGameIssue : public CBaseCSIssue
{
public:
	~CRestartGameIssue();
	bool IsEnabled() override;
	bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime ) override;
	// Returns false
	bool IsAllyRestrictedVote() override;
	// "#SFUI_vote_restart_game"
	const char *GetDisplayString() override;
	void ExecuteCommand() override;
	void ListIssueDetails( CPlayerSlot nForWhom ) override;
	// "#SFUI_vote_passed_restart_game"
	const char *GetVotePassedString() override;
	// VOTEISSUE_RESTARTGAME
	int GetVoteIssue() override;
};

//-----------------------------------------------------------------------------
// "Kick": kicks the player whose slot leads the details string. 392 bytes on Windows, 4232 on
// Linux.
//-----------------------------------------------------------------------------
class CKickIssue : public CBaseCSIssue
{
public:
	~CKickIssue();
	// The name of the player in the slot of the details string, "unnamed" without one
	const char *GetDetailsString() override;
	// Also clears m_steamIDtoBan
	void OnVoteFailed() override;
	// The player to kick votes no; stores the SteamID of that player in m_steamIDtoBan
	void OnVoteStarted() override;
	bool IsEnabled() override;
	// Returns true
	bool IsEnabledInQueuedMatchmaking() override;
	bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime ) override;
	// Returns true
	bool IsAllyRestrictedVote() override;
	const char *GetDisplayString() override;
	// "#SFUI_otherteam_vote_kick_player"
	const char *GetOtherTeamDisplayString() override;
	// Kicks the connected player whose SteamID is m_steamIDtoBan and bans that SteamID for a
	// convar's time; warns when no SteamID was captured
	void ExecuteCommand() override;
	void ListIssueDetails( CPlayerSlot nForWhom ) override;
	// "#SFUI_vote_passed_kick_player"
	const char *GetVotePassedString() override;
	// VOTEISSUE_KICK
	int GetVoteIssue() override;
	// The slot of the details string when a player is in it
	CPlayerSlot GetTargetPlayerSlot() override;

private:
	// Zeroed by the constructor; no method reads it
	CSteamID m_steamIDVoteCaller;
	// The player to kick, captured when the vote starts
	CSteamID m_steamIDtoBan;
};

//-----------------------------------------------------------------------------
// "LoadBackup": loads a round backup file. 920 bytes on Windows, 12432 on Linux.
//-----------------------------------------------------------------------------
class CLoadBackupIssue : public CBaseCSIssue
{
public:
	~CLoadBackupIssue();
	const char *GetDetailsString() override;
	void OnVoteFailed() override;
	bool IsEnabled() override;
	// Returns true
	bool IsEnabledInQueuedMatchmaking() override;
	bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime ) override;
	// Returns false
	bool IsAllyRestrictedVote() override;
	// The value of a convar
	bool IsVoteCallExclusiveToSpectators() override;
	int GetVotesRequiredToPass() override;
	// "#SFUI_vote_loadbackup"
	const char *GetDisplayString() override;
	void ExecuteCommand() override;
	void ListIssueDetails( CPlayerSlot nForWhom ) override;
	// "#SFUI_vote_passed_loadbackup"
	const char *GetVotePassedString() override;
	// VOTEISSUE_LOADBACKUP
	int GetVoteIssue() override;
	// Returns 1
	float GetFailedVoteLockOutTime() override;

private:
	CUtlVector< const char * > m_arrStrings;
	char m_szPrevDetailsString[ VOTE_ISSUE_MAX_PATH ];
	char m_szNiceName[ VOTE_ISSUE_MAX_PATH ];
};

//-----------------------------------------------------------------------------
// "ChangeLevel": changes the map to the one of the details string. Same size as CBaseIssue.
//-----------------------------------------------------------------------------
class CChangeLevelIssue : public CBaseCSIssue
{
public:
	~CChangeLevelIssue();
	// m_szDetailsString
	const char *GetDetailsString() override;
	bool IsEnabled() override;
	bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime ) override;
	// Returns false
	bool IsAllyRestrictedVote() override;
	// "#SFUI_vote_changelevel"
	const char *GetDisplayString() override;
	void ExecuteCommand() override;
	void ListIssueDetails( CPlayerSlot nForWhom ) override;
	// "#SFUI_vote_passed_changelevel"
	const char *GetVotePassedString() override;
	// VOTEISSUE_CHANGELEVEL
	int GetVoteIssue() override;
};

//-----------------------------------------------------------------------------
// "NextLevel": sets the next map; with several choices it is a vote between them. 400 bytes on
// Windows, 4240 on Linux.
//-----------------------------------------------------------------------------
class CNextLevelIssue : public CBaseCSIssue
{
public:
	~CNextLevelIssue();
	// m_szDetailsString
	const char *GetDetailsString() override;
	// The value of a convar
	bool IsEnabled() override;
	bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime ) override;
	// Returns false
	bool IsAllyRestrictedVote() override;
	const char *GetDisplayString() override;
	void ExecuteCommand() override;
	void ListIssueDetails( CPlayerSlot nForWhom ) override;
	const char *GetVotePassedString() override;
	int GetNumberVoteOptions() override;
	bool IsYesNoVote() override;
	// VOTEISSUE_NEXTLEVEL
	int GetVoteIssue() override;

private:
	// The map choices of a vote between maps
	CUtlVector< const char * > m_IssueOptions;
};

//-----------------------------------------------------------------------------
// "ScrambleTeams": scrambles the teams. Same size as CBaseIssue.
//-----------------------------------------------------------------------------
class CScrambleTeams : public CBaseCSIssue
{
public:
	~CScrambleTeams();
	// The value of a convar
	bool IsEnabled() override;
	bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime ) override;
	// Returns false
	bool IsAllyRestrictedVote() override;
	// "#SFUI_vote_scramble_teams"
	const char *GetDisplayString() override;
	void ExecuteCommand() override;
	void ListIssueDetails( CPlayerSlot nForWhom ) override;
	// "#SFUI_vote_passed_scramble_teams"
	const char *GetVotePassedString() override;
	// VOTEISSUE_SCRAMBLE
	int GetVoteIssue() override;
};

//-----------------------------------------------------------------------------
// "SwapTeams": swaps the teams. Same size as CBaseIssue.
//-----------------------------------------------------------------------------
class CSwapTeams : public CBaseCSIssue
{
public:
	~CSwapTeams();
	// The value of a convar
	bool IsEnabled() override;
	bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime ) override;
	// Returns false
	bool IsAllyRestrictedVote() override;
	// "#SFUI_vote_swap_teams"
	const char *GetDisplayString() override;
	void ExecuteCommand() override;
	void ListIssueDetails( CPlayerSlot nForWhom ) override;
	// "#SFUI_vote_passed_swap_teams"
	const char *GetVotePassedString() override;
	// VOTEISSUE_SWAPTEAMS
	int GetVoteIssue() override;
};

//-----------------------------------------------------------------------------
// "PauseMatch": pauses the match at the next freeze time; one vote passes it. Same size as
// CBaseIssue.
//-----------------------------------------------------------------------------
class CPauseMatchIssue : public CBaseCSIssue
{
public:
	~CPauseMatchIssue();
	bool IsEnabled() override;
	// Returns true
	bool IsEnabledInQueuedMatchmaking() override;
	// Returns true
	bool IsEnabledDuringWarmup() override;
	// Returns 0
	float GetCommandDelay() override;
	// Returns true
	bool ShouldIgnoreCreationTimer() override;
	bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime ) override;
	// The value of a convar
	bool IsVoteCallExclusiveToSpectators() override;
	// Returns 1
	int GetVotesRequiredToPass() override;
	// "#SFUI_vote_pause_match"
	const char *GetDisplayString() override;
	void ExecuteCommand() override;
	void ListIssueDetails( CPlayerSlot nForWhom ) override;
	// "#SFUI_vote_passed_pause_match"
	const char *GetVotePassedString() override;
	// VOTEISSUE_PAUSEMATCH
	int GetVoteIssue() override;
	// Returns 1
	float GetFailedVoteLockOutTime() override;
};

//-----------------------------------------------------------------------------
// "UnpauseMatch": unpauses the match; every voter must agree. Same size as CBaseIssue.
//-----------------------------------------------------------------------------
class CUnpauseMatchIssue : public CBaseCSIssue
{
public:
	~CUnpauseMatchIssue();
	bool IsEnabled() override;
	// Returns true
	bool IsEnabledInQueuedMatchmaking() override;
	// Returns true
	bool IsEnabledDuringWarmup() override;
	// Returns true
	bool ShouldIgnoreCreationTimer() override;
	bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime ) override;
	// Returns true
	bool IsUnanimousVoteToPass() override;
	// Returns true
	bool IsVoteCallExclusiveToSpectators() override;
	// "#SFUI_vote_unpause_match"
	const char *GetDisplayString() override;
	void ExecuteCommand() override;
	void ListIssueDetails( CPlayerSlot nForWhom ) override;
	// "#SFUI_vote_passed_unpause_match"
	const char *GetVotePassedString() override;
	// VOTEISSUE_UNPAUSEMATCH
	int GetVoteIssue() override;
	// Returns 1
	float GetFailedVoteLockOutTime() override;
};

//-----------------------------------------------------------------------------
// "ReadyForMatch": ends the warmup; every voter must agree. Same size as CBaseIssue.
//-----------------------------------------------------------------------------
class CReadyForMatchIssue : public CBaseCSIssue
{
public:
	~CReadyForMatchIssue();
	bool IsEnabled() override;
	// Returns true
	bool IsEnabledInQueuedMatchmaking() override;
	// Returns true
	bool IsEnabledDuringWarmup() override;
	// Returns true
	bool ShouldIgnoreCreationTimer() override;
	bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime ) override;
	// Returns true
	bool IsUnanimousVoteToPass() override;
	// Returns true
	bool IsVoteCallExclusiveToSpectators() override;
	// "#SFUI_vote_ready_for_match"
	const char *GetDisplayString() override;
	void ExecuteCommand() override;
	void ListIssueDetails( CPlayerSlot nForWhom ) override;
	// "#SFUI_vote_passed_ready_for_match"
	const char *GetVotePassedString() override;
	// VOTEISSUE_READYFORMATCH
	int GetVoteIssue() override;
	// Returns 1
	float GetFailedVoteLockOutTime() override;
};

//-----------------------------------------------------------------------------
// "NotReadyForMatch": takes back a ready vote; one vote passes it. Same size as CBaseIssue.
//-----------------------------------------------------------------------------
class CNotReadyForMatchIssue : public CBaseCSIssue
{
public:
	~CNotReadyForMatchIssue();
	bool IsEnabled() override;
	// Returns true
	bool IsEnabledInQueuedMatchmaking() override;
	// Returns true
	bool IsEnabledDuringWarmup() override;
	// Returns 0
	float GetCommandDelay() override;
	// Returns true
	bool ShouldIgnoreCreationTimer() override;
	bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime ) override;
	// Returns 1
	int GetVotesRequiredToPass() override;
	// "#SFUI_vote_not_ready_for_match"
	const char *GetDisplayString() override;
	void ExecuteCommand() override;
	void ListIssueDetails( CPlayerSlot nForWhom ) override;
	// "#SFUI_vote_passed_not_ready_for_match"
	const char *GetVotePassedString() override;
	// VOTEISSUE_NOTREADYFORMATCH
	int GetVoteIssue() override;
	// Returns 1
	float GetFailedVoteLockOutTime() override;
};

//-----------------------------------------------------------------------------
// "StartTimeOut": calls a tactical timeout for the team of the caller. Same size as CBaseIssue.
//-----------------------------------------------------------------------------
class CStartTimeOutIssue : public CBaseCSIssue
{
public:
	~CStartTimeOutIssue();
	bool IsEnabled() override;
	// Returns true
	bool IsEnabledInQueuedMatchmaking() override;
	bool CanTeamCallVote( int iTeam ) const override;
	bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime ) override;
	// Returns true
	bool IsAllyRestrictedVote() override;
	// "#SFUI_vote_start_timeout"
	const char *GetDisplayString() override;
	// "#SFUI_otherteam_vote_timeout"
	const char *GetOtherTeamDisplayString() override;
	void ExecuteCommand() override;
	void ListIssueDetails( CPlayerSlot nForWhom ) override;
	// "#SFUI_vote_passed_timeout"
	const char *GetVotePassedString() override;
	// VOTEISSUE_STARTTIMEOUT
	int GetVoteIssue() override;
	vote_create_failed_t MakeVoteFailErrorCodeForClients( vote_create_failed_t eDefaultFailCode ) override;
};

//-----------------------------------------------------------------------------
// "Surrender": the team of the caller concedes the match. Same size as CBaseIssue.
//-----------------------------------------------------------------------------
class CSurrender : public CBaseCSIssue
{
public:
	~CSurrender();
	// The value of a convar
	bool IsEnabled() override;
	// Returns true
	bool IsEnabledInQueuedMatchmaking() override;
	bool CanTeamCallVote( int iTeam ) const override;
	bool CanCallVote( CPlayerSlot nSlot, const char *pszTypeString, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime ) override;
	// Returns true
	bool IsAllyRestrictedVote() override;
	// Returns false
	bool IsVoteChangeAllowed() override;
	// Returns false
	bool IsUnanimousVoteToPass() override;
	// Returns false
	bool IsSingleVoteToPass() override;
	// "#SFUI_vote_surrender"
	const char *GetDisplayString() override;
	// "#SFUI_otherteam_vote_surrender"
	const char *GetOtherTeamDisplayString() override;
	void ExecuteCommand() override;
	void ListIssueDetails( CPlayerSlot nForWhom ) override;
	const char *GetVotePassedString() override;
	// VOTEISSUE_SURRENDER
	int GetVoteIssue() override;
	// VOTE_FAILED_TOO_EARLY_SURRENDER for VOTE_FAILED_TEAM_CANT_CALL and
	// VOTE_FAILED_WAITINGFORPLAYERS, eDefaultFailCode otherwise
	vote_create_failed_t MakeVoteFailErrorCodeForClients( vote_create_failed_t eDefaultFailCode ) override;
};

#endif // CSTRIKE15_CS_VOTEISSUES_H
