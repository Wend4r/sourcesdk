//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Real-Time Hierarchical Profiling
//
// $NoKeywords: $
//=============================================================================//

#ifndef VPROF_H
#define VPROF_H

#include "tier0/dbg.h"
#include "tier0/fasttimer.h"
#include "tier0/l2cache.h"
#include "tier0/threadtools.h"

// VProf is enabled by default in all configurations -except- X360 Retail.
#if !( defined( _X360 ) && defined( _CERT ) )
#define VPROF_ENABLED
#endif

#if defined(_X360) && defined(VPROF_ENABLED)
#include "tier0/pmc360.h"
#ifndef USE_PIX
#define VPROF_UNDO_PIX
#undef _PIX_H_
#undef PIXBeginNamedEvent
#undef PIXEndNamedEvent
#undef PIXSetMarker
#undef PIXNameThread
#define USE_PIX
#include <pix.h>
#undef USE_PIX
#else
#include <pix.h>
#endif
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

// enable this to get detailed nodes beneath budget
// #define VPROF_LEVEL 1

// enable this to use pix (360 only)
// #define VPROF_PIX 1

#if defined(VPROF_PIX)
#pragma comment( lib, "Xapilibi" )
#endif

//-----------------------------------------------------------------------------
//
// Profiling instrumentation macros
//

#define MAXCOUNTERS 256


#ifdef VPROF_ENABLED

#define VPROF_VTUNE_GROUP

#define	VPROF( name )						VPROF_(name, 1, VPROF_BUDGETGROUP_OTHER_UNACCOUNTED, false, 0)
#define	VPROF_ASSERT_ACCOUNTED( name )		VPROF_(name, 1, VPROF_BUDGETGROUP_OTHER_UNACCOUNTED, true, 0)
#define	VPROF_( name, detail, group, bAssertAccounted, budgetFlags )		VPROF_##detail(name,group, bAssertAccounted, budgetFlags)

#define VPROF_BUDGET( name, group )					VPROF_BUDGET_FLAGS(name, group, BUDGETFLAG_OTHER)
#define VPROF_BUDGET_FLAGS( name, group, flags )	VPROF_(name, 0, group, false, flags)

#define VPROF_SCOPE_BEGIN( tag )	do { VPROF( tag )
#define VPROF_SCOPE_END()			} while (0)

#define VPROF_ONLY( expression )	expression

#define VPROF_ENTER_SCOPE( name )			VProf_EnterScopeAdHoc( name, { __FILE__, __LINE__, __func__ } )
#define VPROF_EXIT_SCOPE()					VProf_ExitScope()

#define VPROF_BUDGET_GROUP_ID_UNACCOUNTED 0


// Budgetgroup flags. These are used with VPROF_BUDGET_FLAGS.
// These control which budget panels the groups show up in.
// If a budget group uses VPROF_BUDGET, it gets the default 
// which is BUDGETFLAG_OTHER.
#define BUDGETFLAG_CLIENT	(1<<0)		// Shows up in the client panel.
#define BUDGETFLAG_SERVER	(1<<1)		// Shows up in the server panel.
#define BUDGETFLAG_OTHER	(1<<2)		// Unclassified (the client shows these but the dedicated server doesn't).
#define BUDGETFLAG_HIDDEN	(1<<15)
#define BUDGETFLAG_ALL		0xFFFF


