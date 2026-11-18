#ifndef CCSCRIPT_H
#define CCSCRIPT_H

#include "entitysystem.h"
#include "igameevents.h"
#include "mathlib/vector.h"
#include "tier0/globalsymbol.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"

class CTakeDamageInfo;
struct CTakeDamageResult;
struct EntityIOConnection_t;

namespace v8
{
	class Context;
	class Function;
	class FunctionTemplate;
	class Object;
	class Value;

	template < class T >
	class Global;
}

// Intrusive list of every live instance of T, newest last
template< typename T >
struct CUtlAutoList
{
	virtual ~CUtlAutoList() = default;

	T *m_pNext;
	T *m_pPrev;
};

// A game event listener that removes itself from the game event manager when it is destroyed
class CGameEventListener : public IGameEventListener2
{
public:
	virtual ~CGameEventListener() {}

	// Set while the listener is registered for at least one event
	bool m_bRegisteredForEvents;
};

// A cs_script (JavaScript) instance with its own v8 context
class CCSBaseScript : public IEntityListener, public CUtlAutoList< CCSBaseScript >
{
public:
	virtual ~CCSBaseScript() = default;

	// "cs_script" for the base, "point_script" for the entity script
	virtual const char *GetName() = 0;

	// Registers the script API (Domain, Instance, Entity and others) in the script context
	virtual void RegisterBindings() = 0;
	// Called before a script is loaded: forgets the callbacks and output connections the previous script made
	virtual void ClearCallbacks() = 0;

	// Named by what it does: the entity the script runs on, the target of its output connections. Null for the base
	virtual CEntityInstance *GetOwnerEntity() = 0;

	virtual void OnBeginRoundRestart() = 0;
	virtual void OnPlayerReset( CEntityInstance *pPlayer ) = 0;
	virtual void OnPlayerTeamChanged( CEntityInstance *pPlayer, int nOldTeam ) = 0;
	virtual void OnBotControl( CEntityInstance *pController ) = 0;
	virtual void OnRoundStart() = 0;
	virtual void OnRoundEnd( int nWinningTeam, int nReason ) = 0;
	virtual void OnBombPlantStart( CEntityInstance *pPlanter ) = 0;
	virtual void OnBombPlantAbort( CEntityInstance *pPlanter ) = 0;
	virtual void OnBombPlant( CEntityInstance *pPlantedC4, CEntityInstance *pPlanter ) = 0;
	virtual void OnBombDefuseStart( CEntityInstance *pPlantedC4 ) = 0;
	virtual void OnBombDefuseAbort( CEntityInstance *pPlantedC4 ) = 0;
	virtual void OnBombDefuse( CEntityInstance *pPlantedC4, CEntityInstance *pDefuser ) = 0;
	virtual void OnBombExplode( CEntityInstance *pPlantedC4 ) = 0;
	virtual void OnHostagePickUpStart( CEntityInstance *pHostage ) = 0;
	virtual void OnHostagePickUpAbort( CEntityInstance *pHostage, CEntityInstance *pPlayer ) = 0;
	virtual void OnHostagePickUp( CEntityInstance *pHostage ) = 0;
	virtual void OnHostageDrop( CEntityInstance *pHostage, CEntityInstance *pDropper ) = 0;
	virtual void OnHostageRescue( CEntityInstance *pHostage ) = 0;
	virtual void OnHostageKill( CEntityInstance *pHostage, CEntityInstance *pAttacker ) = 0;
	virtual uint32 OnModifyPlayerDamage( CEntityInstance *pPlayer, CTakeDamageInfo *pInfo ) = 0;
	virtual void OnPlayerDamage( CEntityInstance *pPlayer, const CTakeDamageResult *pResult ) = 0;
	virtual void OnPlayerKill( CEntityInstance *pPlayer, const CTakeDamageResult *pResult ) = 0;
	virtual void OnPlayerJump( CEntityInstance *pPlayer ) = 0;
	virtual void OnPlayerLand( CEntityInstance *pPlayer ) = 0;
	virtual void OnPlayerChat( CEntityInstance *pPlayer, const char *pszText, int nTeam ) = 0;
	virtual void OnPlayerPing( CEntityInstance *pPlayer, const Vector &vecPosition ) = 0;
	virtual void OnGunFire( CEntityInstance *pWeapon ) = 0;
	virtual void OnGunReload( CEntityInstance *pWeapon ) = 0;
	virtual void OnBulletImpact( CEntityInstance *pWeapon, const Vector &vecPosition, CEntityInstance *pHitEntity ) = 0;
	virtual void OnWeaponDrop( CEntityInstance *pWeapon, CEntityInstance *pDropper ) = 0;
	virtual void OnWeaponPickup( CEntityInstance *pWeapon ) = 0;
	virtual void OnGrenadeThrow( CEntityInstance *pWeapon, CEntityInstance *pProjectile ) = 0;
	virtual void OnGrenadeBounce( CEntityInstance *pProjectile, CEntityInstance *pHitEntity, Vector vecNormal, int nBounces ) = 0;
	virtual void OnKnifeAttack( CEntityInstance *pWeapon, int nAttackType ) = 0;
	virtual void OnCustomHudClicked( CEntityInstance *pPlayer, CEntityInstance *pLayout, const CUtlString &sButtonId ) = 0;
	virtual uint32 OnBeforePlayerDamage( CEntityInstance *pPlayer, CTakeDamageInfo *pInfo ) = 0;

public:
	// An output of an entity connected to a script callback by Instance.ConnectOutput
	struct OutputConnection_t
	{
		CEntityHandle m_hEntity;
		EntityIOConnection_t *m_pConnection;
	};

