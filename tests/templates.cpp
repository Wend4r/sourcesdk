#include <tier0/basetypes.h>
#include <tier0/bufferstring.h>
#include <tier0/circularbuffer.h>
#include <tier0/dynfunction.h>
#include <tier0/memblockallocator.h>
#include <tier0/mempool.h>
#include <tier0/memstack.h>
#include <tier0/platform.h>
#include <tier0/threadtools.h>
#include <tier0/tslist.h>
#include <tier0/utlblockvector.h>
#include <tier0/utlscratchmemory.h>
#include <tier0/utlstring.h>
#include <tier0/vprof.h>
#include <tier1/UtlSortVector.h>
#include <tier1/convar.h>
#include <tier1/fmtstr.h>
#include <tier1/functors.h>
#include <tier1/keyvalues3.h>
#include <tier1/rangecheckedvar.h>
#include <tier1/refcount.h>
#include <tier1/smartptr.h>
#include <tier1/tier1.h>
#include <tier1/utlbidirectionalset.h>
#include <tier1/utlcommon.h>
#include <tier1/utldelegate.h>
#include <tier1/utldict.h>
#include <tier1/utlenvelope.h>
#include <tier1/utlfixedmemory.h>
#include <tier1/utlflags.h>
#include <tier1/utlhandletable.h>
#include <tier1/utlhash.h>
#include <tier1/utlhashdict.h>
#include <tier1/utlhashtable.h>
#include <tier1/utlintrusivelist.h>
#include <tier1/utlleanvector.h>
#include <tier1/utllinkedlist.h>
#include <tier1/utlmap.h>
#include <tier1/utlmultilist.h>
#include <tier1/utlntree.h>
#include <tier1/utlpriorityqueue.h>
#include <tier1/utlqueue.h>
#include <tier1/utlrbtree.h>
#include <tier1/utlsoacontainer.h>
#include <tier1/utlstack.h>
#include <tier1/utlstringmap.h>
#include <tier1/utlsymbollarge.h>
#include <tier1/utltshash.h>
#include <tier1/utlvector.h>
#include <tier1/utlvectormemory.h>
#include <tier1/utlblockmemory.h>
#include <appframework/iappsystem.h>
#include <engine/eventdispatcher.h>
#include <entity2/entityinstance.h>
#include <entity2/entitykeyvalues.h>
#include <mathlib/vec3d.h>
#include <networksystem/netmessage.h>
#include <schemasystem/schematypes.h>
#include <bitvec.h>
#include <const.h>
#include <ehandle.h>
#include <iloopmode.h>
#include <variant.h>

#include "source2_steam_stats.pb.h"

#ifdef _MSC_VER
// Some containers declare a copy constructor without defining it to stay non-copyable
#pragma warning( disable : 4661 )
#endif

// Templates only compile when instantiated, so these instantiate the ones plugins use.
// An explicit instantiation compiles every member of a class, but not those of its base
// classes, so the bases are named too. CUtlLeanVectorImpl is instantiated with
// CUtlLeanVectorBase only, the fixed growable base has no external memory constructor.

// Bit vectors, with sizes that have a specialization, that do not and that are not a multiple of 32.
template class CBitVecT< CFixedBitVecBase< 32 > >;
template class CBitVecT< CFixedBitVecBase< 33 > >;
template class CBitVecT< CFixedBitVecBase< ABSOLUTE_PLAYER_LIMIT > >;
template class CBitVecT< CFixedBitVecBase< 256 > >;
template class CBitVecT< CFixedBitVecBase< MAX_EDICTS > >;
template class CFixedBitVecBase< 33 >;
template class CBitVec< MAX_EDICTS >;
template class CTypedBitVec< 64 >;
template class CBitVecT< CVarBitVecBase< unsigned short, 32 > >;
template class CVarBitVecBase< unsigned short, 32 >;
template class CVarBitVecT< unsigned short, 32 >;
template class CBitVecT< CVarBitVecBase< int, 64 > >;
template class CVarBitVecBase< int, 64 >;

// Console variables of every type TranslateConVarType has.
#define INSTANTIATE_CONVAR( T ) \
	template class CConVarRef< T >; \
	template class CConVar< T >;