// NOTE: You can use strings instead of these defines. . they are defined here and added
// in vprof.cpp so that they are always in the same order.
#define VPROF_BUDGETGROUP_OTHER_UNACCOUNTED			_T("Unaccounted")
#define VPROF_BUDGETGROUP_WORLD_RENDERING			_T("World Rendering")
#define VPROF_BUDGETGROUP_DISPLACEMENT_RENDERING	_T("Displacement_Rendering")
#define VPROF_BUDGETGROUP_GAME						_T("Game")
#define VPROF_BUDGETGROUP_NPCS						_T("NPCs")
#define VPROF_BUDGETGROUP_SERVER_ANIM				_T("Server Animation")
#define VPROF_BUDGETGROUP_PHYSICS					_T("Physics")
#define VPROF_BUDGETGROUP_STATICPROP_RENDERING		_T("Static_Prop_Rendering")
#define VPROF_BUDGETGROUP_MODEL_RENDERING			_T("Other_Model_Rendering")
#define VPROF_BUDGETGROUP_MODEL_FAST_PATH_RENDERING _T("Fast Path Model Rendering")
#define VPROF_BUDGETGROUP_BRUSHMODEL_RENDERING		_T("Brush_Model_Rendering")
#define VPROF_BUDGETGROUP_SHADOW_RENDERING			_T("Shadow_Rendering")
#define VPROF_BUDGETGROUP_DETAILPROP_RENDERING		_T("Detail_Prop_Rendering")
#define VPROF_BUDGETGROUP_PARTICLE_RENDERING		_T("Particle/Effect_Rendering")
#define VPROF_BUDGETGROUP_ROPES						_T("Ropes")
#define VPROF_BUDGETGROUP_DLIGHT_RENDERING			_T("Dynamic_Light_Rendering")
#define VPROF_BUDGETGROUP_OTHER_NETWORKING			_T("Networking")
#define VPROF_BUDGETGROUP_CLIENT_ANIMATION			_T("Client_Animation")
#define VPROF_BUDGETGROUP_OTHER_SOUND				_T("Sound")
#define VPROF_BUDGETGROUP_OTHER_VGUI				_T("VGUI")
#define VPROF_BUDGETGROUP_OTHER_FILESYSTEM			_T("FileSystem")
#define VPROF_BUDGETGROUP_PREDICTION				_T("Prediction")
#define VPROF_BUDGETGROUP_INTERPOLATION				_T("Interpolation")
#define VPROF_BUDGETGROUP_SWAP_BUFFERS				_T("Swap_Buffers")
#define VPROF_BUDGETGROUP_PLAYER					_T("Player")
#define VPROF_BUDGETGROUP_OCCLUSION					_T("Occlusion")
#define VPROF_BUDGETGROUP_OVERLAYS					_T("Overlays")
#define VPROF_BUDGETGROUP_TOOLS						_T("Tools")
#define VPROF_BUDGETGROUP_LIGHTCACHE				_T("Light_Cache")
#define VPROF_BUDGETGROUP_DISP_HULLTRACES			_T("Displacement_Hull_Traces")
#define VPROF_BUDGETGROUP_TEXTURE_CACHE				_T("Texture_Cache")
#define VPROF_BUDGETGROUP_PARTICLE_SIMULATION		_T("Particle Simulation")
#define VPROF_BUDGETGROUP_SHADOW_DEPTH_TEXTURING	_T("Flashlight Shadows")
#define VPROF_BUDGETGROUP_CLIENT_SIM				_T("Client Simulation") // think functions, tempents, etc.
#define VPROF_BUDGETGROUP_STEAM						_T("Steam") 
#define VPROF_BUDGETGROUP_CVAR_FIND					_T("Cvar_Find") 
#define VPROF_BUDGETGROUP_CLIENTLEAFSYSTEM			_T("ClientLeafSystem")
#define VPROF_BUDGETGROUP_JOBS_COROUTINES			_T("Jobs/Coroutines")
	
#ifdef _X360
// update flags
#define VPROF_UPDATE_BUDGET				0x01	// send budget data every frame
#define VPROF_UPDATE_TEXTURE_GLOBAL		0x02	// send global texture data every frame
#define VPROF_UPDATE_TEXTURE_PERFRAME	0x04	// send perframe texture data every frame
#endif

//-------------------------------------

#ifndef VPROF_LEVEL
#define VPROF_LEVEL 0
#endif

#define	VPROF_0(name,group,assertAccounted,budgetFlags)	VProfScopeHelper<0, assertAccounted> vprofHelper_(name, group, budgetFlags, { __FILE__, __LINE__, __func__ });

#if VPROF_LEVEL > 0 
#define	VPROF_1(name,group,assertAccounted,budgetFlags)	VProfScopeHelper<1, assertAccounted> vprofHelper_(name, group, budgetFlags, { __FILE__, __LINE__, __func__ });
#else
#define	VPROF_1(name,group,assertAccounted,budgetFlags)	((void)0)
#endif

