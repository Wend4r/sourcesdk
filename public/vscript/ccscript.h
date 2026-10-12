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

    /* 0x10 */ uint8_t    pad[16]; /* padding / unknown (0x10..0x1F) */
    /* 0x20 */ int32_t    indexOrId;      // written: *(_DWORD *)(a1 + 32) = v6;
                                         // v6 == dword_1817075D0 (incremented). Likely an instance id / index.
    /* 0x24 */ int32_t    pad_after_id;
    /* 0x28 */ /*v8::Global<v8::Context>*/ void* v8_api_global;  // written as a qword, holds v8::api_internal* or 0; GlobalizeReference placed here
    /* 0x30 */ uint16_t   unknown_word_48; // *(_WORD *)(a1 + 48) = 0;
    /* 0x32 */ uint8_t    padding_48[6];  // alignment to 0x38
    /* 0x38 */ void*      unknown_qword_56; // *(_QWORD *)(a1 + 56) = 0LL;
    /* 0x40 */ void*      unknown_qword_64; // *(_QWORD *)(a1 + 64) = 0LL;
    //
    // Four repeated "blocks" follow — pattern: three qwords + dword + byte.
    // Each block initialized mostly to zeros, with a DWORD set to 32 and a byte set to 0.
    //
    /* 0x48 */ void*      block0_ptr0;    // *(_QWORD *)(a1 + 72) = 0LL;   (0x48)
    /* 0x50 */ uint32_t   block0_count;   // *(_DWORD *)(a1 + 80) = 32;    (0x50)
    /* 0x54 */ uint8_t    block0_flag;    // *(_BYTE  *)(a1 + 84) = 0;
    /* 0x55 */ uint8_t    pad_b0[3];      // pad to 0x58
    /* 0x58 */ void*      block0_ptr1;    // *(_QWORD *)(a1 + 88) = 0LL;
    /* 0x60 */ void*      block0_ptr2;    // *(_QWORD *)(a1 + 96) = 0LL;

    /* 0x68 */ void*      block1_ptr0;    // *(_QWORD *)(a1 + 104) = 0LL;
    /* 0x70 */ uint32_t   block1_count;   // *(_DWORD *)(a1 + 112) = 32;
    /* 0x74 */ uint8_t    block1_flag;    // *(_BYTE  *)(a1 + 116) = 0;
    /* 0x75 */ uint8_t    pad_b1[3];      // pad to 0x78
    /* 0x78 */ void*      block1_ptr1;    // *(_QWORD *)(a1 + 120) = 0LL;
    /* 0x80 */ void*      block1_ptr2;    // *(_QWORD *)(a1 + 128) = 0LL;

    /* 0x88 */ void*      block2_ptr0;    // *(_QWORD *)(a1 + 136) = 0LL;
    /* 0x90 */ uint32_t   block2_count;   // *(_DWORD *)(a1 + 144) = 32;
    /* 0x94 */ uint8_t    block2_flag;    // *(_BYTE  *)(a1 + 148) = 0;
    /* 0x95 */ uint8_t    pad_b2[3];      // pad to 0x98
    /* 0x98 */ void*      block2_ptr1;    // *(_QWORD *)(a1 + 152) = 0LL;
    /* 0xA0 */ void*      block2_ptr2;    // *(_QWORD *)(a1 + 160) = 0LL;

    /* 0xA8 */ void*      block3_ptr0;    // *(_QWORD *)(a1 + 168) = 0LL;
    /* 0xB0 */ uint32_t   block3_count;   // *(_DWORD *)(a1 + 176) = 32;
    /* 0xB4 */ uint8_t    block3_flag;    // *(_BYTE  *)(a1 + 180) = 0;
    /* 0xB5 */ uint8_t    pad_b3[3];      // pad to 0xB8
    /* 0xB8 */ /* more fields may follow... */

};

class CCSScript_EntityScript : public CCSBaseScript, public IGameEventListener2
{
public:
	// Calls the script's handler for the event, as FireGameEvent does
	virtual void unk039( IGameEvent *pEvent ) = 0;
};

#endif // CCSCRIPT_H