// The CUtlString GetAs and SetAs are specializations.
#define INSTANTIATE_CONVAR_ACCESSORS( T ) \
	INSTANTIATE_CONVAR( T ) \
	template T ConVarRefAbstract::GetAs< T >( CSplitScreenSlot ) const; \
	template void ConVarRefAbstract::SetAs< T >( const T &, CSplitScreenSlot );

INSTANTIATE_CONVAR_ACCESSORS( bool )
INSTANTIATE_CONVAR_ACCESSORS( int16 )
INSTANTIATE_CONVAR_ACCESSORS( uint16 )
INSTANTIATE_CONVAR_ACCESSORS( int32 )
INSTANTIATE_CONVAR_ACCESSORS( uint32 )
INSTANTIATE_CONVAR_ACCESSORS( int64 )
INSTANTIATE_CONVAR_ACCESSORS( uint64 )
INSTANTIATE_CONVAR_ACCESSORS( float32 )
INSTANTIATE_CONVAR_ACCESSORS( float64 )
INSTANTIATE_CONVAR( CUtlString )
INSTANTIATE_CONVAR_ACCESSORS( Color )
INSTANTIATE_CONVAR_ACCESSORS( Vector2D )
INSTANTIATE_CONVAR_ACCESSORS( Vector )
INSTANTIATE_CONVAR_ACCESSORS( Vector4D )
INSTANTIATE_CONVAR_ACCESSORS( QAngle )
INSTANTIATE_CONVAR_ACCESSORS( VectorWS )

class CConCommandOwner
{
public:
	void Command( const CCommandContext &context, const CCommand &command );
	int Completion( const CCommand &command, CUtlVector< CUtlString > &completions );
};

template class CConCommandMemberAccessor< CConCommandOwner >;

// Thread primitives.
template class CInterlockedIntT< int >;
template class CInterlockedIntT< unsigned >;
template class CInterlockedPtr< CUtlString >;
template class CThreadTerminalMutex< CThreadNullMutex >;
template class CAutoLockT< CAtomicMutex >;
template class CAutoLockT< CThreadNullMutex >;
template class CMessageQueue< CUtlString >;

bool g_bMutexCondition;

template class CThreadConditionalMutex< CThreadNullMutex, &g_bMutexCondition >;

// Vectors and their allocators.
template class CUtlVectorBase< CUtlString >;
template class CUtlVectorMemory_Growable< CUtlString >;
template class CCopyableUtlVector< CUtlString >;
template class CUtlLeanVectorImpl< CUtlLeanVectorBase< CUtlString, int, CMemAllocAllocator >, CUtlString, int >;
template class CUtlLeanVectorBase< CUtlString, int, CMemAllocAllocator >;
template class CUtlLeanVectorFixedGrowable< CUtlString, 4, int, CMemAllocAllocator >;
template class CUtlLeanVectorFixedGrowableBase< CUtlString, 4, int, CMemAllocAllocator >;
template class CUtlVectorMemory_Aligned< CUtlString, int, 16 >;
template class CUtlVectorBase< CUtlString, int, CUtlVectorMemory_Aligned< CUtlString, int, 16 > >;
template class CUtlVectorAutoPurge< CUtlString * >;
template class CUtlVectorUltraConservative< CUtlString * >;
template class CUtlVectorMemory_Fixed< CUtlString, 4 >;
template class CUtlVectorMemory< CUtlString >;
template class CUtlVectorMemory_FixedGrowable< CUtlString, 4 >;
template class CUtlVectorMemory_Conservative< CUtlString >;
template class CUtlVectorMemory_RawAllocator< CUtlString, int, CMemAllocAllocator >;
template class CUtlBlockMemory< CUtlString, int >;
template class CUtlVectorFixed< CUtlString, 4 >;
template class CUtlVectorFixedGrowable< CUtlString, 4 >;
template class CUtlVectorConservative< CUtlString >;
template class CUtlBlockVector< CUtlString >;