#if VPROF_LEVEL > 1 
#define	VPROF_2(name,group,assertAccounted,budgetFlags)	VProfScopeHelper<2, assertAccounted> vprofHelper_(name, group, budgetFlags, { __FILE__, __LINE__, __func__ });
#else
#define	VPROF_2(name,group,assertAccounted,budgetFlags)	((void)0)
#endif

#if VPROF_LEVEL > 2 
#define	VPROF_3(name,group,assertAccounted,budgetFlags)	VProfScopeHelper<3, assertAccounted> vprofHelper_(name, group, budgetFlags, { __FILE__, __LINE__, __func__ });
#else
#define	VPROF_3(name,group,assertAccounted,budgetFlags)	((void)0)
#endif

#if VPROF_LEVEL > 3 
#define	VPROF_4(name,group,assertAccounted,budgetFlags)	VProfScopeHelper<4, assertAccounted> vprofHelper_(name, group, budgetFlags, { __FILE__, __LINE__, __func__ });
#else
#define	VPROF_4(name,group,assertAccounted,budgetFlags)	((void)0)
#endif

//-------------------------------------

#ifdef _MSC_VER
#define VProfCode( code ) \
	if ( 0 ) \
		; \
	else \
	{ \
	VPROF( __FUNCTION__ ": " #code ); \
		code; \
	}
#else
#define VProfCode( code ) \
	if ( 0 ) \
		; \
	else \
	{ \
		VPROF( #code ); \
		code; \
	} 
#endif


//-------------------------------------

#define VPROF_INCREMENT_COUNTER(name,amount)			do { static CVProfCounter _counter( name ); _counter.Increment( amount ); } while( 0 )
#define VPROF_INCREMENT_GROUP_COUNTER(name,group,amount)			do { static CVProfCounter _counter( name, group ); _counter.Increment( amount ); } while( 0 )

#else

#define	VPROF( name )									((void)0)
#define	VPROF_ASSERT_ACCOUNTED( name )					((void)0)
#define	VPROF_( name, detail, group, bAssertAccounted, budgetFlags )	((void)0)
#define VPROF_BUDGET( name, group )						((void)0)
#define VPROF_BUDGET_FLAGS( name, group, flags )		((void)0)

#define VPROF_SCOPE_BEGIN( tag )	do {
#define VPROF_SCOPE_END()			} while (0)

#define VPROF_ONLY( expression )	((void)0)

#define VPROF_ENTER_SCOPE( name )
#define VPROF_EXIT_SCOPE()

#define VPROF_INCREMENT_COUNTER(name,amount)			((void)0)
#define VPROF_INCREMENT_GROUP_COUNTER(name,group,amount)	((void)0)

#define VPROF_TEST_SPIKE( msec )	((void)0)

#define VProfCode( code ) code

#endif
 
//-----------------------------------------------------------------------------

#ifdef VPROF_ENABLED

template < class T, typename I, size_t N > class CUtlVectorMemory_Growable;
template < class T, typename I, class A > class CUtlVector;

class CUtlSourceLocation
{
public:
	CUtlSourceLocation( const char *pFileName, uint64 nLine, const char *pFunctionName ) :
		m_pszFileName( pFileName ), m_nLine( nLine ), m_pszFunctionName( pFunctionName )
	{}

	const char *m_pszFileName;
	uint64 m_nLine;
	const char *m_pszFunctionName;
};

struct VProfBudgetGroupCallSite
{
	VProfBudgetGroupCallSite( const char *pGroupName, int nFlags, int nGroupId = VPROF_BUDGET_GROUP_ID_UNACCOUNTED ) :
		m_pszGroupName( pGroupName ), m_nFlags( nFlags ), m_nGroupID( nGroupId )
	{}

	const char *m_pszGroupName;
	int m_nFlags;
	int m_nGroupID;
};

// One entry is selected with CVProfNode::m_nBudgetGroupID. The table pointer and
// entry count are private globals in tier0.
struct VProfBudgetGroupEntry
{
	const char *m_pszGroupName;
	int m_nFlags;

private:
	uint8 m_pad000C[4092];
};

struct VProfReportSettings_t;
struct CVProfSummingContext;
struct VProfDataReportItem_t;
struct CVProfLayoutVerifier;

COMPILE_TIME_ASSERT( sizeof( CUtlSourceLocation ) == 24 );
COMPILE_TIME_ASSERT( sizeof( VProfBudgetGroupCallSite ) == 16 );
COMPILE_TIME_ASSERT( sizeof( VProfBudgetGroupEntry ) == 4104 );

// Enters a scope of the Unaccounted budget group, on the profiled thread only
PLATFORM_INTERFACE void VProf_EnterScopeAdHoc( const char *pszName, CUtlSourceLocation location );
PLATFORM_INTERFACE void VProf_ExitScope();

typedef void (*VProfExitScopeCB)();

//-----------------------------------------------------------------------------
//
// A node in the call graph hierarchy
//

class PLATFORM_CLASS CVProfNode 
{
	friend class CVProfRecorder;
	friend class CVProfile;
	friend struct CVProfLayoutVerifier;

public:
	CVProfNode( const char *pszName, CVProfNode *pParent, VProfBudgetGroupCallSite &pBudgetGroupName, const CUtlSourceLocation &location );
	~CVProfNode();
	
	CVProfNode &operator=( const CVProfNode & );

	CVProfNode *GetVParent() { return m_pParent; }
	const CVProfNode *GetVParent() const { return m_pParent; }
	CVProfNode *GetVSibling() { return m_pSibling; }
	const CVProfNode *GetVPrevSibling() const;
	const CVProfNode *GetVSibling() const { return m_pSibling; }
	CVProfNode *GetVChild() { return m_pChild; }
	const CVProfNode *GetVChild() const { return m_pChild; }
	
	void MarkFrame();
	void ResetPeak();
	
	void Pause();
	void Resume();
	void Reset();

	void EnterScope();
	bool ExitScope();

	CVProfNode *FindOrCreateChild( const char *, VProfBudgetGroupCallSite &, const CUtlSourceLocation & );

	const char *GetName() const { return m_pszName; }

	// Only used by the record/playback stuff.
	void SetBudgetGroupID( int id ) { m_nBudgetGroupID = id; }
	int GetBudgetGroupID() const { return m_nBudgetGroupID; }

	// Times are in milliseconds
	int	GetCurCalls() const { return m_nCurFrameCalls; }
	double GetCurTime() const { return CyclesToMilliseconds( m_nCurFrameTime ); }
	int GetPrevCalls() const { return m_nPrevFrameCalls; }
	double GetPrevTime() const { return CyclesToMilliseconds( m_nPrevFrameTime ); }
	int	GetTotalCalls() const { return m_nTotalCalls; }
	double GetTotalTime() const { return CyclesToMilliseconds( m_nTotalTime ); }
	double GetPeakTime() const { return CyclesToMilliseconds( m_nPeakTime ); }

	const CUtlSourceLocation &GetSourceLocation() const { return m_SourceLocation; }

	double GetCurTimeLessChildren() const;
	double GetPrevTimeLessChildren() const;
	double GetTotalTimeLessChildren() const;

	void ClearPrevTime() { m_nPrevFrameTime = 0; m_nUnknown0040 = 0; }

	// Not used in the common case...
	void SetCurFrameTime( unsigned long milliseconds );
	
	void SetClientData( int iClientData ) { m_iClientData = iClientData; }
	int GetClientData() const { return m_iClientData; }

// Used by vprof record/playback.
protected:
	CVProfNode( const char *, VProfBudgetGroupCallSite &, double, const CUtlSourceLocation & );

	void SetUniqueNodeID( int id ) { m_iUniqueNodeID = id; }
	int GetUniqueNodeID() const { return m_iUniqueNodeID; }

	static int s_iCurrentUniqueNodeID;

private:
	static double CyclesToMilliseconds( uint64 nCycles ) { return nCycles * ( 1000.0 / Plat_CPUTickFrequency() ); }

public:
	const char *m_pszName;
	uint64 m_nTimer;

private:
	// The first is zeroed whenever the timer starts, tier0 never reads either
	uint8 m_pad0010[8];

public:
	int m_nRecursions;
	unsigned m_nCurFrameCalls;
	uint64 m_nCurFrameTime;
	// tier0 only zeroes these or copies them along with the time before them
	uint64 m_nUnknown0028;
	unsigned m_nPrevFrameCalls;

private:
	uint8 m_pad0034[4];

public:
	uint64 m_nPrevFrameTime;
	uint64 m_nUnknown0040;
	unsigned m_nTotalCalls;

private:
	uint8 m_pad004C[4];

public:
	uint64 m_nTotalTime;
	uint64 m_nUnknown0058;
	uint64 m_nPeakTime;
	uint64 m_nUnknown0068;
	unsigned m_nMergedCurFrameCalls;

private:
	uint8 m_pad0074[4];

public:
	uint64 m_nMergedCurFrameTime;
	CVProfNode *m_pParent;
	CVProfNode *m_pChild;
	CVProfNode *m_pSibling;
	int m_nBudgetGroupID;
	int m_iClientData;
	int m_iUniqueNodeID;

private:
	uint8 m_pad0094[4];

public:
	CUtlSourceLocation m_SourceLocation;
};

inline const CVProfNode *CVProfNode::GetVPrevSibling() const
{
	if ( !m_pParent )
		return NULL;

	const CVProfNode *pNode = m_pParent->m_pChild;
	while ( pNode && pNode->m_pSibling != this )
		pNode = pNode->m_pSibling;

	return pNode;
}

inline double CVProfNode::GetCurTimeLessChildren() const
{
	double result = GetCurTime();
	for ( const CVProfNode *pChild = m_pChild; pChild; pChild = pChild->m_pSibling )
		result -= pChild->GetCurTime();

	return result;
}

inline double CVProfNode::GetPrevTimeLessChildren() const
{
	double result = GetPrevTime();
	for ( const CVProfNode *pChild = m_pChild; pChild; pChild = pChild->m_pSibling )
		result -= pChild->GetPrevTime();

	return result;
}

inline double CVProfNode::GetTotalTimeLessChildren() const
{
	double result = GetTotalTime();
	for ( const CVProfNode *pChild = m_pChild; pChild; pChild = pChild->m_pSibling )
		result -= pChild->GetTotalTime();

	return result;
}

//-----------------------------------------------------------------------------
//
// Coordinator and root node of the profile hierarchy tree
//

enum VProfReportType_t
{
	VPRT_SUMMARY									= ( 1 << 0 ),
	VPRT_HIERARCHY									= ( 1 << 1 ),
	VPRT_HIERARCHY_TIME_PER_FRAME_AND_COUNT_ONLY	= ( 1 << 2 ),
	VPRT_LIST_BY_TIME								= ( 1 << 3 ),
	VPRT_LIST_BY_TIME_LESS_CHILDREN					= ( 1 << 4 ),
	VPRT_LIST_BY_AVG_TIME							= ( 1 << 5 ),	
	VPRT_LIST_BY_AVG_TIME_LESS_CHILDREN				= ( 1 << 6 ),
	VPRT_LIST_BY_PEAK_TIME							= ( 1 << 7 ),
	VPRT_LIST_BY_PEAK_OVER_AVERAGE					= ( 1 << 8 ),
	VPRT_LIST_TOP_ITEMS_ONLY						= ( 1 << 9 ),

	VPRT_FULL = (0xffffffff & ~(VPRT_HIERARCHY_TIME_PER_FRAME_AND_COUNT_ONLY|VPRT_LIST_TOP_ITEMS_ONLY)),
};

enum CounterGroup_t
{
	COUNTER_GROUP_DEFAULT=0,
	COUNTER_GROUP_NO_RESET,				// The engine doesn't reset these counters. Usually, they are used 
										// like global variables that can be accessed across modules.
	COUNTER_GROUP_TEXTURE_GLOBAL,		// Global texture usage counters (totals for what is currently in memory).
	COUNTER_GROUP_TEXTURE_PER_FRAME		// Per-frame texture usage counters.
}; 

class PLATFORM_CLASS CVProfile 
{
	friend struct CVProfLayoutVerifier;

public:
	CVProfile();
	~CVProfile();

	int AssignNextTimespanId();
	
	void Start();
	void Stop();
	void Term();

	void SetTargetThreadId( unsigned id );
	uint32 GetTargetThreadId();
	bool InTargetThread();

	void CalcBudgetGroupTimes_Recursive( CVProfNode *, unsigned int *, int, float );

	void EnterScopeTargetThread( const char *pszName, bool bAssertAccounted, VProfBudgetGroupCallSite &pBudgetGroupName, const CUtlSourceLocation &location );
	void EnterScopeTargetThreadChecked( const char *pszName, bool bAssertAccounted, VProfBudgetGroupCallSite &pBudgetGroupName, const CUtlSourceLocation &location );
	void EnterScopeNodeTargetThread( CVProfNode *node );
	void ExitScopeTargetThread();
	void ExitScopeTargetThreadChecked();

	static VProfExitScopeCB EnterScopeBackgroundThread( const char *pszName, VProfBudgetGroupCallSite &budgetGroup, const CUtlSourceLocation &location );
	static void ExitScopeBackgroundThread();
	CVProfNode *GetBackgroundRoot();
	void MergeBackgroundTrees();
	void SetProfileAllThreads( bool bProfileAllThreads );

	void MarkFrame();
	void ResetPeaks();
	
	void Pause();
	void Resume();
	void Reset();
	
	bool IsEnabled() const;

	bool AtRoot() const;

	//
	// Queries
	//

#ifdef VPROF_VTUNE_GROUP
#	define MAX_GROUP_STACK_DEPTH 1024

	void EnableVTuneGroup( const char *pGroupName );
	void DisableVTuneGroup( void );
	
	void PushGroup( int nGroupID );
	void PopGroup( void );
#endif
	
	int NumFramesSampled() const;
	double GetPeakFrameTime() const;
	double GetTotalTimeSampled() const;
	double GetTimeLastFrame() const;
	double GetTotalSecondsSampled() const;
	double GetTotalWallClockSecondsSampled();
	
	void SetOutputStream( void (*)(const char *, ...) );

	CVProfNode *GetRoot();
	CVProfNode *FindNode( CVProfNode *pStartNode, const char *pszNode );

	void OutputReport( const VProfReportSettings_t &, const char *pszStartNode, int budgetGroupID = -1 );
	void OutputReport( const char *pszStartNode = NULL, int budgetGroupID = -1 );
	void GenerateDataReport( CUtlVector< VProfDataReportItem_t, int, CUtlVectorMemory_Growable< VProfDataReportItem_t, int, 0 > > *pReport );

	static int GetNumBudgetGroups( void );
	static const char *GetBudgetGroupName( int budgetGroupID );
	static int GetBudgetGroupFlags( int budgetGroupID );	// Returns a combination of BUDGETFLAG_ defines.
	static void GetBudgetGroupColor( int budgetGroupID, int &r, int &g, int &b, int &a );

	static int BudgetGroupNameToBudgetGroupID( const char *pBudgetGroupName );
	static int BudgetGroupNameToBudgetGroupID( const char *pBudgetGroupName, int budgetFlagsToORIn );
	static int BudgetGroupNameToBudgetGroupIDNoCreate( const char *pBudgetGroupName );

	void HideBudgetGroup( int budgetGroupID, bool bHide = true );
	void HideBudgetGroup( const char *pszName, bool bHide = true );

	uint64 *FindOrCreateCounter( const char *pName, CounterGroup_t eCounterGroup = COUNTER_GROUP_DEFAULT );
	void ResetCounters( CounterGroup_t eCounterGroup );
	
	int GetNumCounters( void ) const;
	
	uint64 GetCounterValue( int index ) const;
	const char *GetCounterName( int index ) const;
	const char *GetCounterNameAndValue( int index, uint64 &val ) const;
	CounterGroup_t GetCounterGroup( int index ) const;

	CVProfNode *GetCurrentNode();

protected:
	void FreeNodes_R( CVProfNode *pNode );

#ifdef VPROF_VTUNE_GROUP
	bool VTuneGroupEnabled();
	int VTuneGroupID();
#endif

	void SumTimes( const char *pszStartNode, int budgetGroupID, CVProfSummingContext & );
	void SumTimes( const CVProfNode *pNode, int budgetGroupID, CVProfSummingContext & );
	void DumpNodes( const VProfReportSettings_t &, const CVProfNode *pNode, int indent, bool bAverageAndCountOnly, const CVProfSummingContext &, double, double, bool );
	static int FindBudgetGroupName( const char *pBudgetGroupName );
	static int AddBudgetGroupName( const char *pBudgetGroupName, int budgetFlags );

public:
	bool m_bVTuneGroupEnabled;

private:
	uint8 m_pad0001[3];

public:
	int m_nVTuneGroupID;
	int m_nGroupIDStack[MAX_GROUP_STACK_DEPTH];
	int m_nGroupIDStackDepth;
	int m_nEnabled;
	CVProfNode m_Root;

private:
	// Points at m_Root after construction
	uint8 m_pad10D0[8];

public:
	CVProfNode *m_pCurNode;
	bool m_bAtRoot;

private:
	uint8 m_pad10E1[7];

public:
	int m_nFrames;
	int m_nPausedEnabledDepth;

	// Wall clock seconds
	double m_flStartTime;
	double m_flStopTime;
	double m_flPauseStartTime;
	double m_flTotalPauseTime;

	uint64 m_nCounters[MAXCOUNTERS];
	char m_eCounterGroups[MAXCOUNTERS]; // (These are CounterGroup_t's).
	const char *m_pszCounterNames[MAXCOUNTERS];
	int m_nCounterCount;

private:
	uint8 m_pad2214[4];

public:
	uint8 m_CounterMutex[16];

private:
	// Three arrays of 256 zeroed 24-byte elements, each followed by an int
	uint8 m_pad2228[6152];
	uint8 m_pad3A30[6152];
	uint8 m_pad5238[6148];

public:
	int m_nNextTimespanId;
	uint64 m_nTargetThreadID;
	bool m_bProfileAllThreads;

private:
	uint8 m_pad6A49[7];

public:
	CVProfNode *m_pBackgroundRoot;

private:
	// Background thread roots
	uint8 m_pad6A58[24];

public:
	uint8 m_BackgroundRootMutex[16];
	void ( *m_pOutputStream )( const char *, ... );
};

COMPILE_TIME_ASSERT( sizeof( CVProfNode ) == 192 );
COMPILE_TIME_ASSERT( sizeof( CVProfile ) == 27272 );

//-------------------------------------

PLATFORM_INTERFACE CVProfile g_VProfCurrentProfile;
PLATFORM_INTERFACE LoggingChannelID_t LOG_VPROF;


//-----------------------------------------------------------------------------

PLATFORM_INTERFACE bool g_VProfSignalSpike;
PLATFORM_INTERFACE bool g_VProfNodeSpikeReporting;
PLATFORM_INTERFACE float g_VProfNodeSpikeMinimumMs;
PLATFORM_INTERFACE float g_VProfNodeSpikeMultiplier;

class CVProfSpikeDetector
{
public:
	CVProfSpikeDetector( float spike ) :
		m_timeLast( GetTimeLast() )
	{
		m_spike = spike;
		m_Timer.Start();
	}

	~CVProfSpikeDetector()
	{
		m_Timer.End();
		if ( Plat_FloatTime() - m_timeLast > 2.0 )
		{
			m_timeLast = Plat_FloatTime();
			if ( m_Timer.GetDuration().GetMillisecondsF() > m_spike )
			{
				g_VProfSignalSpike = true;
			}
		}
	}

private:
	static float &GetTimeLast() { static float timeLast = 0; return timeLast; }
	CFastTimer	m_Timer;
	float m_spike;
	float &m_timeLast;
};


// Macro to signal a local spike. Meant as temporary instrumentation, do not leave in code
#define VPROF_TEST_SPIKE( msec ) CVProfSpikeDetector UNIQUE_ID( msec )

//-----------------------------------------------------------------------------

template < int DETAIL_LEVEL, bool ASSERT_ACCOUNTED >
class VProfScopeHelper
{
public:
	VProfScopeHelper( const char *pszName, const CUtlSourceLocation &location )
	{
		m_pExitScope = EnterScopeInternal( pszName, location );
	}

	VProfScopeHelper( const char *pszName, const char *pszGroupName, int nFlags, const CUtlSourceLocation &location )
	{
		VProfBudgetGroupCallSite budgetGroup( pszGroupName, nFlags );
		m_pExitScope = EnterScopeInternalBudgetFlags( pszName, budgetGroup, location );
	}

	VProfScopeHelper( VProfScopeHelper &other )
	{
		m_pExitScope = other.m_pExitScope;
	}

	~VProfScopeHelper()
	{
		ExitScope();
	}

	void ExitScope()
	{
		if ( !m_pExitScope )
			return;

		m_pExitScope();
		m_pExitScope = nullptr;
	}

	DLL_CLASS_IMPORT VProfScopeHelper &operator=( const VProfScopeHelper &other );
	DLL_CLASS_IMPORT VProfScopeHelper &operator=( VProfScopeHelper &&other );

	static DLL_CLASS_IMPORT VProfExitScopeCB EnterScopeInternal( const char *pszName, const CUtlSourceLocation &location );
	static DLL_CLASS_IMPORT VProfExitScopeCB EnterScopeInternalBudgetFlags( const char *pszName, VProfBudgetGroupCallSite &budgetGroup, const CUtlSourceLocation &location );

private:
	VProfExitScopeCB m_pExitScope = nullptr;
};

//-----------------------------------------------------------------------------

class CVProfCounter
{
public:
	CVProfCounter( const char *pName, CounterGroup_t group = COUNTER_GROUP_DEFAULT )
	{
		m_pCounter = g_VProfCurrentProfile.FindOrCreateCounter( pName, group );
		Assert( m_pCounter );
	}
	~CVProfCounter()
	{
	}
	void Increment( uint64 val )
	{ 
		Assert( m_pCounter );
		*m_pCounter += val; 
	}
private:
	uint64 *m_pCounter;
};

#endif

#ifdef _X360

#include "xbox/xbox_console.h"
#include "tracerecording.h"
#include  "tier1/fmtstr.h"
#pragma comment( lib, "tracerecording.lib" )
#pragma comment( lib, "xbdm.lib" )

class CPIXRecorder
{
public:
	CPIXRecorder() : m_bActive( false ) {}
	~CPIXRecorder() { Stop(); }

	void Start( const char *pszFilename = "capture" )
	{
		if ( !m_bActive )
		{
			if ( !XTraceStartRecording( CFmtStr( "e:\\%s.pix2", pszFilename ) ) )
			{
				Msg( "XTraceStartRecording failed, error code %d\n", GetLastError() );
			}
			else
			{
				m_bActive = true;
			}
		}
	}

	void Stop()
	{
		if ( m_bActive )
		{
			m_bActive = false;
			if ( XTraceStopRecording() )
			{
				Msg( "CPU trace finished.\n" );
				// signal VXConsole that trace is completed
				XBX_rTraceComplete();
			}
		}
	}

private:
	bool m_bActive;
};

#define VPROF_BEGIN_PIX_BLOCK( convar ) \
	{ \
	bool bRunPix = 0; \
	static CFastTimer PIXTimer; \
	extern ConVar convar; \
	ConVar &PIXConvar = convar; \
	CPIXRecorder PIXRecorder; \
		{ \
		PIXLabel: \
			if ( bRunPix ) \
			{ \
				PIXRecorder.Start(); \
			} \
			else \
			{ \
				if ( PIXConvar.GetBool() ) \
				{ \
					PIXTimer.Start(); \
				} \
			} \
				{


#define VPROF_END_PIX_BLOCK() \
				} \
			\
			if ( !bRunPix ) \
			{ \
				if ( PIXConvar.GetBool() ) \
				{ \
					PIXTimer.End(); \
					if ( PIXTimer.GetDuration().GetMillisecondsF() > PIXConvar.GetFloat() ) \
					{ \
						PIXConvar.SetValue( 0 ); \
						bRunPix = true; \
						goto PIXLabel; \
					} \
				} \
			} \
			else \
			{ \
				PIXRecorder.Stop(); \
			} \
		} \
	}
#else
#define VPROF_BEGIN_PIX_BLOCK( PIXConvar ) {
#define VPROF_END_PIX_BLOCK() }
#endif


#ifdef VPROF_UNDO_PIX
#undef USE_PIX
#undef _PIX_H_
#undef PIXBeginNamedEvent
#undef PIXEndNamedEvent
#undef PIXSetMarker
#undef PIXNameThread
#include <pix.h>
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif

//=============================================================================
