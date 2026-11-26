//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Pose tasks: the work a pose node of a graph registers with the
//          task system instead of computing a pose itself
//
//===========================================================================//

#ifndef ANIMTASK_H
#define ANIMTASK_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/bufferstring.h"
#include "schemasystem/schematypes.h"
#include "color.h"

class CNmTaskContext;
class CNmTaskSerializer;

//-----------------------------------------------------------------------------
// CNmPoseTask, the base of every pose task (sample, blends, IK, cached pose read
// and write and so on): its virtual methods only, the fields are not known. The
// tasks are created by the task system, so pointers to them are only used to call
// these methods. The game's task classes only override them.
//-----------------------------------------------------------------------------
abstract_class CNmPoseTaskVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;

	virtual ~CNmPoseTaskVTable() {}

	// Runs the task
	virtual void Execute( const CNmTaskContext &context ) = 0;
	// Returns false in every task and is never overridden
	virtual bool Unk_3() const = 0;
	// Named by what it does: 0 for the tasks that produce a pose by themselves (sample,
	// reference, zero, cached pose read), 1 for tasks with one input, 2 for blends
	virtual int GetDependencyCount() const = 0;

	// Write and read the state of the task bit by bit, for recorded graph states
	virtual void Serialize( CNmTaskSerializer &serializer ) const = 0;
	virtual void Deserialize( CNmTaskSerializer &serializer ) = 0;

	// Such as "Sample", "Blend" or "Foot IK"
	virtual const char *GetDebugName() const = 0;
	// Empty in the base
	virtual CBufferStringN< 256 > GetDebugText() const = 0;
	// White in the base, DrawDebug draws with it
	virtual Color GetDebugColor() const = 0;
	// 0 in the base; the time of a sample task, the weight of a blend
	virtual float GetDebugProgressOrWeight() const = 0;
	// The type of the draw context is not known
	virtual void DrawDebug( void *pDrawContext ) const = 0;
};

#endif // ANIMTASK_H