// Maps, trees, lists and other containers.
template class CUtlOrderedMap< int, CUtlString >;
template class CUtlOrderedMap< CUtlString, CUtlString >;
template class CUtlOrderedMap< CUtlString, int >;
template class CUtlOrderedMap< const char *, CUtlString, CDefFastCaselessStringLess, uint16 >;
template class CUtlOrderedMap< CUtlSymbolLarge, CUtlString, CDefLess< CUtlSymbolLarge >, uint16 >;
template class CUtlRBTree< CUtlString, CDefLess< CUtlString > >;
template class CUtlDict< CUtlString >;
template class CUtlStringMap< CUtlString >;
template class CUtlOrderedMapBase< int, CUtlString, CDefLess< int > >;
template class CUtlOrderedMapBase< CUtlString, CUtlString, CDefLess< CUtlString > >;
template class CUtlOrderedMapBase< CUtlString, int, CDefLess< CUtlString > >;
template class CUtlOrderedMapBase< const char *, CUtlString, CDefFastCaselessStringLess, uint16 >;
template class CUtlOrderedMapBase< CUtlSymbolLarge, CUtlString, CDefLess< CUtlSymbolLarge >, uint16 >;
template class CUtlHashDict< CUtlString >;
template class CUtlHashtable< CUtlString, CUtlString >;
template class CUtlHashtable< CUtlString, empty_t >;
template class CUtlStableHashtable< CUtlString, CUtlString >;
template class CUtlLinkedList< CUtlString >;
template class CUtlLinkedList< CUtlString, unsigned short, true >;
template class CUtlFixedLinkedList< CUtlString >;
template class CUtlPtrLinkedList< CUtlString >;
template class CUtlNTree< CUtlString >;
template class CUtlMemoryStack< CUtlString, int, 1024 >;
template class CUtlQueue< CUtlString >;
template class CUtlQueueFixed< CUtlString, 4 >;
template class CUtlStack< CUtlString >;
template class CUtlPriorityQueue< CUtlString >;
template class CUtlTSHash< CUtlString, 16, uint >;
template class CUtlHash< CUtlString >;
template class CUtlMemoryPool< CUtlString >;
template class CUtlMemoryBlockAllocator< byte >;
template class CUtlMultiList< CUtlString, int >;
template class CUtlHashFixed< CUtlString, 16 >;
template class CUtlScalarHash< int >;
template class CUtlFixedMemory< CUtlString >;
template class CUtlEnvelope< CUtlString >;
template class CUtlKeyValuePair< CUtlString, CUtlString >;
template class CUtlKeyValuePair< CUtlString, empty_t >;
template class CUtlScratchMemoryPoolFixedGrowable< 1024 >;
template class CBidirectionalSet< int, int, int >;
template class CObjectPool< CUtlString >;
template class CFixedBudgetMemoryPool< 16, 16 >;
template class CFixedSizeCircularBuffer< CUtlString, 16 >;
template class CUtlFlags< uint8 >;
template class CUtlDelegate< bool( int, const CUtlString & ) >;
template class FastDelegate0<>;
template class FastDelegate1< int >;
template class FastDelegate2< int, const CUtlString &, bool >;
template class FastDelegate3< int, int, int >;
template class FastDelegate4< int, int, int, int >;
template class FastDelegate5< int, int, int, int, int >;
template class FastDelegate6< int, int, int, int, int, int >;
template class FastDelegate7< int, int, int, int, int, int, int >;
template class FastDelegate8< int, int, int, int, int, int, int, int >;

// CUtlHandle converts between objects and handles through their own members.
class CHandleObject
{
public:
	UtlHandle_t GetHandle() const { return UTLHANDLE_INVALID; }
	static CHandleObject *GetPtrFromHandle( UtlHandle_t h ) { return nullptr; }
	static bool IsHandleValid( UtlHandle_t h ) { return false; }
};

template class CUtlHandleTable< CHandleObject, 16 >;
template class CUtlHandle< CHandleObject >;

// The SDK has no allocator with the interface CAlignedMemPool takes.
class CPoolAllocator
{
public:
	void *Alloc( size_t nSize ) { return malloc( nSize ); }
	void Free( void *p ) { free( p ); }
};

template class CAlignedMemPool< 16, 16, 1024, CPoolAllocator >;

// A tree with other memory than the default.
class CRBTreeAllocator : public CMemAllocAllocator
{
};

