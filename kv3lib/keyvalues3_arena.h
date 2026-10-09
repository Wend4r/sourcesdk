#ifndef KEYVALUES3_ARENA_H
#define KEYVALUES3_ARENA_H

#ifdef _WIN32
#pragma once
#endif

#include "kv3lib/keyvalues3.h"
#include "keyvalues3_helpers.h"
#include "keyvalues3_cluster.h"

#include "tier0/memdbgon.h"

class CKV3ArenaBase
{
public:
	CKV3ArenaBase( CKV3Arena* context );
	~CKV3ArenaBase() { Purge(); }

	const char* LookupString( UtlSymLargeId_t symid ) { return m_Symbols.String( symid ); }
	const char *AllocString( const char *pString, UtlSymLargeId_t *pSymLargeId = nullptr )
	{
		UtlSymLargeId_t id = m_Symbols.Add( pString );

		if ( pSymLargeId )
		{
			*pSymLargeId = id;
		}

		return LookupString( id );
	}

	void Clear();
	void Purge();

protected:
	template <typename CLUSTER>
	struct ClusterNodeChain
	{
		ClusterNodeChain() : m_pTail( nullptr ), m_pHead( nullptr )
		{}

		void Reset()
		{
			m_pTail = nullptr;
			m_pHead = nullptr;
		}

		void AddToChain( CLUSTER *cluster );
		void RemoveFromChain( CLUSTER *cluster );

		CLUSTER *m_pTail;
		CLUSTER *m_pHead;
	};

	template <typename NODE>
	class NodeList
	{
	public:
		struct ListEntry
		{
			ListEntry() : m_pNext( nullptr )
			{
			}

			ListEntry *m_pNext;
			NODE m_Value;
		};

		NodeList() : m_nUsedBytes( 0 ), m_nAllocatedBytes( 0 ), m_pData( nullptr )
		{}

		~NodeList() { Free(); }

		NODE *Alloc( int initial_size );
		void Free() { Purge(); }
		void Purge();
		void Clear();

		int UsedBytes() const { return m_nUsedBytes; }
		int AllocatedBytes() const { return m_nAllocatedBytes; }
		int FreeBytes() const { return m_nAllocatedBytes - m_nUsedBytes; }

		bool IsFull() const { return m_nUsedBytes >= m_nAllocatedBytes; }

		ListEntry *Head() { return &m_pData[0]; }
		ListEntry *Tail() { return reinterpret_cast<ListEntry *>((uint8 *)Head() + m_nUsedBytes); }

		bool IsWithinRange( NODE *element ) { return AllocatedBytes() > 0 && element >= (void *)Head() && element < (void *)Tail(); }

	private:
		void EnsureByteSize( int bytes_needed );

	private:
		int m_nUsedBytes;
		int m_nAllocatedBytes;
		ListEntry *m_pData;
	};

	CKV3Arena* m_pContext;
	CUtlBuffer m_BinaryData;

	CKeyValues3Cluster m_KV3BaseCluster;

	ClusterNodeChain<CKeyValues3Cluster> m_KV3PartialClusters;
	ClusterNodeChain<CKeyValues3Cluster> m_KV3FullClusters;

	ClusterNodeChain<CKeyValues3ArrayCluster> m_PartialArrayClusters;
	ClusterNodeChain<CKeyValues3ArrayCluster> m_FullArrayClusters;
	NodeList<CKeyValues3Array> m_RawArrayEntries;

	ClusterNodeChain<CKeyValues3TableCluster> m_PartialTableClusters;
	ClusterNodeChain<CKeyValues3TableCluster> m_FullTableClusters;
	NodeList<CKeyValues3Table> m_RawTableEntries;

	CUtlSymbolTableLarge m_Symbols;

	bool m_bMetaDataEnabled: 1;
	bool m_bFormatConverted: 1;
	bool m_bRootAvailabe: 1;

	IErrorListener* m_pParsingErrorListener;

	friend class KeyValues3;
};

class CKV3ArenaImpl : public CKV3ArenaBase
{
	typedef CKV3ArenaBase BaseClass;

public:
	CKV3ArenaImpl( CKV3Arena *context, bool bNoRoot );
	~CKV3ArenaImpl() { Purge(); }

	KeyValues3* AllocKV( KV3TypeEx_t type = KV3_TYPEEX_NULL, KV3SubType_t subtype = KV3_SUBTYPE_UNSPECIFIED );
	// WARNING: kv must belong to this context!!!
	void FreeKV( KeyValues3* kv );

	// gets the pre-allocated kv if we indicated its existence when creating the context
	KeyValues3* Root();

	bool IsMetaDataEnabled() const { return m_bMetaDataEnabled; }
	// returns true if the desired format was converted to another after loading via LoadKV3*
	bool IsFormatConverted() const { return m_bFormatConverted; }
	bool IsRootAvailabe() const { return m_bRootAvailabe; }

