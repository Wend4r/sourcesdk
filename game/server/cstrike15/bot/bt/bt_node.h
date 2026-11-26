#ifndef BT_NODE_H
#define BT_NODE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

class IBtAgent;

// What a node reports to its parent. Named by what the composites do with the values: a
// selector stops at the first child that does not fail, a wait node reports running until its
// time is up and success afterwards.
enum BtNodeEvalResult_t : int
{
	BT_NODE_SUCCESS = 0,
	BT_NODE_FAILURE = 1,
	BT_NODE_RUNNING = 2
};

//-----------------------------------------------------------------------------
// Virtual table of CBtNode, the base of every node of a bot's behaviour tree (schema class
// CBtNode, 88 bytes). A tree is built from a KeyValues3 file: the "type" string of each node
// picks the class (action_*, decorator_*, condition_*, selector, sequencer, parallel,
// subtree), "children" fills the child list of composites and decorators.
//
// The class declares virtual methods only and carries a VTable suffix; its data members are
// not declared. The Linux table has 11 slots, the Windows table 10: the destructor takes two
// slots on Linux and one on Windows, the methods after it sit one slot lower on Windows.
//
// The names of Init, Start, Execute, Abort, Terminate, GetName and PostInit are named by what
// they do; Eval and the result type are the game's own names.
//-----------------------------------------------------------------------------
abstract_class CBtNodeVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;

	// The tree calls it on its root once: stores the tree data, the global and the local
	// blackboard and the agent, then calls PostInit. Composites, decorators and the subtree
	// pass it on to their children. Named by what it does.
	virtual bool Init( void *pTreeData, void *pGlobalBlackboard, void *pLocalBlackboard, IBtAgent *pAgent ) = 0;
	// Called before Eval when the node is not running yet: by the tree on its root, by a
	// selector on a child that is not its running child. False fails the node. Named by
	// what it does.
	virtual bool Start() = 0;
	// Runs the node; the tree evaluates its root inside the "CBtTree::Eval" profiler scope.
	// The node keeps the result and the time of the last call.
	virtual BtNodeEvalResult_t Eval() = 0;
	// The tree calls it on its root inside the "CBtTree::Execute" profiler scope. The node
	// keeps the result and the time of the last call. Named after that scope.
	virtual BtNodeEvalResult_t Execute() = 0;
	// A selector calls it on its running child when it switches to another child: a wait
	// node forgets its end time, a lock node gives its lock back. Named by what it does.
	virtual void Abort() = 0;
	// Passed on only to running children; lock and run-once nodes give back their lock or
	// token. Named by what it does.
	virtual void Terminate() = 0;

	virtual ~CBtNodeVTable() = 0;

	// The label of the node in the behaviour tree debug overlay ("Selector", "Wait",
	// "ConditionIsEqual", ...); the subtree returns its file name. Named by what it does.
	virtual const char *GetName() const = 0;
	// The last step of Init; empty in the base. Named by what it does.
	virtual void PostInit() = 0;
};

#endif // BT_NODE_H
