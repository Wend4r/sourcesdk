#ifndef IWORLDRENDERERMGR_H
#define IWORLDRENDERERMGR_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "worldschema.h"

#include <appframework/iappsystem.h>
#include <tier0/platform.h>

struct CreateWorldInfo_t;
class CThreadRWLock;
class IWorld;
class CEntityKeyValues;
class IWorldReference;
class CWorldVisibility;
class IWorldLoadUnloadCallback;
class IWorldVPKOverrideManager;
class IWorldVPKReference;

abstract_class IWorldRendererMgr : public IAppSystem
{
public:
	virtual IWorldReference *CreateWorld( CreateWorldInfo_t &info ) = 0;
	virtual IWorldReference *GetWorldReference( int iIndex ) const = 0;
	virtual IWorldReference *FindWorldReferenceAndAddRef( SpawnGroupHandle_t hSpawnGroup ) = 0;
	virtual uint32 GetWorldCount( CThreadRWLock *lock = NULL ) = 0;
	virtual uint32 GetPendingWorldCount() = 0;
	virtual const char *GetWorldName( int iIndex ) const = 0;
	virtual IWorld *GetGeometryWorld( int iIndex ) = 0;
	virtual IWorld *GetGeometryWorld( IWorldReference *pWorldRef ) const = 0;
	virtual CWorldVisibility *GetWorldVisibility( int iIndex ) const = 0;

	// World group id
	virtual int FindLoadedWorldIndex( WorldGroupId_t hWorldGroupId, const char *pWorldName, CThreadRWLock *lock = NULL ) const = 0;
	virtual void UpdateObjectsForRendering( WorldGroupId_t hWorldGroupId, int nSkipFlags, const Vector &vCameraPos, float flLODScale, float flFarPlane, float flElapsedTime = 0.0f, const Vector *pCameraPos2 = NULL ) = 0;
	virtual bool IsFullyLoadedForPlayer( WorldGroupId_t hWorldGroupId, CSplitScreenSlot nSlot ) const = 0;
	virtual bool GetBoundsForWorld( WorldGroupId_t hWorldGroupId, Vector &vecMins, Vector &vecMaxs ) const = 0;
	virtual void UnkPutUtlContainer( WorldGroupId_t hWorldGroupId, void * ) = 0;

	virtual bool Unk_CollectWorldPairs( WorldGroupId_t hWorldGroupId, void *pOut ) = 0;
	virtual bool Unk_CollectWorldPairs( IWorldReference *pWorldRef, void *pOut ) = 0;

	virtual void FindEntitiesByTargetname( WorldGroupId_t hWorldGroupId, const char *pTargetname, const char *pSearchLump, CUtlVector< const CEntityKeyValues * > &res ) const = 0;

	// World reference
	// pWorldRef NULL uses the first loaded world
	virtual const CUtlVector< const CEntityKeyValues * > *GetEntityList( IWorldReference *pWorldRef, const char *pSearchLump = NULL ) const = 0;
	// pLumpName NULL or empty checks for any entity lump
	virtual bool IsEntityLumpLoaded( IWorldReference *pWorldRef, const char *pLumpName ) const = 0;
	// Same lookup as IsEntityLumpLoaded
	virtual bool unk030( IWorldReference *pWorldRef, const char *pLumpName ) const = 0;

	virtual void UnkMarkGeometryWorld( int nCompareFlags, int nCompareValue ) = 0;
	virtual void ServiceWorldRequests() = 0;
	virtual IWorld *FindWorld( WorldGroupId_t hWorldGroupId, const char *pWorldName ) = 0;

	// Load/Unload callback
	virtual void AddWorldLoadHandler( IWorldLoadUnloadCallback *pCallback ) = 0;
	virtual void RemoveWorldLoadHandler( IWorldLoadUnloadCallback *pCallback ) = 0;

	virtual int GetDeletedWorldCount() = 0;

	// VPK manager
	virtual void InstallWorldVPKOverrideManager( IWorldVPKOverrideManager *pVPKMgr ) = 0;
	virtual void RemoveWorldVPKOverrideManager( IWorldVPKOverrideManager *pVPKMgr ) = 0;

	// VPK override entries, each holding two strings
	virtual int UnkGetCount() = 0;
	virtual const char *UnkGetStringElm( int i ) const = 0;
	virtual const char *UnkGetString2Elm( int i ) const = 0;

	virtual void unk042( float flFrameTime, float flTime ) = 0;
	virtual void unk043( float flFrameTime, float flTime ) = 0;

	virtual void *unk044( void *p ) = 0;

	virtual void *unk045( float flRadius ) = 0;
	virtual void unk046( void *p ) = 0;
	virtual void unk047( void *p ) = 0;

	virtual void unk048( void *p ) = 0;

	virtual void unk049( void *p ) = 0;
	virtual void unk050( void *p ) = 0;
	virtual void unk051( void *p ) = 0;

	virtual void unk052( void *p ) = 0;
	virtual void unk053( WorldGroupId_t hWorldGroupId, void *pOut ) = 0;
	virtual void unk054() = 0;

	virtual void unk055( void *p, bool bAdd ) = 0;
	virtual bool unk056( void *p ) = 0;

	// Set, get and remove an entry keyed by a 64-bit value
	virtual void unk057( void *p ) = 0;
	virtual bool unk058( void *p ) = 0;
	virtual void unk059( void *p ) = 0;

	virtual IWorldVPKReference *unk060( const char *pVPKName ) = 0;
};

#endif // IWORLDRENDERERMGR_H