	// filled in after loading via LoadKV3* in binary encoding
	CUtlBuffer& GetBinaryData() { return m_BinaryData; }

	IErrorListener* GetParsingErrorListener() const { return m_pParsingErrorListener; }
	void SetParsingErrorListener( IErrorListener* listener ) { m_pParsingErrorListener = listener; }

	void EnableMetaData( bool bEnable );
	void CopyMetaData( KV3MetaData_t* pDest, const KV3MetaData_t* pSrc );

	void Clear();
	void Purge();

	template <typename CLUSTER>
	void ClearClusterNodeChain( ClusterNodeChain<CLUSTER> &cluster_node );
	template <typename CLUSTER>
	void PurgeClusterNodeChain( ClusterNodeChain<CLUSTER> &cluster_node );

	bool IsArrayAllocated( CKeyValues3Array *element ) { return m_RawArrayEntries.IsWithinRange( element ); }
	bool IsTableAllocated( CKeyValues3Table *element ) { return m_RawTableEntries.IsWithinRange( element ); }

private:
	template <typename CLUSTER>
	void MoveToPartial( ClusterNodeChain<CLUSTER> &full_cluster, ClusterNodeChain<CLUSTER> &partial_cluster );

	template <typename CLUSTER, typename... Args, typename = typename std::enable_if_t<std::is_constructible_v<typename CLUSTER::NodeType, Args...>, int>>
	auto Alloc( ClusterNodeChain<CLUSTER> &partial_clusters, ClusterNodeChain<CLUSTER> &full_clusters, int initial_size, Args&&... args );

	template <typename CLUSTER, typename NODE, typename... Args, typename = typename std::enable_if_t<std::is_constructible_v<typename CLUSTER::NodeType, Args...>, int>>
	NODE *RawAlloc( NodeList<NODE> &raw_array, ClusterNodeChain<CLUSTER> &partial_clusters, ClusterNodeChain<CLUSTER> &full_clusters, int initial_size, Args&&... args );

	CKeyValues3Array *AllocArray( int initial_size = 0 ) { return RawAlloc( m_RawArrayEntries, m_PartialArrayClusters, m_FullArrayClusters, initial_size ); }
	CKeyValues3Table *AllocTable( int initial_size = 0 ) { return RawAlloc( m_RawTableEntries, m_PartialTableClusters, m_FullTableClusters, initial_size ); }

	template<typename CLUSTER, typename NODE>
	void Free( NODE *element, ClusterNodeChain<CLUSTER> &partial_clusters, ClusterNodeChain<CLUSTER> &full_clusters );

	inline void FreeArray( CKeyValues3Array *element ) { Free( element, m_PartialArrayClusters, m_FullArrayClusters ); }
	inline void FreeTable( CKeyValues3Table *element ) { Free( element, m_PartialTableClusters, m_FullTableClusters ); }

	friend class KeyValues3;
};

inline CKV3ArenaImpl &CKV3Arena::Impl()
{
	return *reinterpret_cast< CKV3ArenaImpl * >( m_Storage );
}

inline const CKV3ArenaImpl &CKV3Arena::Impl() const
{
	return *reinterpret_cast< const CKV3ArenaImpl * >( m_Storage );
}

template<typename CLUSTER>
inline void CKV3ArenaBase::ClusterNodeChain<CLUSTER>::AddToChain( CLUSTER *cluster )
{
	if(m_pTail)
		m_pTail->SetNext( cluster );
	else
		m_pHead = cluster;

	cluster->SetNext( nullptr );
	cluster->SetPrev( m_pTail );

	m_pTail = cluster;
}

template<typename CLUSTER>
inline void CKV3ArenaBase::ClusterNodeChain<CLUSTER>::RemoveFromChain( CLUSTER *cluster )
{
	auto prev = cluster->GetPrev();
	auto next = cluster->GetNext();

	if(prev)
		prev->SetNext( next );
	else
		m_pHead = next;

	if(next)
		next->SetPrev( prev );
	else
		m_pTail = prev;

	cluster->SetPrev( nullptr );
	cluster->SetNext( nullptr );
}

template<typename NODE>
inline void CKV3ArenaBase::NodeList<NODE>::EnsureByteSize( int bytes_needed )
{
	if(bytes_needed < m_nAllocatedBytes)
		return;

	int new_alloc_size = KV3Helpers::CalcNewBufferSize( m_nAllocatedBytes, bytes_needed, ALLOC_CONTEXT_NODELIST_MIN, ALLOC_CONTEXT_NODELIST_MAX );

	m_pData = (ListEntry *)realloc( m_pData, new_alloc_size );
	m_nAllocatedBytes = new_alloc_size;
}

template<typename NODE>
inline NODE *CKV3ArenaBase::NodeList<NODE>::Alloc( int initial_size )
{
	int byte_size_needed = m_nUsedBytes + static_cast<int>(NODE::TotalSizeOf( initial_size )) + 8;
	EnsureByteSize( byte_size_needed );
	
	auto entry = Tail();
	m_nUsedBytes = byte_size_needed;

	Construct( &entry->m_Value, KV3_INVALID_CLUSTER_ELEMENT, initial_size );
	entry->m_pNext = Tail();

	return &entry->m_Value;
}

