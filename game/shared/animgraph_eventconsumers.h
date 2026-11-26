//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Event consumers: turn the events an AnimGraph2 graph sampled in an
//          update into sounds, particles, attribute changes and so on
//
//===========================================================================//

#ifndef ANIMGRAPH_EVENTCONSUMERS_H
#define ANIMGRAPH_EVENTCONSUMERS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

class CBaseEntity;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

//-----------------------------------------------------------------------------
// CNmEventConsumer, the base of the consumers of sampled graph events
// (CNmEventConsumerSound, CNmEventConsumerParticle, CNmEventConsumerAttributes,
// CNmEventConsumerBodyGroup, CNmEventConsumerCamera, CNmEventConsumerCloth,
// CNmEventConsumerContact, CNmEventConsumerLegacy, CNmEventConsumerPulse): its
// virtual methods only, the fields are not known. The consumers only override
// these methods.
//
// After every update of the graph each consumer gets PreUpdate, then StartEvent for
// every relevant new event it accepts (ShouldConsumeEvent), UpdateEvent for the
// lasting events still running, EndEvent for the lasting events that ended, and
// PostUpdate at the end. A consumer keeps a lasting event as a 40-byte record: the
// sampled event, the target entity handle, the event, and two handles the consumer
// owns (the sound or the particle it started). The methods from PreUpdate on are
// named by what they do.
//-----------------------------------------------------------------------------
abstract_class CNmEventConsumerVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	// Name of the most derived consumer class
	virtual const char *GetKV3TransferClassName() const = 0;

	virtual ~CNmEventConsumerVTable() {}

	// Called on every consumer before any event is dispatched; the particle consumer drops
	// the particles that finished
	virtual void PreUpdate() = 0;
	// pActiveEvent is the record of a lasting event, null for an instant one; the sound
	// consumer starts the sound here
	virtual void StartEvent( const void *pSampledEvent, CBaseEntity *pTargetEntity, void *pActiveEvent ) = 0;
	// Only the attribute consumer implements it
	virtual void UpdateEvent( const void *pSampledEvent, void *pActiveEvent ) = 0;
	// Also called when an event restarts; the sound consumer stops the sound
	virtual void EndEvent( const void *pSampledEvent, void *pActiveEvent ) = 0;
	// Only the legacy consumer implements it
	virtual void PostUpdate( void *pGraphOwner ) = 0;
	// The sound consumer stops every sound it started, the particle consumer destroys
	// every particle it started
	virtual void StopAllEvents() = 0;
	// Filter before StartEvent: whether the event is of the type the consumer handles
	virtual bool ShouldConsumeEvent( const void *pSampledEvent ) const = 0;
};

#endif // ANIMGRAPH_EVENTCONSUMERS_H
