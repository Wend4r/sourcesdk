#ifndef CCSCRIPT_H
#define CCSCRIPT_H

#include "entitysystem.h"
#include "igameevents.h"
#include "mathlib/vector.h"
#include "tier0/utlstring.h"

class CTakeDamageInfo;
struct CTakeDamageResult;

template< typename T >
struct CUtlAutoList
{
	virtual ~CUtlAutoList() = default;

	T *m_pNext;
	T *m_pPrev;
};

class CCSScript : public IEntityListener, public CUtlAutoList< CCSScript >
{
public:
	virtual ~CCSScript() = default;

	virtual const char *GetName() = 0;

	// Registers the script API (Domain, Instance, Entity and others) in the script context
	virtual void RegisterBindings() = 0;
	virtual void ClearCallbacks() = 0;

	virtual void OnBeginRoundRestart() = 0;
	virtual void OnPlayerReset( CEntityInstance *pPlayer ) = 0;
	virtual void OnPlayerTeamChanged( CEntityInstance *pPlayer, int nOldTeam ) = 0;
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

	int m_nId;
	void *m_pContext; // v8::Global< v8::Context >
	bool m_bUnk030;
	uint8 m_Unk031[ 0x130 - 0x31 ];
};

class CCSScript_EntityScript : public CCSScript, public IGameEventListener2
{
public:
	void FireGameEvent( IGameEvent *pEvent ) override;
};

#endif // CCSCRIPT_H
