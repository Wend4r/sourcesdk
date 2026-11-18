#ifndef ANIMGRAPHCONTROLLER_H
#define ANIMGRAPHCONTROLLER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/globalsymbol.h"
#include "tier0/utlvector.h"
#include "entityhandle.h"
#include "schemasystem/schematypes.h"
#include "animlib/graph/animgraph_valuetypes.h"

#include "schemacompiler/schemaon.h"

class CBaseEntity;
class CNmGraphInstance;
class CAnimGraphControllerManager;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

// Handle of an external graph connected to the graph of an entity, -1 for none
schema struct ExternalAnimGraphHandle_t
{
	TYPEMETA( MIsBoxedIntegerType );

	uint32 m_Value;
};

//-----------------------------------------------------------------------------
// A control parameter of an AnimGraph2 graph that a graph controller drives, looked up by its ID
// in the graph instance the controller binds to. The counterpart of the ControlParameter of an
// Esoterica graph controller. T is the type of the value: bool, float32, CGlobalSymbol and so on.
//-----------------------------------------------------------------------------
template < typename T >
class CAnimGraph2ParamRefBase
{
public:
	bool IsBound() const { return m_pBoundGraphInstance != nullptr && m_index >= 0; }

	CGlobalSymbol m_ID;
	CNmGraphInstance *m_pBoundGraphInstance;
	int16 m_index;

	// A missing optional parameter is not reported; CAnimGraph2ParamOptionalRef sets it.
	// Named by the dump of the bindings ("OPTIONAL").
	bool m_bOptional;

	// Set when the controller tried to bind the parameter, whether it was found or not.
	// Named by the dump of the bindings ("BOUND").
	bool m_bBound;
};

// A parameter the controller drives when the graph has it
template < typename T >
class CAnimGraph2ParamOptionalRef : public CAnimGraph2ParamRefBase< T >
{
};

// A parameter that goes back to its default value after every update of the graph, so it
// works as a trigger. The controller keeps a list of them and resets them in PostGraphUpdate.
abstract_class IAnimGraph2ParamAutoReset
{
public:
	// Writes the default value into the node of the parameter
	virtual void Reset() = 0;
};

// A parameter whose value the controller copies, before every update of the graph, from the
// graph of another entity (m_hParameterSourceEntity). Named by what it holds.
struct AnimGraphControllerCopiedParameter_t
{
	CGlobalSymbol m_ID;
	int16 m_index;
	NmGraphValueType_t m_valueType;
};

//-----------------------------------------------------------------------------
// Base of the classes that drive the control parameters of an AnimGraph2 graph from the state
// of the game (CCS2PawnGraphController, CCS2WeaponGraphController, CDynamicPropGraphController
// and so on). The CAnimGraphControllerManager of an animated entity owns its controllers, binds
// them to the graph instance once it exists and calls PreGraphUpdate and PostGraphUpdate of each
// around every update of the graph.
//
// The game implements the virtual methods; the objects are only reached through pointers.
//-----------------------------------------------------------------------------
schema class CAnimGraphControllerBase
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext );

	// The name of the most derived class, "CAnimGraphControllerBase" here
	virtual const char *GetKV3TransferClassName() const;

	virtual ~CAnimGraphControllerBase();

	// Prints "Bindings for <class>" and every parameter with its type and state, then calls
	// PrintAdditionalBindings; graphcontroller_dumpparams runs it. Named by what it does.
	virtual void PrintBindings();

	// True when the controller drives the parameter with this ID; false here. Named by what it does.
	virtual bool HasParameter( const CGlobalSymbol &parameterID );

	// Writes the current value of the parameter with this ID into a tagged value of the graph
	// (the data, then its NmGraphValueType_t) and returns true; false here and for an ID the
	// controller does not drive. Used to copy parameters into the graph of another entity.
	// Named by what it does.
	virtual bool GetParameterValue( const CGlobalSymbol &parameterID, void *pOutValue );

	// Empty in this class and never overridden, in the server and in the client; no caller was
	// found. Named by what it does.
	virtual void EmptyControllerHook();

	// Empty here; CCS2WeaponGraphController has an empty override of its own that the override
	// of CCS2PawnGraphController calls first, as the pawn does with every parameter method of the
	// weapon controller. No caller was found. Named by what it does.
	virtual void EmptyChainedParameterHook();

	// Sets every bound parameter back to the default value of its node; empty here. Named by
	// what it does.
	virtual void ResetParametersToDefault();

	// Finds the controller of the same class for this owner and external graph in the manager or
	// creates one and adds it; null here. Named by what it does.
	virtual CAnimGraphControllerBase *FindOrCreateInManager( CAnimGraphControllerManager *pManager, CBaseEntity *pOwner, ExternalAnimGraphHandle_t hExternalGraph );

	// Called by CAnimGraphControllerManager::CreateControllerToGraphBindings once m_pGraphInstance
	// is set: stores the context, calls OnCreateGraphBindings and ClearCopiedParametersOnCreate;
	// the derived classes then bind each parameter. The context is the member of the manager
	// that precedes its pointer to the main graph instance.
	virtual void CreateGraphBindings( void *pBindingContext );

	// The manager calls it when it removes the controller while bindings exist; the derived
	// classes unbind each parameter, then ClearCopiedParametersOnDestroy runs
	virtual void DestroyGraphBindings();

	// Both clear m_copiedParameters and m_hParameterSourceEntity; CreateGraphBindings calls this
	// one, DestroyGraphBindings the next. Named by what they do.
	virtual void ClearCopiedParametersOnCreate();
	virtual void ClearCopiedParametersOnDestroy();

	// Empty, never overridden; PrintBindings calls it last. Named by its caller.
	virtual void PrintAdditionalBindings();

	// Empty, never overridden; CreateGraphBindings calls it first. Named by its caller.
	virtual void OnCreateGraphBindings();

	// A constant of the derived class: the key of the controller in the manager and the
	// "ControllerTypeID" of a saved CAnimGraphControllerPtr
	virtual uint32 GetControllerTypeID() = 0;

	// CAnimGraphControllerManager::PreGraphUpdate, before the graph is evaluated
	virtual void PreGraphUpdate();

	// After the graph is evaluated: reads the sampled events and resets m_autoResetParameters.
	// Named by its counterpart in Esoterica.
	virtual void PostGraphUpdate();

	// The graph instance the controller binds its parameters to: the main graph of the entity,
	// or the external graph of m_hExternalGraph
	noschema CNmGraphInstance *m_pGraphInstance;

	noschema CUtlVectorFixedGrowable< AnimGraphControllerCopiedParameter_t, 2 > m_copiedParameters;

	// The CBaseAnimGraph whose controllers give the values of m_copiedParameters
	noschema CEntityHandle m_hParameterSourceEntity;

	ExternalAnimGraphHandle_t m_hExternalGraph;

	// What CreateGraphBindings received
	noschema void *m_pBindingContext;

	// The entity whose manager owns the controller, and the entity the controller works for
	// ("GraphControllerManagerOwner" and "GraphControllerOwner" of a saved CAnimGraphControllerPtr);
	// a weapon controller lives in the manager of the pawn
	noschema CBaseEntity *m_pGraphControllerManagerOwner;
	noschema CBaseEntity *m_pGraphControllerOwner;

	noschema CUtlVectorFixedGrowable< IAnimGraph2ParamAutoReset *, 8 > m_autoResetParameters;
};

#include "schemacompiler/schemaoff.h"

#endif // ANIMGRAPHCONTROLLER_H
