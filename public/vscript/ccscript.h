#ifndef CCSCRIPT_H
#define CCSCRIPT_H

#include "entitysystem.h"
#include "igameevents.h"
#include "tier0/generichash.h"
#include "tier0/globalsymbol.h"
#include "tier0/utlstring.h"
#include "tier1/utlhashtable.h"
#include "tier1/utlvector.h"

class CBaseEntity;
class CCSPlayerController;
class CCSPlayerPawn;
class CCSWeaponBase;
class CPlantedC4;
class CTakeDamageInfo;
class CTakeDamageResult;

namespace v8
{
	class Function;
	class FunctionTemplate;
	class Object;
	template < class T > class Global;
}

// The weapon attack OnKnifeAttack reports; the script sees it as CSWeaponAttackType.
enum class WeaponAttackType_t : int32
{
	eInvalid = -1,
	ePrimary = 0,
	eSecondary = 1,

	eCount = 2,
};

// Why a round ended, as OnRoundEnd reports it; the script sees the CSRoundEndReason enumerator of
// the same name.
enum RoundEndReason_t
{
	ROUND_END_REASON_UNKNOWN = -1,
	ROUND_END_REASON_IN_PROGRESS = 0,
	ROUND_END_REASON_TARGET_BOMBED = 1,
	ROUND_END_REASON_BOMB_DEFUSED = 7,
	ROUND_END_REASON_CTS_WIN = 8,
	ROUND_END_REASON_TERRORISTS_WIN = 9,
	ROUND_END_REASON_DRAW = 10,
	ROUND_END_REASON_HOSTAGES_RESCUED = 11,
	ROUND_END_REASON_TARGET_SAVED = 12,
	ROUND_END_REASON_HOSTAGES_NOT_RESCUED = 13,
	ROUND_END_REASON_GAME_COMMENCING = 16,
	ROUND_END_REASON_TERRORISTS_SURRENDER = 17,
	ROUND_END_REASON_CTS_SURRENDER = 18,
};

// Hashes a CGlobalSymbol by the 32-bit hash stored in front of its string
struct CGlobalSymbolHashFunctor
{
	unsigned int operator()( const CGlobalSymbol &sym ) const { return sym.IsValid() ? reinterpret_cast< const uint32 * >( sym.String() )[ -1 ] : 0; }
};

// Hashes an entity handle stored as an integer
struct CCSScriptEntityHandleHashFunctor
{
	unsigned int operator()( int nHandle ) const { return MurmurHash2( &nHandle, sizeof( nHandle ), 0x3501A674 ); }
};

template < typename T >
struct CUtlAutoList
{
	virtual ~CUtlAutoList() = default;

	T *m_pPrev;
	T *m_pNext;
};

// cs_script instance; the Instance.On* callbacks of the script are called through the On* methods
class CCSBaseScript : public IEntityListener, public CUtlAutoList< CCSBaseScript >
{
public:
	virtual ~CCSBaseScript() {}

	virtual const char *GetName() = 0;
	virtual void CreateEntityTemplate( /* v8::Local< v8::FunctionTemplate > */ void *pTemplate ) = 0;

	// Clears m_Callbacks; the entity script also stops listening for game events
	virtual void ClearCallbacks() = 0;

	virtual void OnBeginRoundRestart() = 0;
	virtual void OnPlayerReset( CCSPlayerPawn *pPlayer ) = 0;
	virtual void OnPlayerTeamChanged( CCSPlayerPawn *pPlayer, int nOldTeam ) = 0;
	virtual void OnRoundStart() = 0;
	virtual void OnRoundEnd( int nWinningTeam, RoundEndReason_t nReason ) = 0;
	virtual void OnBombPlantStart( CCSPlayerPawn *pPlanter ) = 0;
	virtual void OnBombPlantAbort( CCSPlayerPawn *pPlanter ) = 0;
	virtual void OnBombPlant( CPlantedC4 *pPlantedC4, CCSPlayerPawn *pPlanter ) = 0;
	virtual void OnBombDefuseStart( CPlantedC4 *pPlantedC4 ) = 0;
	virtual void OnBombDefuseAbort( CPlantedC4 *pPlantedC4 ) = 0;
	virtual void OnBombDefuse( CPlantedC4 *pPlantedC4, CCSPlayerPawn *pDefuser ) = 0;
	virtual void OnBombExplode( CPlantedC4 *pPlantedC4 ) = 0;
	virtual void OnHostagePickUpStart( CEntityInstance *pHostage ) = 0;
	virtual void OnHostagePickUpAbort( CEntityInstance *pHostage, CEntityInstance *pPlayer ) = 0;
	virtual void OnHostagePickUp( CEntityInstance *pHostage ) = 0;
	virtual void OnHostageDrop( CEntityInstance *pHostage, CEntityInstance *pDropper ) = 0;
	virtual void OnHostageRescue( CEntityInstance *pHostage ) = 0;
	virtual void OnHostageKill( CEntityInstance *pHostage, CEntityInstance *pAttacker ) = 0;