template class CUtlRBTree< CUtlString, CDefLess< CUtlString >, int, CUtlLeanVector< UtlRBTreeNode_t< CUtlString, int >, int, CRBTreeAllocator > >;

// Intrusive lists link the nodes through their own members.
struct IntrusiveNode_t
{
	IntrusiveNode_t *m_pNext;
	IntrusiveNode_t *m_pPrev;
	const char *m_Name;
};

template class CUtlIntrusiveList< IntrusiveNode_t >;
template class CUtlIntrusiveDList< IntrusiveNode_t >;
template class CUtlIntrusiveDListWithTailPtr< IntrusiveNode_t >;
template class CUtlIntrusiveListWithTailPtr< IntrusiveNode_t >;

struct CUtlStringLess
{
	bool Less( const CUtlString &a, const CUtlString &b, void *pCtx ) { return a < b; }
};

template class CUtlSortVector< CUtlString, CUtlStringLess >;

template class CUtlSymbolTableLargeBase< false, 2048, CThreadNullMutex >;
template class CUtlSymbolTableLargeBase< true, 2048, CThreadNullMutex >;
template class CUtlSymbolTableLargeBase< false, 2048, CAtomicMutex >;
template class CUtlSymbolTableLargeBase< true, 2048, CAtomicMutex >;

// Strings, smart pointers, handles and other utilities.
template class CBufferStringN< MAX_PATH >;
template class CFmtStrN< 256 >;
template class CUtlConstStringBase< char >;
template class CHandle< CEntityInstance >;
template class CSmartPtr< CRefCounted<> >;
template class CTSQueue< CUtlString >;
template class CTSPool< CUtlString >;
template class CTSList< CUtlString >;
template class CTSListWithFreeList< CUtlString >;

struct TSSimpleListNode_t : TSLNodeBase_t
{
	int m_nValue;
};

template class CTSSimpleList< TSSimpleListNode_t >;

class CRefCountedBase1 {};
class CRefCountedBase2 {};
class CRefCountedBase3 {};
class CRefCountedBase4 {};
class CRefCountedBase5 {};

template class CRefCountServiceBase< CRefMT >;
template class CRefCountServiceBase< CRefST >;
template class CRefCounted<>;
template class CRefCounted< CRefCountServiceST >;
template class CRefCounted1< IRefCounted >;
template class CRefCounted2< CRefCountedBase1, CRefCountedBase2 >;
template class CRefCounted3< CRefCountedBase1, CRefCountedBase2, CRefCountedBase3 >;
template class CRefCounted4< CRefCountedBase1, CRefCountedBase2, CRefCountedBase3, CRefCountedBase4 >;
template class CRefCounted5< CRefCountedBase1, CRefCountedBase2, CRefCountedBase3, CRefCountedBase4, CRefCountedBase5 >;
template class CBaseAutoPtr< IRefCounted >;
template class CRefPtr< IRefCounted >;
template class CAutoRef<>;
template class CPlainAutoPtr< CUtlString >;
template class CArrayAutoPtr< CUtlString >;
template class CLateBoundPtr< CUtlString >;

struct IntHandleType_t;

template class CBaseIntHandle< uint16 >;
template class CBaseIntHandle< uint32 >;
template class CBaseIntHandle< uint64 >;
template class CIntHandle16< IntHandleType_t >;
template class CIntHandle32< IntHandleType_t >;
template class CRangeCheckedVar< int, 0, 10, 5 >;
template class CStridedPtr< float >;
template class CStridedConstPtr< float >;
template class Vec3D< float >;
template class Vec3D< int >;
template class CVariantBase< CVariantDefaultAllocator >;
template class VProfScopeHelper< 0, false >;
template class CEntityKeyComplex< CUtlString >;
template struct CEventDispatcher_Identified< CEventIDManager_Default >;
template struct CEventDispatcher< CEventIDManager_Default >;
template class CSchemaPtrMap< int, CUtlString * >;
template class CNetMessagePB< 0, CMsgSource2SystemSpecs >;
template struct CBaseCmdKeyValues< CMsgSource2SystemSpecs >;
template class CBaseAppSystem< IAppSystem >;
template class CTier0AppSystem< IAppSystem >;
template class CTier1AppSystem< IAppSystem >;
template class CDynamicFunction< void ( * )() >;
