//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The navigation space (the volume tree of the fly space) and its game version.
//
//=============================================================================//

#ifndef NAV_SPACE_H
#define NAV_SPACE_H
#ifdef _WIN32
#pragma once
#endif

#include <functional>

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlhashtable.h"
#include "mathlib/aabb.h"

class CNavBlock;
class CJob;
class IThreadPool;
class IVPhysics2World;

//-----------------------------------------------------------------------------
// The volume tree of the fly space: cells with the blocks inside them, built and updated by
// background jobs. CNavSpaceGame derives from it, adds no virtual method and a hash table of
// its own. 264 bytes on Linux, 296 on Windows (the std::function member differs in size).
//-----------------------------------------------------------------------------
abstract_class CNavSpace
{
public:
	// One cell of the space. 64 bytes
	struct SpaceCell_t
	{
		AABB_t m_Bounds;
		// The data of the cell in the loaded space (an 80-byte object whose type is not
		// reconstructed); the cell deletes it when m_bOwnsData is set
		void *m_pData;
		bool m_bOwnsData;
		// The blocks inside the cell
		CUtlVector< CNavBlock * > m_Blocks;
	};

	// The job queue of a build: at most m_nMaxRunningJobs jobs run in the thread pool, the rest wait
	// (the tree build jobs, CNavSpaceBuildTreeJob). 96 bytes on Linux, 128 on Windows (the
	// std::function member differs in size). Named by what it holds
	struct BuildJobs_t
	{
		// 3 after construction: the pending jobs are submitted while fewer jobs run
		int m_nMaxRunningJobs;
		// The submitted jobs; a finished one (state 0 or 3) is released and removed
		CUtlVector< CJob * > m_RunningJobs;
		// The jobs waiting to be submitted (taken from the back)
		CUtlVector< CJob * > m_PendingJobs;
		// Called (without arguments) each time a running job finishes and is removed; empty after
		// construction
		std::function< void() > m_fnOnJobFinished;
		// The pool the jobs are submitted to (g_pThreadPool when null); cleared before the build's
		// pool is destroyed
		IThreadPool *m_pThreadPool;
	};

	// The state machine of the background build: m_nState 0 idle, 1 building the tree
	// ("Space - building %d"), 2 calculating the blocks ("Space - CalcBlocks"), 3 done (the
	// thread pool is destroyed). 144 bytes on Linux, 176 on Windows. Named by what it holds
	struct Builder_t
	{
		CNavSpace *m_pSpace;
		// Set by CNavSpaceGame: the physics world the game's physics queries trace in (the first
		// member of the game's physics query object)
		IVPhysics2World *m_pPhysicsWorld;
		int m_nState;
		BuildJobs_t m_Jobs;
		// Created for the build, destroyed when it ends
		IThreadPool *m_pThreadPool;
		// The reference counted calc blocks job (CNavSpaceCalcBlocksJob); state 4 means done
		CJob *m_pCalcBlocksJob;
		// Stub_3's argument: drive the build to its end without waiting. Named by what it does
		bool m_bFinishImmediately;
		// Zeroed by the constructor (a 4-byte store, so a member rather than padding); no other
		// access was found in either server library
		int m_nReserved;
	};

	virtual ~CNavSpace() = 0;
	// Called on the active space every server frame: advances the background jobs that build
	// the volume tree and calculate its blocks. Named by what it does
	virtual void Update() = 0;
	// Sets m_Builder.m_bFinishImmediately and drives the build jobs to the end (shuts the job
	// pool down); called on the old space before another becomes active, and with false when
	// the active spawn group changes
	virtual void Stub_3( bool bArg ) = 0;
	// When the active nav space changes, the old one gets Stub_3 and OnDeactivated and the new
	// one OnActivated. Both forward to INavObstructionManager::OnNavSpaceActivated /
	// OnNavSpaceDeactivated. Named by what they do
	virtual void OnActivated() = 0;
	virtual void OnDeactivated() = 0;
	// Creates a block (an 80-byte CNavBlock with a new id and the given volume points) and adds
	// it to m_Blocks
	virtual void Stub_6() = 0;
	// Removes the block of the argument's id when the argument is set
	virtual void Stub_7( void *pArg ) = 0;
	// False in CNavSpace; CNavSpaceGame tests the info_nav_space entities against a box grown by
	// a float
	virtual void Stub_8() = 0;

public:
	// The space loaded from the map (a 64-byte object: its destructor frees a vector of the cell
	// data objects); the game space loads it in its constructor. Its type is not reconstructed
	void *m_pSpaceData;
	// A 112-byte object: a CAtomicMutex and three CUtlHashtables at 16, 48 and 80 (24-byte entries);
	// the key and value types of the tables are not identified, so its type is not reconstructed
	void *m_pLookupTables;
	CUtlVector< SpaceCell_t > m_Cells;
	// The blocks by id
	CUtlHashtable< uint32, CNavBlock * > m_Blocks;
	// 40 bytes the constructors zero (the Windows constructor with 8-, 4-, 12-, 12- and 4-byte
	// stores at 80, 88, 92, 104 and 116); no other access was found in either server library
	// (the methods of CNavSpace and CNavSpaceGame, the build jobs and the users of the active space
	// were searched), so the parts are not named
	uint8 m_Reserved[ 40 ];
	Builder_t m_Builder;
};

#endif // NAV_SPACE_H