	// Taken from a global counter when the script is constructed
	int m_nId;

	void *m_pContext; // v8::Global< v8::Context >

	// Named by what it does: set when loading the script registered at least one export, the events are dispatched only while it is set
	bool m_bLoaded;

	// Named by what it does: the names of m_Exports, in registration order
	CUtlVector< CUtlString > m_ExportNames;

	// Named by what it does: the objects RegisterBindings exposes to the script, such as "Instance", by name
	CUtlHashtable< CGlobalSymbol, v8::Global< v8::Object > * > m_Exports;

	// Named by what it does: the template of every script class ("Entity", "CSPlayerPawn" and others), by class name
	CUtlHashtable< CGlobalSymbol, v8::Global< v8::FunctionTemplate > * > m_ClassTemplates;

	// Named by what it does: the objects that hold the values of the registered enums, by enum name
	CUtlHashtable< CGlobalSymbol, v8::Global< v8::Object > * > m_EnumObjects;

	// The script functions by callback name: the event names ("OnRoundStart"), "Think", "ScriptInput:<name>" and "OutputConnection:<id>"
	CUtlHashtable< CGlobalSymbol, v8::Global< v8::Function > * > m_Callbacks;

	// Named by what it does: the script object of each entity the script has seen; the entry goes when the entity is deleted
	CUtlHashtable< CEntityHandle, v8::Global< v8::Object > * > m_EntityObjects;

	// Named by what it does: the script objects of the weapon data (CSWeaponData), by the id the weapon data has in its vdata
	CUtlHashtable< uint32, v8::Global< v8::Object > * > m_WeaponDataObjects;

	// Named by what it does: the script objects Instance.Precache returns, by resource id
	CUtlHashtable< uint32, v8::Global< v8::Object > * > m_PrecacheObjects;

	// Named by what it does: the id the next output connection gets
	int m_nNextOutputConnectionId;

	// Named by what it does: the output connections made by Instance.ConnectOutput, by connection id
	CUtlHashtable< int, OutputConnection_t > m_OutputConnections;
};

// The script of a point_script entity
class CCSScript_EntityScript : public CCSBaseScript, public CGameEventListener
{
public:
	typedef void ( *GameEventHandler_t )( CCSScript_EntityScript *pScript, IGameEvent *pEvent );

	// A promise returned by Instance.Delay, resolved when the owner entity thinks after m_flResumeTime
	struct PendingDelay_t
	{
		GameTime_t m_flResumeTime;
		void *m_pResolver; // v8::Global< v8::Promise::Resolver > *
	};

	void FireGameEvent( IGameEvent *pEvent ) override;

public:
	// The point_script entity
	CEntityHandle m_hOwnerEntity;

	// Named by what it does: the promises of Instance.Delay that are not resolved yet
	CUtlVector< PendingDelay_t > m_PendingDelays;

	// Named by what it does: the handler of each game event the script listens to, by event id
	CUtlHashtable< int, GameEventHandler_t > m_GameEventHandlers;
};

#endif // CCSCRIPT_H
