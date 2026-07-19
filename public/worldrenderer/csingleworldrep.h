#ifndef CSINGLEWORLDREP_H
#define CSINGLEWORLDREP_H

#ifdef _WIN32
	#pragma once
#endif

#include "iworldreference.h"
#include "icomputeworldorigin.h"

#include "resourcefile/resourcehandle.h"
#include <tier0/platform.h>
#include <tier0/utlstring.h>

class CWorld;
class CVoxelVisibility;
struct CreateWorldInfo_t;

class CSingleWorldRep : public IWorldReference
{
public:
	CSingleWorldRep( const CreateWorldInfo_t& info );
	virtual ~CSingleWorldRep();

	const char* GetWorldName() const { return m_worldName.Get(); }
	const char* GetFolder() const { return m_folder.Get(); }
	CWorld* GetGeometryWorld() const { return m_pGeometryWorld; }

public:
	CUtlString m_worldName;
	CUtlString m_folder;
	HResourceManifest m_hWorldManifest;
	HWorldStrong m_hGeometryWorld;
	HWorldVisStrong m_hWorldVis;
	CWorld* m_pGeometryWorld;
	CVoxelVisibility* m_pWorldVisibility;
	IComputeWorldOriginCallback* m_pComputeOriginCallback;
	HResourceManifest m_hResourceManifest;
	uint8 m_priority;
	WorldGroupId_t m_worldGroupId;
	uint8 m_nFlags;
	uint8 m_nFlags2;
	int32 m_nRefCount;
	SpawnGroupHandle_t m_hSpawnGroup;
};

#endif // CSINGLEWORLDREP_H
