#ifndef SCRIPTCOMPONENT_H
#define SCRIPTCOMPONENT_H

#if _WIN32
#pragma once
#endif

#include "tier1/utlsymbollarge.h"
#include "tier1/utlvector.h"
#include "entity2/entitycomponent.h"
#include "variant.h"

FORWARD_DECLARE_HANDLE( HSCRIPT );

class CEntityIOOutput;
class CEntityKeyValues;
struct CEntityPrecacheContext;

// Script side of an entity whose class is implemented in script. The game allocates it with new when the public
// script scope of the entity is created and frees it when that scope is released; the component helper does neither.
class CScriptComponent : public CEntityComponent
{
public:
	enum ScopeFlags_t
	{
		// The scope handle is the script instance owned by the script system; releasing the component does not release it.
		SCOPE_EXTERNAL = 1,
	};

	// Block of the game's ultra-conservative vector: the count followed by the elements.
	struct FuncHandles_t
	{
		int m_Size;
		HSCRIPT *m_Elements[ 1 ];
	};

	// Releases the scope and the script outputs.
	~CScriptComponent()
	{
		ReleaseScope();
		PurgeScriptOutputs();
	}

	// Not overridden here: Schema_DynamicBinding, GetDataDescMap, GetSchemaBinding and GetComponentHelper.
	// The game returns module-owned statics from them: the schema binding of "CScriptComponent", an empty datamap
	// named "CEntityComponent" and the lazily registered component helper named "ScriptComponent".
	void KV3TransferSave( CKV3TransferSaveContext *pContext ) const override {}
	void KV3TransferLoad( CKV3TransferLoadContext *pContext ) override;
	const char *GetComponentName() override { return "CScriptComponent"; }

	// The script class instance; "Dispatch*" functions of the script system receive it.
	HSCRIPT GetScriptInstance() const { return m_hScope; }
	const char *GetScriptClassName() const { return m_scriptClassName.String(); }

	// Releases the cached function handles and, unless SCOPE_EXTERNAL is set, the scope, then clears the flags and the class name.
	void ReleaseScope();

	// Destroys the script outputs from last to first and frees their block.
	void PurgeScriptOutputs();

	// Appends the script outputs named pszName to outputs.
	void FindScriptOutputs( const char *pszName, CUtlVector< CEntityIOOutput * > &outputs );

	// Returns the outputs of m_pScriptOutputs and their count; null when there are none.
	CEntityIOOutput *GetScriptOutputs( int *pCount );

	// Call "DispatchActivate" and "DispatchUpdateOnRemove" of the entity framework table with the script instance.
	// pEntity names the entity in the warning printed when the script instance is null.
	// DispatchActivate returns the script result converted to bool, or true when the function could not be called.
	bool DispatchActivate( const CEntityInstance *pEntity );

	// Calls "DispatchPrecache" with the script instance and pContext registered as a CScriptPrecacheContext instance for the call.
	void DispatchPrecache( const CEntityInstance *pEntity, const CEntityPrecacheContext *pContext );

	// Calls "DispatchSpawn" with the script instance and pKeyValues registered as a CScriptKeyValues instance for the call.
	void DispatchSpawn( const CEntityInstance *pEntity, const CEntityKeyValues *pKeyValues );
	void DispatchUpdateOnRemove( const CEntityInstance *pEntity );

	// Calls "DispatchInput" with the script instance, pszInputName and hArgs, the table of the input arguments.
	// Returns the script result converted to bool; false without a script instance or when the call fails.
	bool DispatchInput( const char *pszInputName, HSCRIPT hArgs );

protected:
	// Calls pszFunction of the "EntityFramework" table that the entity framework script stores in the root table.
	// Returns false when the table or the function is missing or the call fails.
	static bool CallEntityFrameworkFunction( const char *pszFunction, CVariant *pArgs, int nArgs, CVariant *pReturn );

public:
	// The members before m_pOwner belong to the game's second base CScriptScopeT< CDefScriptAccessorBase, true >,
	// which directly follows the vtable pointer. Its layout differs from the SDK script scope template.
	HSCRIPT m_hScope;

	// The game's vectors here are a pointer to their block, null when empty rather than the static empty block
	// CUtlVectorUltraConservative expects, so they are kept as the block pointers.
	FuncHandles_t *m_pFuncHandles;

	// Initialization with a script instance sets it to 3, SCOPE_EXTERNAL and an unidentified bit.
	int m_nScopeFlags;

	CEntityOwnerPtr m_pOwner;

	// Outputs declared by the script class, one per output descriptor the script system registered for it:
	// an int count followed by the aligned outputs, null when empty. See GetScriptOutputs.
	void *m_pScriptOutputs;

	CUtlSymbolLarge m_scriptClassName;
};

#endif // SCRIPTCOMPONENT_H
