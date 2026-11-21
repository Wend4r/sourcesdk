//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The developer commentary system (point_commentary_node).
//
//=============================================================================//

#ifndef COMMENTARYSYSTEM_H
#define COMMENTARYSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ehandle.h"
#include "entitytypes.h"
#include "kv3lib/kv3transfer.h"
#include "schemasystem/schematypes.h"

#include <tier0/utlvector.h>

#include "schemacompiler/schemaon.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CPointCommentaryNode;

// A convar the commentary changed and its value before (schema, 384 bytes)
schema struct modifiedconvars_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	char pszConvar[ 128 ];
	char pszCurrentValue[ 128 ];
	char pszOrgValue[ 128 ];
};

//-----------------------------------------------------------------------------
// Registered as "CCommentarySystem" (reallocating factory, 96 bytes). A schema class (declared in
// commentarysystem.cpp): its four own virtual methods
// are the schema binding, the KV3 transfer pair and the class name. Field names: schema.
//-----------------------------------------------------------------------------
schema class CCommentarySystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();
	TYPEMETA( MGetKV3ClassDefaults );

	CCommentarySystem();
	virtual ~CCommentarySystem();

	// Reads "CommentaryMode" from the session's command key values and turns the commentary on or off;
	// resets m_hLastCommentaryNode, m_bCommentaryConvarsChanging, m_bCheatState and
	// m_bIsFirstSpawnGroupToLoad
	GS_EVENT( GameInit );
	// Stops the active node and removes the "commentary_semaphore" entities
	GS_EVENT( GameDeactivate );
	// Fires "playing_commentary" after the first spawn group loaded in commentary mode
	GS_EVENT( PostSpawnGroupLoad );
	// Restores the convars of m_ModifiedConvars
	GS_EVENT( RestoreGame );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();
	CLASS_USES_KV3TRANSFER_VIRTUAL( CCommentarySystem );
	// Returns "CCommentarySystem"
	virtual const char *GetClassName() const;

public:
	// Not a schema field. GameInit clears it
	noschema bool m_bCommentaryConvarsChanging;
	bool m_bCommentaryEnabledMidGame;
	GameTime_t m_flNextTeleportTime;
	int32 m_iTeleportStage;
	bool m_bCheatState;
	bool m_bIsFirstSpawnGroupToLoad;
	CUtlVector< modifiedconvars_t > m_ModifiedConvars;
	CHandle< CPointCommentaryNode > m_hCurrentNode;
	CHandle< CPointCommentaryNode > m_hActiveCommentaryNode;
	CHandle< CPointCommentaryNode > m_hLastCommentaryNode;
	CUtlVector< CHandle< CPointCommentaryNode > > m_vecNodes;
};

#include "schemacompiler/schemaoff.h"

#endif // COMMENTARYSYSTEM_H
