//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Animation system: anim containers, activity and animation event
//          registries, animation groups and animgraph instances
//
//===========================================================================//

#ifndef IANIMATIONSYSTEM_H
#define IANIMATIONSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <tier0/platform.h>
#include <tier0/smartptr.h>

class CUtlBuffer;
class CUtlString;
class CAnimContainer;
class CModel;
class CTransform;
class CAnimationGroupResource;
class CAnimGraphDebugReplay;
class IAnimationHelper;

// Index of a registered animation event. The class has a constructor, which makes it
// a hidden return argument on Windows and a register return on Linux.
struct AnimationEventIndex_t
{
	AnimationEventIndex_t() : m_nIndex( -1 ) {}

	int m_nIndex;
};

// Returned by value the same way as AnimationEventIndex_t
struct AnimationEventIndexRange_t
{
	AnimationEventIndexRange_t() : m_nFirst( 0 ), m_nEnd( 0 ) {}

	int m_nFirst;
	// One past the highest registered event index
	int m_nEnd;
};

abstract_class IAnimationSystem : public IAppSystem
{
public:
	virtual CAnimContainer *Unk_CreateAnimContainer() = 0;
	// Destroys the anim container held by the model
	virtual void Unk_DestroyAnimContainer( CModel *pModel ) = 0;

	// Pose evaluation: accumulates the sequences of an animation decode description
	virtual void Unk_AddAnimationDecode( void *pDecodeDesc ) = 0;
	virtual void unk014( void *pUnknown1, uint32 nUnknown2, void *pUnknown3, void *pUnknown4, int nUnknown5, void *pUnknown6, int nUnknown7, void *pUnknown8, void *pUnknown9 ) = 0;

	// Forward to the sequence of the model's anim container; an out of range sequence falls back to the first one
	virtual float unk015( CModel *pModel, int nSequence, void *pUnknown ) = 0;
	virtual void *unk016( CModel *pModel, int nSequence, void *pUnknown ) = 0;
	virtual void *unk017( CModel *pModel, int nSequence, float flUnknown1, float flUnknown2, void *pUnknown3, void *pUnknown4, void *pUnknown5 ) = 0;
	virtual float Unk_GetSequenceDuration( CModel *pModel, int nSequence, void *pUnknown ) = 0;
	virtual void *unk019( CModel *pModel, int nSequence, void *pUnknown1, void *pUnknown2 ) = 0;
	virtual void *unk020( CModel *pModel, int nSequence, float flUnknown1, void *pUnknown2, void *pUnknown3 ) = 0;

	// Both write a transform to pOut and return false when there is nothing to compute
	virtual bool unk021( int nUnknown1, void *pUnknown2, void *pUnknown3, void *pUnknown4, CTransform *pOut ) = 0;
	virtual bool unk022( void *pUnknown1, int nUnknown2, void *pUnknown3, int nUnknown4, void *pUnknown5, CTransform *pOut ) = 0;

	virtual const char *unk023( CModel *pModel ) = 0;

	// Raw entity handle of the animdebugger target, -1 when there is none
	virtual void Unk_SetAnimDebuggerEntity( int nEntityHandle ) = 0;
	virtual int Unk_GetAnimDebuggerEntity() = 0;

	// Activity registry
	// Fails when the name is already registered as a private activity
	virtual bool RegisterSharedActivity( const char *pActivityName, int nActivityIndex ) = 0;
	// Returns the existing index when the name is already registered
	virtual int RegisterPrivateActivity( const char *pActivityName ) = 0;
	// Returns -1 for an unknown name
	virtual int GetActivityIndexForName( const char *pActivityName ) = 0;
	// Returns NULL for an unknown index
	virtual const char *GetActivityNameForIndex( int nActivityIndex ) = 0;
	virtual int GetHighestActivityIndex() = 0;

	virtual bool Unk_SaveAnimGraphModelBinding( void *pBinding, CUtlBuffer *pBuffer, CUtlString *pError ) = 0;

	virtual CSmartPtr< CAnimGraphDebugReplay > Unk_CreateAnimGraphDebugReplay() = 0;
	virtual CSmartPtr< CAnimGraphDebugReplay > Unk_LoadAnimGraphDebugReplay( const CUtlString &sFileName, CUtlString *pUnknown ) = 0;
	// Writes to DEFAULT_WRITE_PATH. The replay is passed by value
	virtual bool Unk_SaveAnimGraphDebugReplay( CSmartPtr< CAnimGraphDebugReplay > pReplay, const CUtlString &sFileName ) = 0;

	// Returns a static object
	virtual void *unk035() = 0;

	virtual void FrameUpdate() = 0;

	virtual CAnimationGroupResource *Unk_CreateAnimationGroupResource( void *pUnknown1, void *pUnknown2, const char *pName ) = 0;
	// Reference counting of an object whose counter sits right after its vtable
	virtual void *unk038( void *pObject ) = 0;
	virtual void unk039( void *pObject ) = 0;

	virtual void unk040( int *pUnknown1, int nUnknown2, void *pUnknown3, void *pUnknown4, int nUnknown5, void *pUnknown6, int nUnknown7, void *pUnknown8, void *pOut, uint8 *pUnknown10 ) = 0;

	// Global flex controller names; models map their local flex controllers onto these indices
	virtual int Unk_GetGlobalFlexControllerCount() = 0;
	// Returns an empty string for an index out of range
	virtual const char *Unk_GetGlobalFlexControllerName( int nIndex ) = 0;
	// Returns the index of the name, adding it when missing
	virtual int Unk_AddGlobalFlexController( const char *pName ) = 0;

	// Animation event registry
	// Fails when the name is already registered
	virtual bool RegisterSharedEvent( const char *pEventName, int nEventIndex, int nEventType ) = 0;
	virtual AnimationEventIndex_t RegisterPrivateEvent( const char *pEventName, int nEventType ) = 0;
	// The index is -1 for an unknown name
	virtual AnimationEventIndex_t GetEventIndexForName( const char *pEventName ) = 0;
	virtual void GetEventIndexAndTypeForName( const char *pEventName, int *pEventIndex, int *pEventType ) = 0;
	// Returns NULL for an unknown index
	virtual const char *GetEventNameForIndex( int nEventIndex ) = 0;
	// Returns 0 for an unknown index
	virtual int GetEventTypeForIndex( int nEventIndex ) = 0;
	virtual AnimationEventIndexRange_t GetEventIndexRange() = 0;

	// Finishes loading of an animation group resource
	virtual void *unk051( void *pLoadContext ) = 0;

	// Animgraph instances. NULL when the graph definition is not loaded or invalid
	virtual void *Unk_CreateAnimGraphInstance( void *pParams ) = 0;
	virtual void Unk_DestroyAnimGraphInstance( void *pInstance ) = 0;

	virtual void unk054( void *pUnknown ) = 0;
	virtual void unk054( void *pUnknown1, int *pUnknown2 ) = 0;

	// Sets a flag on the registered instance
	virtual void unk056( void *pInstance, bool bUnknown ) = 0;
	virtual void *unk057( void *pUnknown ) = 0;
};

abstract_class IAnimationSystemUtils
{
public:
	virtual IAnimationHelper *Unk_CreateAnimationHelper( void *pUnknown1, void *pUnknown2 ) = 0;
	virtual void Unk_DestroyAnimationHelper( IAnimationHelper *pHelper ) = 0;

	virtual ~IAnimationSystemUtils() {}
};

#endif // IANIMATIONSYSTEM_H
