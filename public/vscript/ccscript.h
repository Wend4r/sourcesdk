#ifndef CCSCRIPT_H
#define CCSCRIPT_H

#include "entitysystem.h"
#include "igameevents.h"
#include "tier0/globalsymbol.h"

class CBaseEntity;
class CCSPlayerController;
class CCSPlayerPawn;
class CCSWeaponBase;
class CPlantedC4;
class CTakeDamageInfo;
class CTakeDamageResult;

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
	virtual CGlobalSymbol GetGlobalSymbol() = 0;
	virtual void CreateEntityTemplate( /* v8::Local< v8::FunctionTemplate > */ void *pTemplate ) = 0;

	// Drops the callbacks registered by the script
	virtual void unk009() = 0;

	virtual void OnBeginRoundRestart() = 0;
	virtual void OnPlayerReset( CCSPlayerPawn *pPlayer ) = 0;
	virtual void OnPlayerTeamChanged( CCSPlayerPawn *pPlayer, int nOldTeam ) = 0;
	virtual void OnRoundStart() = 0;
	virtual void OnRoundEnd( int nWinningTeam, int nReason ) = 0;
	virtual void OnBombPlantStart( CCSPlayerPawn *pPlanter ) = 0;
	virtual void OnBombPlantAbort( CCSPlayerPawn *pPlanter ) = 0;
	virtual void OnBombPlant( CPlantedC4 *pPlantedC4, CCSPlayerPawn *pPlanter ) = 0;
	virtual void OnBombDefuseStart( CPlantedC4 *pPlantedC4 ) = 0;
	virtual void OnBombDefuseAbort( CPlantedC4 *pPlantedC4 ) = 0;
	virtual void OnBombDefuse( CPlantedC4 *pPlantedC4, CCSPlayerPawn *pDefuser ) = 0;
	virtual void OnBombExplode( CPlantedC4 *pPlantedC4 ) = 0;

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
	virtual void OnKnifeAttack( CCSWeaponBase *pWeapon, int nAttackType ) = 0;
	virtual void OnCustomHudClicked( CCSPlayerController *pPlayer, CBaseEntity *pLayout, const void *pButtonId ) = 0;

	virtual int OnBeforePlayerDamage( CCSPlayerPawn *pPlayer, CTakeDamageInfo &info ) = 0;

	// Index of the instance in creation order
	int m_nUnk020;

	// v8::Global< v8::Context > of the script
	void *m_pContext;

	// Checked before any callback is called
	bool m_bUnk030;

	uint64 m_Unk038[ 3 ];

	// Hash tables with 0x18-byte entries; m_Unk090 holds v8 global handles
	uint64 m_Unk050[ 4 ];
	uint64 m_Unk070[ 4 ];
	uint64 m_Unk090[ 4 ];
	uint64 m_Unk0B0[ 4 ];
	uint64 m_Unk0D0[ 4 ];
};

class CCSScript_EntityScript : public CCSBaseScript, public IGameEventListener2
{
public:
	// Calls the script's handler for the event, as FireGameEvent does
	virtual void unk039( IGameEvent *pEvent ) = 0;
};

#endif // CCSCRIPT_H
