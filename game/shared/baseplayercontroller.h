#ifndef BASEPLAYERCONTROLLER_H
#define BASEPLAYERCONTROLLER_H

#pragma once

#include "../server/baseentity.h"

class CBasePlayerController;
class CCommand;
class CGameCheckTransmitInfo;
class INetworkGameServer;

//-----------------------------------------------------------------------------
// CBasePlayerController adds 36 methods to CBaseEntity: Linux slots 239-274, Windows 238-273.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CBasePlayerControllerVTable : public TBase
{
public:
	// Returns true; CCSPlayerController allocates its services. Called by Connect. Named by what it does. Asked by Connect
	virtual bool CreatePlayerControllerServices() = 0;
	// Empty; CCSPlayerController deletes its services. Called by Disconnect. Named by what it does
	virtual void DestroyPlayerControllerServices() = 0;
	// Calls PreThink of every component of the controller; called by OnSimulateUserCommands before the commands run. Named by what it does
	virtual void PreThink() = 0;
	// Calls PostThink of every component of the controller; called by OnSimulateUserCommands after the commands. Named by what it does
	virtual void PostThink() = 0;
	// Empty in every class
	virtual void Stub_242() = 0;
	// Empty in every class
	virtual void Stub_243() = 0;
	// Empty in every class
	virtual void Stub_244() = 0;
	// Empty in every class
	virtual void Stub_245() = 0;
	// Returns true in every class
	virtual bool Stub_246() = 0;
	// Stores m_iszPlayerName
	virtual void SetPlayerName( const char *pszName ) = 0;
	// Reads the settings of the client: cl_predict, fov_desired, closecaption; CCSPlayerController adds cl_color, cl_autohelp and others
	virtual void InitialSpawn() = 0;
	// Sets the connected state; CCSPlayerController announces the connection to the chat. Called when the client becomes active (CSource2GameClients::ClientActive). Named by what it does
	virtual void OnClientActive() = 0;
	// Empty; CCSPlayerController creates the player and observer pawns when the client is past signon state 4, connected or reconnecting, not HLTV and without a pawn. ISource2Server::OnNetworkGameServerActivated forwards to every player controller
	virtual void OnNetworkGameServerActivated( INetworkGameServer *pNetworkGameServer ) = 0;
	// Applies the network settings of the client: the command queue, cl_interp_ratio; never overridden
	virtual void ClientSettingsChanged() = 0;
	// Fills the transmit info of the client: the engine info, the controller, the ear position, the team, HLTV, low violence, the observer target; called by CheckTransmit.
	virtual void BuildTransmitInfo( CGameCheckTransmitInfo &info, CCheckTransmitInfo *pTransmitInfo ) = 0;
	// What EarPosition of the pawn returns; never overridden. Named by what it does
	virtual Vector GetPawnEarPosition() = 0;
	// What ClientDisconnect keeps of the player: bit 1 the controller, bit 2 the pawn, bit 0 goes to the engine; 0 here, CCSPlayerController keeps both (6 or 7) for a human on a playing team. Named by what the caller does with it. Called when the client disconnects
	virtual uint8 GetDisconnectRetainFlags() = 0;
	// The last step of ClientDisconnect, with the flags of GetDisconnectRetainFlags; CCSPlayerController stores m_LastTimePlayerWasDisconnectedForPawnsRemove or removes its player and observer pawns. Named by its caller. Called when the client disconnects
	virtual void OnClientDisconnected( uint8 nRetainFlags ) = 0;
	// ignoremsg here; vote, jointeam, spectate and others in CCSPlayerController
	virtual bool ClientCommand( const CCommand &args ) = 0;
	// Empty; CCSPlayerController releases a controlled bot on disconnect and keeps m_nDisconnectionTick. Called at the end of SetConnectedState. Named by its caller. Called when the client disconnects
	virtual void OnConnectedStateChanged() = 0;
	// m_bIsHLTV, or the spectator team; CCSPlayerController does not count a coach. Named by what it does
	virtual bool IsSpectator() = 0;
	// True for HLTV and for a controller without a pawn; CCSPlayerController also for a coach, the unassigned team and a dead pawn. Asked together with IsSpectator by the player filters. Named by what it does
	virtual bool IsObserver() = 0;
	// Never overridden
	virtual bool IsConnected() = 0;
	// Empty in every class; called right after the controller stores the user command being run or clears it: in OnSimulateUserCommands, ProcessUsercmds of the movement services and CPlayerCommandQueue. Named by its caller. Called while user commands are simulated
	virtual void OnCurrentCommandChanged() = 0;
	// Clears m_bHasAnySteadyStateEnts and the bit of the player in every entity that has it; never overridden. Named by what it does
	virtual void ClearSteadyStateEntities() = 0;
	// By m_iIgnoreGlobalChat
	virtual bool CanHearAndReadChatFrom( CBasePlayerController *pSender ) = 0;
	// Returns true in every class; Host_Say asks it.
	virtual bool CanSpeak() = 0;
	// Sends CUserMessageAchievementEvent to the player; CCSPlayerController does not while it controls a bot
	virtual void AwardAchievement( int nAchievement ) = 0;
	// Empty in every class; ISource2Server::OnClientHltvReplayStart( slot, n ) forwards to the controller of the slot. Named by the interface method
	virtual void OnClientHltvReplayStart( int nUnknown ) = 0;
	// Empty; CCSPlayerController overrides it. ISource2Server::OnClientHltvReplayStop( slot ) forwards to the controller of the slot. Named by the interface method
	virtual void OnClientHltvReplayStop() = 0;
	// Empty; CCSPlayerController sets, in its entity bit vector, the bits of the entities that must always be transmitted to it (by team). Named by the RTTI of a lambda in the CCSPlayerController override (CCSPlayerController::UpdateAlwaysInPVSEntities); no caller found
	virtual void UpdateAlwaysInPVSEntities() = 0;
	// Stores the time of the change; CCSPlayerController also refreshes the pawn state it networks (m_bPawnIsAlive, m_iPawnHealth and the like). Called at the end of every path of SetPawn. Named by its caller. Called when the controller takes a pawn (SetPawn)
	virtual void OnPawnChanged() = 0;
	// Empty, CCSPlayerController too; called after SetPlayerName stores the name, after the m_steamID setter changed the value and by Spawn when m_steamID is set. Named by its callers. Called by Spawn
	virtual void OnNameOrSteamIDChanged() = 0;
	// Runs the queued user commands; named by its profiler string
	virtual void OnSimulateUserCommands() = 0;
	// Returns true in every class; the achievement_earned client command asks it before it fires the event.
	virtual bool ShouldAnnounceAchievement() = 0;
	// Empty in every class; called after the achievement_earned event is fired.
	virtual void OnAchievementEarned( int nAchievement ) = 0;
};

#endif // BASEPLAYERCONTROLLER_H
