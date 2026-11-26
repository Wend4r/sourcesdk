//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Virtual tables of the classes the server registers its Pulse bindings with: the API
//          extension registration contexts, the input parameter adapters of entity inputs
//          and the loader of the domain scopes.
//
//=============================================================================//

#ifndef PULSE_SERVER_LIBRARY_H
#define PULSE_SERVER_LIBRARY_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlstring.h"
#include "tier0/utlhashtable.h"
#include "interfaces/interfaces.h"
#include "entity2/entitypulse.h"

class CBasePulseGraphInstance;
class CBufferString;
class CEntityInstance;
class CPulseCallContextValue;
class CPulseValueFullType;
class CPulseTypeQueriesModuleMetadataProvider;
class CSchemaSystemTypeScope;
class IPulseTypeQueriesForScope;
struct PulseSymbol_t;

//-----------------------------------------------------------------------------
// Passed to the API extensions of the entity classes when they register their Pulse bindings
// (CEntityPulseAPIExtensionRegistrationContext, CEntityPulseAPICheckComponents). No virtual
// destructor. An interface without data of its own: the implementations keep their members
// right after the virtual table pointer.
//-----------------------------------------------------------------------------
abstract_class CPulseAPIExtensionRegistrationContext
{
public:
	// CEntityPulseAPICheckComponents keeps a copy of the type / clears it; empty in the
	// registration context
	virtual void Stub_0( const CPulseValueFullType &type ) = 0;
	virtual void Stub_1() = 0;
	// Named by what they do. The library apply functions (PulseLibraryRegistration_t::m_pfnApply)
	// call them in this order: BeginComponent for an API on a component of the entity
	// ("TestComponent_t" and the function that returns the component of an entity), then
	// RegisterLibrary, Stub_5, EndComponent. The registration context keeps the component getter
	// for the methods it registers, CEntityPulseAPICheckComponents calls it on its entity
	virtual void BeginComponent( const char *pszComponentClass, void *( *pfnGetComponent )( CEntityInstance *pEntity ) ) = 0;
	virtual void EndComponent() = 0;
	// Registers the methods of the library (m_nFeature 1 only; "MPulseLegacyName" adds the old
	// names)
	virtual void RegisterLibrary( const PulseLibraryRegistration_t *pLibrary ) = 0;
	// Empty in both contexts
	virtual void Stub_5() = 0;
	// The registration context appends the name to a list of the entity class and a 32-byte
	// record { name, 0, the component getter, nValue }; empty in CEntityPulseAPICheckComponents
	virtual void Stub_6( void *pArg, int nValue, const char *pszName ) = 0;
};

//-----------------------------------------------------------------------------
// Converts the parameters of an entity input to the arguments of a Pulse binding. The Pulse
// runtime walks the parameters through these methods only. No virtual destructor and no data
// of its own.
//-----------------------------------------------------------------------------
abstract_class CBasePulseInputParameterAdapter
{
public:
	// Named by what it does: the name, type and default value of parameter nIndex (from the C++
	// argument list or the method signature)
	virtual void GetParameter( int nIndex, const char **ppszName, CPulseValueFullType *pType, const CPulseCallContextValue **ppDefaultValue ) = 0;
	// The three store one input value into the arguments of the call under the parameter symbol
	// and index: converted to the parameter type, from a typed value with a fallback conversion,
	// or (Stub_3) as the typed value when it has a type and through another setter otherwise
	virtual bool Stub_1( const PulseSymbol_t &name, int nIndex, void *pArg3, const void *pArg4 ) = 0;
	virtual bool Stub_2( const PulseSymbol_t &name, int nIndex, const void *pArg3, void *pArg4 ) = 0;
	virtual void Stub_3( const PulseSymbol_t &name, int nIndex, const void *pArg3, const void *pArg4 ) = 0;
	// Named by what it does: formats "PulseAPIConvertArgumentsForCall: Binding '%s' Inparam '%s':
	// %s" (binding, parameter, message) into pError and adds the value
	virtual bool ReportConversionError( CBufferString *pError, const char *pszMessage, const char *pszParamName, const char *pszBindingName, const void *pArg5, const void *pArg6 ) = 0;
};

//-----------------------------------------------------------------------------
// The adapter of an entity input: implements the three setters and ReportConversionError and
// keeps the call the input runs in. The derived adapters (CPulseInputParameterAdapterWithCPPSignature,
// ...WithMethodSignature) add the parameter list GetParameter reads. Built on the stack by the
// input dispatch of the pulse signatures; members named by what they hold.
//-----------------------------------------------------------------------------
abstract_class CPulseInputParameterAdapterWithEntityContext : public CBasePulseInputParameterAdapter
{
public:
	// The input arguments of the binding call the setters store into
	PulseArgBlock_t *m_pInParams;
	// The activator and the caller of the input, and the entity that receives it: entity name
	// arguments are resolved against them (the searching entity, the activator and the caller of
	// the entity name lookup)
	CEntityInstance *m_pActivator;
	CEntityInstance *m_pCaller;
	CEntityInstance *m_pEntity;
};

