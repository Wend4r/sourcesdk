#ifndef TRIGGERS_H
#define TRIGGERS_H

#pragma once

#include "basetoggle.h"
#include "../shared/imotionevent.h"

class CTakeDamageInfo;

//-----------------------------------------------------------------------------
// CBaseTrigger adds 8 methods to CBaseToggle: Linux slots 272-279, Windows 271-278.
//-----------------------------------------------------------------------------
template < class TBase = CBaseToggleVTable<> >
abstract_class CBaseTriggerVTable : public TBase
{
public:
	// CBaseTrigger::StartTouch and CTriggerHurt::HurtEntity gate on it
	virtual bool PassesTriggerFilters( CBaseEntity *pOther ) = 0;
	virtual void OnStartTouchAll( CBaseEntity *pOther ) = 0;
	// Per-entity hook of the start-touch body, called before m_OnStartTouch fires (CBuyZone enters the buy zone, CTriggerHurt starts HurtThink, CTriggerPush, CTriggerTeleport ... act here). Named by what it does
	virtual void OnStartTouch( CBaseEntity *pOther ) = 0;
	// Per-entity hook of the end-touch body, called before m_OnEndTouch fires (CBuyZone leaves the buy zone ...). Named by what it does
	virtual void OnEndTouch( CBaseEntity *pOther ) = 0;
	virtual void OnEndTouchAll( CBaseEntity *pOther ) = 0;
	// Activate and the Enable / Disable inputs call it with !m_bDisabled after writing m_bDisabled; switches touching on / off on the async query helper or the collision property. Named by what it does
	virtual void SetTriggerEnabled( bool bEnabled ) = 0;
	// Empty in every class; no caller on the server
	virtual void Stub_277() = 0;
	// Sets m_bUseAsyncQueries and creates the async touch query helper (Activate calls it when m_bUseAsyncQueries is set). Named by what it does
	virtual void EnableAsyncQueries() = 0;
};

//-----------------------------------------------------------------------------
// CTriggerHurt adds 2 methods to CBaseTrigger: Linux slots 280-281, Windows 279-280.
//-----------------------------------------------------------------------------
template < class TBase = CBaseTriggerVTable<> >
abstract_class CTriggerHurtVTable : public TBase
{
public:
	// Empty hook the hurt routine calls with the freshly built CTakeDamageInfo before applying it. Named by what it does
	virtual void ModifyHurtDamageInfo( CTakeDamageInfo &info ) = 0;
	virtual bool HurtEntity( CBaseEntity *pOther, float flDamage ) = 0;
};

//-----------------------------------------------------------------------------
// CTriggerPhysics adds no method to CBaseTrigger. The Linux table has 2 more slots, 280-281: overrides of methods of IMotionEvent, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseTriggerVTable<> >
abstract_class CTriggerPhysicsVTable : public TBase, public IMotionEvent
{
public:
	// Updates the motion controller unless m_bDisabled. Overrides IMotionEvent: a slot of this table on Linux only
	virtual void Unk_0() override = 0;
	// Computes the forces the trigger applies to an object (the one large override of the interface). Overrides IMotionEvent: a slot of this table on Linux only
	virtual void Simulate( void *pUnknown, IPhysicsBody *pBody, float flDeltaTime ) override = 0;
};

#endif // TRIGGERS_H