template<typename NODE>
inline void CKV3ArenaBase::NodeList<NODE>::Clear()
{
	if(m_nAllocatedBytes > 0)
	{
		for(auto iter = Head(); iter; iter = iter->m_pNext)
		{
			Destruct( &iter->m_Value );
		}
	}

	m_nUsedBytes = 0;
}

template<typename NODE>
inline void CKV3ArenaBase::NodeList<NODE>::Purge()
{
	Clear();

	free( m_pData );
	m_pData = nullptr;
}

template<typename CLUSTER>
inline void CKV3ArenaImpl::PurgeClusterNodeChain( ClusterNodeChain<CLUSTER> &cluster_node )
{
	CLUSTER *prev = nullptr;
	for(auto node = cluster_node.m_pTail; node; node = prev)
	{
		prev = node->GetPrev();

		if(node->IsAllocatedOnHeap())
		{
			node->Purge();
			g_pMemAlloc->RegionFree( MEMALLOC_REGION_FREE_4, node );
		}
		else
		{
			node->Clear();
		}
	}

	cluster_node.Reset();
}

template<typename CLUSTER>
inline void CKV3ArenaImpl::ClearClusterNodeChain( ClusterNodeChain<CLUSTER> &cluster_node )
{
	for(auto node = cluster_node.m_pTail; node; node = node->GetPrev())
	{
		node->Clear();
	}
}

template<typename CLUSTER>
inline void CKV3ArenaImpl::MoveToPartial( ClusterNodeChain<CLUSTER> &full_cluster, ClusterNodeChain<CLUSTER> &partial_cluster )
{
	CLUSTER *prev;
	for(auto node = full_cluster.m_pTail; node; node = prev)
	{
		prev = node->GetPrev();
		partial_cluster.AddToChain( node );
	}

	full_cluster.Reset();
}

template <typename CLUSTER, typename... Args, typename>
auto CKV3ArenaImpl::Alloc( ClusterNodeChain<CLUSTER> &partial_clusters,
								ClusterNodeChain<CLUSTER> &full_clusters,
								int initial_size, Args&&... args )
{
	auto cluster = partial_clusters.m_pTail;
	typename CLUSTER::NodeType *elem = nullptr;

	if(cluster)
	{
		elem = cluster->Alloc( Forward< Args >( args )... );

		if(cluster->IsFull())
		{
			partial_clusters.RemoveFromChain( cluster );
			full_clusters.AddToChain( cluster );
		}
	}
	else
	{
		cluster = (CLUSTER *)g_pMemAlloc->RegionAlloc( MEMALLOC_REGION_ALLOC_4, CLUSTER::TotalSizeOf( initial_size ) );

		Construct( cluster, m_pContext, true, initial_size );
		partial_clusters.AddToChain( cluster );

		elem = cluster->Alloc( Forward< Args >( args )... );
	}

	return elem;
}

template<typename CLUSTER, typename NODE, typename ...Args, typename>
inline NODE *CKV3ArenaImpl::RawAlloc( NodeList<NODE> &raw_array, ClusterNodeChain<CLUSTER> &partial_clusters, ClusterNodeChain<CLUSTER> &full_clusters, int initial_size, Args && ...args )
{
	int needed_byte_size = MAX( static_cast<int>(NODE::TotalSizeOf( initial_size )), 32 );

	if(raw_array.IsFull() || needed_byte_size > raw_array.FreeBytes())
	{
		if(initial_size <= ( int )NODE::DATA_SIZE)
			return Alloc( partial_clusters, full_clusters, CLUSTER::CLUSTER_SIZE );
		else
			return nullptr;
	}

	return raw_array.Alloc( initial_size );
}

template<typename CLUSTER, typename NODE>
void CKV3ArenaImpl::Free( NODE *element, ClusterNodeChain<CLUSTER> &partial_clusters, ClusterNodeChain<CLUSTER> &full_clusters )
{
	auto cluster = element->GetCluster();

	Assert( cluster != nullptr && cluster->GetContext() == m_pContext );
	
	cluster->Free( element );

	int num_allocated = cluster->NumAllocated();

	if(cluster->NumCount() > 0)
	{
		if(cluster->NumCount() == num_allocated - 1)
		{
			full_clusters.RemoveFromChain( cluster );
			partial_clusters.AddToChain( cluster );
		}
	}
	else if(cluster->IsAllocatedOnHeap())
	{
		partial_clusters.RemoveFromChain( cluster );

		Destruct( cluster );
		g_pMemAlloc->RegionFree( MEMALLOC_REGION_FREE_4, cluster );
	}
}

#include "tier0/memdbgoff.h"

#endif // KEYVALUES3_ARENA_H