//-----------------------------------------------------------------------------
// Links every object of the class T into a list of the module at construction: the newest object
// is the head the module keeps, m_pNextRegistration the one constructed before it.
//-----------------------------------------------------------------------------
template < class T >
class CPulseModuleRegistrationHook
{
public:
	T *m_pNextRegistration;
};

//-----------------------------------------------------------------------------
// Loads the Pulse domain scope of a module once its interfaces are connected: one static object
// per module (the server constructs it with scope 1). Methods and members named by what they do.
//-----------------------------------------------------------------------------
abstract_class CPulseDomainScopeLoader : public CPulseModuleRegistrationHook< CPulseDomainScopeLoader >, public CPostConnectCallback
{
public:
	// Once: stores the type query services of the module by value type (calling their
	// OnRegistered) and the methods of its libraries by name
	virtual void BuildBindings() = 0;
	// The scope of the module: 0 pulse_system, 1 server, 2 client, 3 resourcecompiler, 4 test
	virtual int GetScope() const = 0;
	// The schema type scope of the module
	virtual CSchemaSystemTypeScope *GetSchemaTypeScope() const = 0;
	// The module the domain scope belongs to ("server.dll", "client.dll", "pulse_system.dll",
	// "resourcecompiler.dll", "[test]")
	virtual const char *GetModuleName() const = 0;
	// Whether a domain of the name is registered for the scope
	virtual bool HasDomain( const char *pszDomain ) const = 0;
	// Loads a graph definition (the instance load API and the KV3 transfer interfaces;
	// "Failed to load %s: %s")
	virtual void *LoadGraphDef( void *pArg1, const void *pArg2, void *pArg3 ) = 0;
	// Creates an instance through the factory of the registered domain; null without one
	virtual CBasePulseGraphInstance *CreateDomainInstance( const char *pszDomain ) = 0;
	// The binding of the method name (aliases included); null when there is none
	virtual const PulseBindingDesc_t *FindMethod( const char *pszMethod ) const = 0;
	// The current name of a renamed hook; null when the name is not an old one
	virtual const char *FindCurrentHookName( const char *pszHook ) const = 0;
	// Checks the value against the schema enum named by the subtype of the type
	virtual bool ValidateSchemaEnumValue( const CPulseValueFullType &type, const void *pValue, CUtlString *pError ) const = 0;
	// Forward to the type query service of the value type (IPulseTypeQueriesForScope of the same
	// names); false / an invalid type without one
	virtual bool IsSubTypeOf( const CPulseValueFullType &type, const CPulseValueFullType &baseType ) const = 0;
	virtual bool IsValueOfType( const CPulseValueFullType &type, const void *pValue, const CPulseValueFullType &testType ) const = 0;
	virtual bool IsValueValid( const CPulseValueFullType &type, const void *pValue ) const = 0;
	virtual CPulseValueFullType GetBaseType( const CPulseValueFullType &type ) const = 0;
	virtual CPulseValueFullType GetValueTypeOf( const CPulseValueFullType &type, const void *pValue ) const = 0;
	virtual void GetSubTypes( int nValueType, void *pArg1, void *pArg2 ) const = 0;
	// Appends the extra registrations of the module
	virtual void AppendExtraRegistrations( CUtlVector< void * > *pRegistrations ) const = 0;
	// Appends the enumerator names of the schema enum named by the subtype of the type
	virtual void GetSchemaEnumeratorNames( const CPulseValueFullType &type, CUtlVector< CUtlString > *pNames ) const = 0;

public:
	// The scope GetScope returns (see GetScope for the values)
	int m_nScope;
	// The third constructor argument: the Windows server passes its static
	// CPulseTypeQueriesModuleMetadataProvider object, the Linux server null. No method of the
	// loader reads it
	CPulseTypeQueriesModuleMetadataProvider *m_pTypeQueriesMetadataProvider;
	// Set by the first BuildBindings
	bool m_bBindingsBuilt;
	// The library methods by name, also by their legacy names and as "<library>::<method>"
	// (FindMethod)
	CUtlHashtable< CUtlString, const PulseBindingDesc_t * > m_Methods;
	// The current name of a hook by its legacy name (FindCurrentHookName)
	CUtlHashtable< CUtlString, CUtlString > m_LegacyHookNames;
	// The type query service of each value type, null where the module has none
	IPulseTypeQueriesForScope *m_pTypeQueries[ PVAL_COUNT ];
};

#endif // PULSE_SERVER_LIBRARY_H