	// The script may change the damage, its types and flags, or abort it
	virtual int OnModifyPlayerDamage( CCSPlayerPawn *pPlayer, CTakeDamageInfo &info ) = 0;
	virtual void OnPlayerDamage( CCSPlayerPawn *pPlayer, const CTakeDamageResult &result ) = 0;
	virtual void OnPlayerKill( CCSPlayerPawn *pPlayer, const CTakeDamageResult &result ) = 0;

	virtual void OnPlayerJump( CCSPlayerPawn *pPlayer ) = 0;
	virtual void OnPlayerLand( CCSPlayerPawn *pPlayer ) = 0;
	virtual void OnPlayerChat( CCSPlayerController *pPlayer, const char *pszText, int nTeam ) = 0;
	virtual void OnPlayerPing( CCSPlayerController *pPlayer, const Vector &vecPosition ) = 0;
	virtual void OnGunFire( CCSWeaponBase *pWeapon ) = 0;
	virtual void OnGunReload( CCSWeaponBase *pWeapon ) = 0;
	virtual void OnBulletImpact( CCSWeaponBase *pWeapon, const Vector &vecPosition, CBaseEntity *pHitEntity ) = 0;
	virtual void OnWeaponDrop( CCSWeaponBase *pWeapon, CCSPlayerPawn *pDropper ) = 0;
	virtual void OnWeaponPickup( CCSWeaponBase *pWeapon ) = 0;
	virtual void OnGrenadeThrow( CCSWeaponBase *pWeapon, CBaseEntity *pProjectile ) = 0;
	virtual void OnGrenadeBounce( CBaseEntity *pProjectile, CBaseEntity *pHitEntity, Vector vecNormal, int nBounces ) = 0;
	virtual void OnKnifeAttack( CCSWeaponBase *pWeapon, WeaponAttackType_t nAttackType ) = 0;
	virtual void OnCustomHudClicked( CCSPlayerController *pPlayer, CBaseEntity *pLayout, const void *pButtonId ) = 0;

	virtual int OnBeforePlayerDamage( CCSPlayerPawn *pPlayer, CTakeDamageInfo &info ) = 0;

	// Index of the instance in creation order
	int m_nInstanceIndex;

	// v8::Global< v8::Context > of the script
	void *m_pContext;

	// Set once the script imports its "cs_script/<name>" module; checked before any callback is called
	bool m_bModuleImported;

	// Export names of that module, each looked up in m_SingletonTemplates, m_ClassTemplates and then m_EnumObjects
	CUtlVector< CUtlString > m_ModuleExportNames;

	// Templates of the singleton objects ("Instance", "Console", "GameRules"), exported as a new instance of each
	CUtlHashtable< CGlobalSymbol, v8::Global< v8::FunctionTemplate > *, CGlobalSymbolHashFunctor > m_SingletonTemplates;

	// Function templates by class name ("Entity", "BaseModelEntity", "CSWeaponBase", ...)
	CUtlHashtable< CGlobalSymbol, v8::Global< v8::FunctionTemplate > *, CGlobalSymbolHashFunctor > m_ClassTemplates;

	// Objects holding the values of each registered enumeration, by its name
	CUtlHashtable< CGlobalSymbol, v8::Global< v8::Object > *, CGlobalSymbolHashFunctor > m_EnumObjects;

	// Callbacks of the script, by name ("Registering callback", "Invoking callback", "Remove callback")
	CUtlHashtable< CGlobalSymbol, v8::Global< v8::Function > *, CGlobalSymbolHashFunctor > m_Callbacks;

	// Objects wrapping each entity, by its handle as an integer
	CUtlHashtable< int, v8::Global< v8::Object > *, CCSScriptEntityHandleHashFunctor > m_EntityObjects;

	// Objects wrapping each "CSWeaponData", by its id
	CUtlHashtable< int, v8::Global< v8::Object > *, IdentityHashFunctor > m_WeaponDataObjects;

	// "CSPrecache" objects returned by Instance.Precache, by their id
	CUtlHashtable< int, v8::Global< v8::Object > * > m_PrecacheObjects;
};

class CCSScript_EntityScript : public CCSBaseScript, public IGameEventListener2
{
public:
	// Itanium also lists this override in the primary vtable, MSVC only in the listener's one
	virtual void FireGameEvent( IGameEvent *pEvent ) override = 0;
};

#endif // CCSCRIPT_H
