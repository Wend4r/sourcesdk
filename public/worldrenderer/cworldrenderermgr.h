#ifndef CWORLDRENDERERMGR_H
#define CWORLDRENDERERMGR_H

#ifdef _WIN32
	#pragma once
#endif

#include "iworldrenderermgr.h"
#include "iworldloadunload.h"
#include "csingleworldrep.h"
#include "centitylump.h"

#include "resourcefile/resourcetype.h"
#include "resourcefile/introspectedtokv3typemanager.h"
#include <tier0/platform.h>
#include <tier0/threadtools.h>
#include <tier1/utlvector.h>
#include <tier1/utlstringmap.h>
#include <tier1/utlmap.h>
#include <tier1/convar.h>
#include <tier3/tier3.h>

struct WorldNode_t;
class ISceneWorld;

struct WorldWindParams_t
{
	float m_flParams[6];
};

struct WindEntry_t
{
	const ISceneWorld* m_pSceneWorld;
	Vector m_vTargetWindDir;
	float m_flTargetWindSpeed;
	float m_flLastWindSpeed;
	float m_flLastSetTime;
	Vector m_vWindDir;
	float m_flWindSpeed;
	Vector m_vWindOffset;
	bool m_bOverrideWind;
};
static_assert( sizeof( WindEntry_t ) == 0x40 );

class CWorldNodeManager : public CDefaultIntrospectedToKV3TypeManager<0x646F6E7776 /* 'vwnod' */, WorldNode_t>
{
};

class CWorldManager : public CDefaultIntrospectedToKV3TypeManager<0x646C727776 /* 'vwrld' */, World_t>
{
};

class CVoxelVisibilityTypeManager : public IResourceTypeManager
{
};

class CEntityMgr : public CTypesafeIntrospectedToKV3TypeManager<0x73746E6576 /* 'vents' */, PermEntityLumpData_t>
{
};

struct SingleWorldEntityLump_t
{
	CUtlString m_worldName;
	HEntityLumpStrong m_hEntityLump;
};

static_assert( sizeof( SingleWorldEntityLump_t ) == 0x10 );

class CWorldRendererMgr : public CTier3AppSystem<IWorldRendererMgr>
{
public:
	uint32 GetWorldCountDirect() const { return m_worldList.Count(); }

public:
	CWorldNodeManager m_worldNodeManager;
	CWorldManager m_worldManager;
	CVoxelVisibilityTypeManager m_worldVisManager;
	CEntityMgr m_entityLumpManager;
	CUtlVector<CSingleWorldRep*> m_worldList;
	CUtlVector<CSingleWorldRep*> m_worldPendingList;
	CUtlVector<CSingleWorldRep*> m_worldDeletionList;
	CThreadRWLock_FastRead m_worldLock;
	CUtlStringMap<int> m_worldVPKRefcounts;
	CUtlVector<SingleWorldEntityLump_t> m_worldEntityLumps;
	CUtlVector<IWorldLoadUnloadCallback*> m_worldLoadHandlers;
	CUtlVector<IWorldLoadUnloadCallback*> m_worldCreateCallbacks;
	CAtomicMutex m_worldRequestsMutex;
	CUtlVector<void*> m_worldRequestQueue;
	CUtlVector<WindEntry_t> m_windEntries;
	Vector m_vDefaultWindOffset;
	CUtlVector<IWorldVPKOverrideManager*> m_vpkOverrideManagers;
	CUtlOrderedMap<const ISceneWorld*, WorldWindParams_t> m_windEntryMap;
	float m_flOverrideRenderFrameTime;
	float m_flOverrideRenderTime;

	CConCommandMemberAccessor<CWorldRendererMgr> entity_lump_list;
	CConCommandMemberAccessor<CWorldRendererMgr> entity_lump_spew;
	CConCommandMemberAccessor<CWorldRendererMgr> world_layer_list;
	CConCommandMemberAccessor<CWorldRendererMgr> world_layer_set_visible;
	CConCommandMemberAccessor<CWorldRendererMgr> world_dump_loaded_worlds;
};

static_assert( offsetof( CWorldRendererMgr, m_worldList ) == 0x90 );
static_assert( offsetof( CWorldRendererMgr, m_worldLock ) == 0xD8 );
static_assert( offsetof( CWorldRendererMgr, m_worldEntityLumps ) == 0x190 );
static_assert( offsetof( CWorldRendererMgr, m_worldLoadHandlers ) == 0x1A8 );
static_assert( offsetof( CWorldRendererMgr, m_worldCreateCallbacks ) == 0x1C0 );
static_assert( offsetof( CWorldRendererMgr, m_worldRequestsMutex ) == 0x1D8 );
static_assert( offsetof( CWorldRendererMgr, m_worldRequestQueue ) == 0x1E8 );
static_assert( offsetof( CWorldRendererMgr, m_windEntries ) == 0x200 );
static_assert( offsetof( CWorldRendererMgr, m_vDefaultWindOffset ) == 0x218 );
static_assert( offsetof( CWorldRendererMgr, m_vpkOverrideManagers ) == 0x228 );
static_assert( offsetof( CWorldRendererMgr, m_flOverrideRenderFrameTime ) == 0x268 );
static_assert( sizeof( CWorldRendererMgr ) == 0x360 );

#endif // CWORLDRENDERERMGR_H
