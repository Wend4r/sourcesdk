#ifndef ENTITYSUBCLASSVDATA_H
#define ENTITYSUBCLASSVDATA_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "networkvar.h"
#include "schemasystem/schematypes.h"

#include "schemacompiler/schemaon.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
struct CEntityPrecacheContext;

//-----------------------------------------------------------------------------
// Base of the classes that hold the data of an entity subclass, one object per subclass of a
// .vdata file ("weapon_ak47" in scripts/weapons.vdata). The subclass game system creates the
// object for the class named by the "_class" key of the subclass, fills the members below and
// then loads the rest of the subclass through KV3TransferLoad.
//
// The game implements the virtual methods; the objects are only reached through pointers.
//-----------------------------------------------------------------------------
schema class CEntitySubclassVDataBase : public CMemZeroOnNew
{
public:
	TYPEMETA( MGetKV3ClassDefaults; MVDataRoot; MVDataNodeType = 1; MVDataOverlayType = 1; MVDataUseLinkedEntityClasses );

	virtual ~CEntitySubclassVDataBase();
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext );

	// The name of the most derived class, "CEntitySubclassVDataBase" here
	virtual const char *GetKV3TransferClassName() const;

	// Precaches the resources the data names: empty here, the derived classes precache their
	// models and particles. Named by what the overrides do.
	virtual void Precache( const CEntityPrecacheContext *pContext );

	// Index of the subclass type in the tables of the subclass game system, the value its
	// ParseSubclass filters by. Named by what it holds.
	noschema int32 m_nSubclassType;

	// Token of the lowercase subclass name, the key of the subclass in the game system and the
	// value of CBaseEntity::m_nSubclassID of the entities of this subclass
	noschema CUtlStringToken m_nSubclassID;

	// The subclass name ("weapon_ak47")
	noschema CUtlString m_szSubclassName;

	// The entity class of the subclass: the "_class" key, or the subclass name without it
	noschema CUtlString m_szEntityClassName;

	// The "_not_pickable" key of the subclass; a subclass where it is 2 is not loaded at all
	noschema bool m_bNotPickable;
};

#include "schemacompiler/schemaoff.h"

#endif // ENTITYSUBCLASSVDATA_H
