//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Interface the client game module exposes to the engine
//
//===========================================================================//

#ifndef ISOURCE2CLIENT_H
#define ISOURCE2CLIENT_H

#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "entityhandle.h"
#include "entity2/entityindex.h"
#include "splitscreenslot.h"

class CGlobalVarsBase;
class KeyValues;
struct FlattenedSerializerSpewField_t;

abstract_class ISource2Client : public IAppSystem
{
public:
	// NULL restores the module's own globals
	virtual void SetGlobals( CGlobalVarsBase *pGlobals ) = 0;

	// Forwards to the game network string tables
	virtual void unk012( bool bUnk ) = 0;
	virtual void unk013( int nUnk, void *pUnk ) = 0;
	virtual void unk014( int nPlayerIndex ) = 0;

	virtual void CreateMove( int nSlot, bool bActive ) = 0;
	// Forward to the input system
	virtual void unk016() = 0;
	virtual void unk017( void *pUnk ) = 0;

	virtual int unk018( CSplitScreenSlot nSlot ) = 0;
	virtual int unk019( CSplitScreenSlot nSlot ) = 0;
	virtual int unk020( int nUnk ) = 0;
	virtual void unk021() = 0;
	// Forwards to the input system
	virtual void unk022( int nSlot, void *pUnk, bool bUnk ) = 0;
	virtual bool unk023( int nUnk1, void *pUnk, int nUnk2, int nUnk3 ) = 0;
	virtual void unk024( CSplitScreenSlot nSlot, void *pUnk, int nCommandNumber ) = 0;
	virtual void unk025() = 0;
	virtual double unk026( void *pUnk, int nCommandNumber ) = 0;

	// Forward to the game rules
	virtual void unk027( bool bUnk ) = 0;
	virtual void unk028() = 0;
	// Forward to an object owned by the client entity system
	virtual void unk029() = 0;
	virtual void unk030() = 0;
	virtual void unk031( void *pUnk, bool bUnk ) = 0;
	virtual void *unk032() = 0;
	virtual void unk033( void *pUnk1, int *pUnk2, int nUnk ) = 0;
	// Forwards to an object owned by the client entity system
	virtual void unk034() = 0;
	virtual void unk035( void *pUnk1, void *pUnk2 ) = 0;

	virtual void FrameStageNotify( int nStage ) = 0;

	virtual void unk037() = 0;
	virtual void unk038() = 0;
	virtual void unk039() = 0;
	virtual void unk040() = 0;
	virtual void unk041() = 0;
	virtual void HudText( const char *pszMessage ) = 0;
	virtual void unk043() = 0;
	virtual bool unk044() = 0;
	virtual bool unk045() = 0;
	virtual void unk046() = 0;
	virtual void unk047() = 0;

	virtual void VoiceStatus( int nEntityIndex, int nSplitScreenSlot, bool bTalking ) = 0;

	// Forward to the mouth (lip sync) system
	virtual void unk049( int nUnk ) = 0;
	virtual void unk050( uint16 nUnk ) = 0;
	virtual void unk051( void *pUnk, int nUnk1, int nUnk2 ) = 0;
	// Writes two vectors to pOut and passes them to unk098
	virtual void unk052( void *pOut, int nUnk ) = 0;
	virtual void unk053() = 0;
	virtual int unk054( int nSlot, int nUnk, int nButtonCode, void *pUnk ) = 0;
	virtual void unk055( void *pUnk ) = 0;
	virtual void unk056( int nUnk ) = 0;
	virtual void unk057( void *pUnk ) = 0;
	virtual bool unk058() = 0;
	virtual bool unk059( const char *pszCommand ) = 0;
	virtual void unk060() = 0;
	virtual void unk061() = 0;

	// Fires the "demo_start" game event
	virtual void Unk_OnDemoPlaybackStart( void *p ) = 0;
	// Fires the "demo_stop" game event
	virtual void OnDemoPlaybackStop() = 0;
	virtual void unk064( void *pUnk ) = 0;
	// Demo scrubbing data ("SetupScrubbingData")
	virtual void unk065( bool bUnk, int nUnk1, int nUnk2, void *pUnk ) = 0;
	virtual void unk066( bool bUnk1, int nUnk1, int nUnk2, void *pUnk, bool bUnk2, bool bUnk3 ) = 0;
	// Reads the "AnimAssetData" block of pUnk1
	virtual void unk067( void *pUnk1, void *pUnk2 ) = 0;
	virtual void unk068() = 0;
	virtual void unk069( void *pUnk, int nUnk ) = 0;
	// Fires the "demo_skip" game event
	virtual void unk070( int nUnk1, int nUnk2, bool bUnk ) = 0;
	// Forwards to the game configuration
	virtual void unk071() = 0;
	virtual void unk072( void *pUnk, int nUnk ) = 0;
	virtual void unk073() = 0;
	// Dispatches the "UpdateProgressBar" UI event
	virtual bool Unk_UpdateProgressBar( void *p ) = 0;
	virtual bool unk075() = 0;
	virtual bool unk076( int nUnk, void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	// Fires the "keybind_changed" game event
	virtual void Unk_OnKeyBindingChanged( void *p ) = 0;
	virtual bool unk078( void *pUnk, int nUnk, void *pOut ) = 0;
	virtual void unk079( int nUnk, void *pOut ) = 0;
	// Forwards to the input system
	virtual void unk080( int nUnk, void *pOut ) = 0;
	// True when the index has no entity or the entity is dormant
	virtual bool unk081( CEntityIndex nEntityIndex ) = 0;
	// Handle of the first split screen slot's local player pawn
	virtual CEntityHandle unk082() = 0;
	virtual bool unk083( int nUnk ) = 0;

	// Per-entity queries
	virtual bool unk084( CEntityIndex nEntityIndex, void *pOut ) = 0;
	virtual float unk085( CEntityIndex nEntityIndex ) = 0;
	virtual void unk086( CEntityIndex nEntityIndex, void *pOut ) = 0;
	virtual bool unk087( CEntityIndex nEntityIndex, void *pOut1, void *pOut2 ) = 0;
	virtual bool unk088( CEntityIndex nEntityIndex, int *pOut1, int *pOut2 ) = 0;
	virtual CEntityHandle unk089() = 0;
	virtual void unk090( CEntityIndex nEntityIndex, void *pUnk ) = 0;
	virtual bool unk091( CEntityIndex nEntityIndex, void *pOut, int *pUnk ) = 0;
	virtual bool unk092( CEntityIndex nEntityIndex, void *pOut, int *pUnk ) = 0;
	virtual bool unk093( int nUnk ) = 0;
	virtual void unk094( CEntityIndex nEntityIndex, void *pOut1, void *pOut2, void *pOut3 ) = 0;
	virtual bool unk095() = 0;
	virtual bool unk096() = 0;
	virtual CEntityHandle unk097() = 0;
	// Zeroes both outputs
	virtual void unk098( void *pUnk, void *pOut1, void *pOut2 ) = 0;

	virtual bool FormatSerializerFieldValue( CEntityIndex nEntityIndex, FlattenedSerializerSpewField_t &field ) = 0;

	virtual void unk100() = 0;
	virtual void unk101() = 0;
	// Forwards to the game configuration
	virtual void unk102() = 0;
	virtual void unk103( void *pUnk1, void *pUnk2 ) = 0;
	// Calls the deleting destructor of pObject
	virtual void unk104( void *pObject ) = 0;
	virtual void *unk105() = 0;
	virtual void unk106() = 0;
	virtual void unk107( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	// Forwards to the scene system
	virtual void unk108() = 0;
	virtual void unk109( void *pUnk1, void *pUnk2, void *pUnk3, void *pUnk4 ) = 0;
	// Forwards to the scene system
	virtual void unk110() = 0;
	// Physics shape trace
	virtual bool unk111( int nUnk1, int nUnk2, void *pUnk1, void *pUnk2 ) = 0;
	virtual void unk112( int nUnk1, int nUnk2, int nUnk3, void *pUnk ) = 0;
	virtual void unk113( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	virtual void unk114( void *pUnk1, char *pUnk2, void *pUnk3 ) = 0;
	// Looks the entity up and calls unk114
	virtual void unk115( CEntityIndex nEntityIndex, void *pUnk ) = 0;
	virtual void unk116( const char *pszValue ) = 0;
	// Returns its argument
	virtual void *unk117( void *pUnk ) = 0;
	virtual bool unk118() = 0;
	virtual float unk119() = 0;
	virtual void unk120() = 0;
	virtual void unk121() = 0;
	virtual void unk122() = 0;
	virtual bool unk123() = 0;
	virtual void unk124( float flUnk ) = 0;
	virtual void unk125() = 0;
	virtual bool unk126() = 0;
	virtual void unk127( void *pUnk1, void *pUnk2 ) = 0;
	// Forwards to the game event manager
	virtual void unk128( void *pUnk ) = 0;
	virtual void unk129() = 0;
	// Forwards to the game network string tables
	virtual bool unk130() = 0;
	virtual void unk131( int nReason ) = 0;
	virtual void unk132( void *pUnk1, void *pUnk2, int *pUnk3 ) = 0;
	// Save/restore block handling
	virtual void unk133( void *pUnk1, void *pUnk2, int nUnk, void *pUnk3 ) = 0;
	virtual void unk134( void *pUnk1, void *pUnk2, int nUnk, void *pUnk3 ) = 0;
	// Returns "core.global_stack"
	virtual const char *unk135() = 0;
	virtual bool unk136() = 0;
	// Writes NULL and returns 0
	virtual int unk137( void **ppOut ) = 0;
	virtual void unk138( void *pUnk ) = 0;
	virtual bool unk139( const char **ppszUnk, int nUnk, CUtlString *pOut ) = 0;
	virtual void *unk140() = 0;
	// Returns the CToolClientSimulationAPI singleton
	virtual void *GetToolClientSimulationAPI() = 0;
	virtual void unk142() = 0;
	virtual void unk143() = 0;
	virtual bool unk144() = 0;
	virtual void *unk145() = 0;
	virtual void *unk146() = 0;
	virtual void unk147( int nUnk1, int nUnk2, int nUnk3, int nUnk4 ) = 0;
	virtual void unk148() = 0;
	virtual const char *unk149( CEntityIndex nEntityIndex ) = 0;
	virtual CEntityHandle unk150( CEntityHandle hEntity ) = 0;
	virtual void unk151() = 0;
	// Forwards to the game rules
	virtual void unk152() = 0;
	virtual const char *unk153() = 0;
	virtual void unk154( void *pUnk ) = 0;
	virtual void unk155( CEntityIndex nEntityIndex, int *pUnk, int nUnk ) = 0;
	virtual void unk156( CEntityIndex nEntityIndex, int *pUnk ) = 0;

	virtual void *unk157( int nUnk1, int nUnk2, void *pUnk ) = 0;
	virtual void unk158( int nUnk1, int nUnk2, int nUnk3, void *pUnk ) = 0;
	virtual void unk159( int nUnk ) = 0;

	virtual CEntityHandle unk160( int nUnk ) = 0;
	virtual void unk161() = 0;
	virtual void *unk162() = 0;
	virtual bool unk163( void *pUnk ) = 0;
	// HLTV replay state ("OnHltvReplay")
	virtual void Unk_OnHltvReplay( void *p ) = 0;
	virtual void unk165() = 0;
	virtual void unk166() = 0;
	virtual void unk167( void *pMsg ) = 0;
	virtual void unk168() = 0;
	virtual void unk169( void *pUnk1, void *pUnk2 ) = 0;
	virtual void unk170() = 0;
	// Builds a game coordinator protobuf message
	virtual void unk171( void *pUnk ) = 0;
	// Fills matchmaking KeyValues ( "members/machine0/player0" )
	virtual void unk172( KeyValues *pKV, void *pUnk ) = 0;
	virtual CEntityHandle unk173( CEntityIndex nEntityIndex ) = 0;
	// Name of the entity class, "" for an invalid index
	virtual const char *unk174( CEntityIndex nEntityIndex ) = 0;
	virtual bool unk175( CEntityIndex nEntityIndex, void *pOut ) = 0;

	// Forward to the input system
	virtual bool unk176() = 0;
	virtual void unk177() = 0;
	virtual void unk178() = 0;
	virtual float unk179() = 0;
	// Forwards to an object owned by the client entity system
	virtual void unk180() = 0;
	virtual void unk181( void *pUnk, int nUnk ) = 0;

	virtual void unk182( const void *pMsg ) = 0;
	virtual void unk183( const void *pMsg ) = 0;

	// Forward to the input system
	virtual bool unk184() = 0;
	virtual void unk185( void *pUnk, int *pUnk2 ) = 0;
	virtual void unk186() = 0;
	virtual bool unk187( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	virtual void unk188( int nUnk, void *pUnk1, void *pUnk2 ) = 0;

	virtual void unk189( void *pUnk, int nUnk1, int nUnk2 ) = 0;
	virtual void unk190() = 0;
	virtual void unk191() = 0;
	virtual void unk192() = 0;
	virtual void unk193() = 0;
	virtual void unk194() = 0;
	virtual void *unk195() = 0;
	virtual float unk196() = 0;
	virtual float unk197() = 0;
	virtual void unk198( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	virtual bool unk199( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	virtual void unk200() = 0;
	virtual void unk201( void *pUnk1, void *pUnk2 ) = 0;
	virtual bool unk202() = 0;
	virtual void unk203() = 0;
	virtual void *unk204() = 0;
	virtual bool unk205( CEntityIndex nEntityIndex ) = 0;
	virtual CEntityHandle unk206( CEntityIndex nEntityIndex ) = 0;
	virtual CEntityHandle unk207( int nUnk ) = 0;
	virtual void unk208( const char *pszUnk, int *pOut ) = 0;
	virtual void unk209( int nUnk, void *pUnk ) = 0;
};

#endif // ISOURCE2CLIENT_H
