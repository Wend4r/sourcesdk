#ifndef KEYVALUES3_CLUSTER_H
#define KEYVALUES3_CLUSTER_H

#ifdef _WIN32
#pragma once
#endif

#include "kv3lib/keyvalues3.h"
#include "keyvalues3_metadata.h"
#include "keyvalues3_array.h"
#include "keyvalues3_table.h"

#include "tier0/memdbgon.h"

template <size_t SIZE, typename T>
class CKeyValues3ClusterImpl
{
public:
	typedef T NodeType;
	static const size_t CLUSTER_SIZE = SIZE;

	union Node
	{
		Node() : m_pNextFree( nullptr ) {}
		~Node() {}

		NodeType m_Value;
		Node *m_pNextFree;
	};

	enum
	{
		HEAP_MARKER = (1 << 31),

		FLAGS_MASK = ~(HEAP_MARKER)
	};

	CKeyValues3ClusterImpl( CKV3Arena *context, bool allocated_on_heap = false, int initial_size = SIZE );
	~CKeyValues3ClusterImpl() { Purge(); }

	CKV3Arena *GetContext() const { return m_pContext; }

	bool IsFull() const { return NumCount() >= NumAllocated(); }
	bool IsAllocatedOnHeap() const { return (m_nAllocatedElements & HEAP_MARKER) != 0; }
	int NumAllocated() const { return m_nAllocatedElements & FLAGS_MASK; }
	int NumCount() const { return m_nElementCount; }

	template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<NodeType, Args...>, int>>
	NodeType *Alloc( Args&&... args );

	void Free( int element, bool clearing_context = false );
	void Free( NodeType *node, bool clearing_context = false );

	void Purge();
	void Clear();

	Node *GetNextFree() const { return m_pFirstFreeNode; }
	void SetNextFree( Node *free ) { m_pFirstFreeNode = free; }

	CKeyValues3ClusterImpl *GetNext() const { return m_pNext; }
	void SetNext( CKeyValues3ClusterImpl *cluster ) { m_pNext = cluster; }

	CKeyValues3ClusterImpl *GetPrev() const { return m_pPrev; }
	void SetPrev( CKeyValues3ClusterImpl *cluster ) { m_pPrev = cluster; }

	Node *Head() { return &m_Values[0]; }
	Node *Tail() { return &m_Values[NumAllocated()]; }

	const Node *Head() const { return const_cast<CKeyValues3ClusterImpl *>(this)->Head(); }
	const Node *Tail() const { return const_cast<CKeyValues3ClusterImpl *>(this)->Tail(); }

	void EnableMetaData( bool bEnable );
	void ClearMetaData();
	void PurgeMetaData();
	void PurgeMetaData( int element );
	KV3MetaData_t *GetMetaData( int element ) const;

	int GetNodeIndex( NodeType *node ) const;

	static constexpr size_t TotalSizeOf( int initial_size ) { return ALIGN_VALUE( TotalSizeWithoutStaticData() + TotalSizeOfData( MAX( initial_size, 0 ) ), 8 ); }
	static constexpr size_t TotalSizeOfData( int size ) { return sizeof( Node ) * size; }
	static constexpr size_t TotalSizeWithoutStaticData() { return sizeof( CKeyValues3ClusterImpl ) - TotalSizeOfData( SIZE ); }

	friend CKeyValues3Cluster *KeyValues3::GetCluster() const;
	friend CKeyValues3ArrayCluster *CKeyValues3Array::GetCluster() const;
	friend CKeyValues3TableCluster *CKeyValues3Table::GetCluster() const;

private:
	void InitNodes();
	void PurgeNodes( bool clearing_context = false );

private:
	struct kv3metadata_t
	{
		int m_AllocatedElements;
		KV3MetaData_t m_elements[SIZE];
	};

	CKV3Arena *m_pContext;
	Node *m_pFirstFreeNode;

	int m_nAllocatedElements;
	int m_nElementCount;

	CKeyValues3ClusterImpl *m_pPrev;
	CKeyValues3ClusterImpl *m_pNext;

	kv3metadata_t *m_pMetaData;

	Node m_Values[SIZE];
};

template<size_t SIZE, typename T>
inline CKeyValues3ClusterImpl<SIZE, T>::CKeyValues3ClusterImpl( CKV3Arena *context, bool allocated_on_heap, int initial_size ) :
	m_pContext( context ),
	m_pFirstFreeNode( nullptr ),
	m_nAllocatedElements( initial_size | (allocated_on_heap ? HEAP_MARKER : 0) ),
	m_nElementCount( 0 ),
	m_pPrev( nullptr ),
	m_pNext( nullptr ),
	m_pMetaData( nullptr )
{
	InitNodes();
}

template<size_t SIZE, typename T>
template<typename... Args, typename>
inline T *CKeyValues3ClusterImpl<SIZE, T>::Alloc( Args&&... args )
{
	Assert( !IsFull() );

	Node *node = GetNextFree();
	Assert( node != nullptr );

	SetNextFree( node->m_pNextFree );

	Construct( &node->m_Value, Forward< Args >( args )... );
	node->m_Value.SetClusterElement( GetNodeIndex( &node->m_Value ) );

	m_nElementCount++;

	return &node->m_Value;
}

template<size_t SIZE, typename T>
inline void CKeyValues3ClusterImpl<SIZE, T>::Free( NodeType* node, bool clearing_context )
{
	Assert( node >= (void *)Head() && node < (void *)Tail() );
	Free( GetNodeIndex( node ), clearing_context );
}

template<size_t SIZE, typename T>
inline void CKeyValues3ClusterImpl<SIZE, T>::Free( int element, bool clearing_context )
{
	Assert( element >= 0 && element < NumAllocated() );

	Node *node = &m_Values[element];
	node->m_Value.Free( clearing_context );

	m_nElementCount--;

	node->m_pNextFree = GetNextFree();
	SetNextFree( node );
}

template<size_t SIZE, typename T>
inline void CKeyValues3ClusterImpl<SIZE, T>::InitNodes()
{
	Node *iter = Tail() - 1;
	Node *prev = nullptr;

	for(int i = 0; i < NumAllocated(); i++, iter--)
	{
		iter->m_pNextFree = prev;
		prev = iter;
	}

	m_nElementCount = 0;
	SetNextFree( prev );
}

template<size_t SIZE, typename T>
inline void CKeyValues3ClusterImpl<SIZE, T>::PurgeNodes( bool clearing_context )
{
	CVarBitVec free_nodes( NumAllocated() );

	for(auto iter = GetNextFree(); iter; iter = iter->m_pNextFree)
	{
		free_nodes.Set( GetNodeIndex( &iter->m_Value ) );
	}

	if(!free_nodes.IsAllSet())
	{
		for(int i = 0; i < NumAllocated(); i++)
		{
			if(!free_nodes.IsBitSet( i ))
			{
				Free( i, clearing_context );
			}
		}

		InitNodes();
	}
}

template<size_t SIZE, typename T>
inline int CKeyValues3ClusterImpl<SIZE, T>::GetNodeIndex( NodeType *element ) const
{
	Node *node = reinterpret_cast<Node *>(element);

	auto head = Head();
	if(node < head || node >= Tail())
		return -1;

	return node - head;
}

template<size_t SIZE, typename T>
inline void CKeyValues3ClusterImpl<SIZE, T>::Purge()
{
	PurgeNodes( true );
	PurgeMetaData();
}

template<size_t SIZE, typename T>
inline void CKeyValues3ClusterImpl<SIZE, T>::Clear()
{
	PurgeNodes( true );
	ClearMetaData();
}

template<size_t SIZE, typename T>
void CKeyValues3ClusterImpl<SIZE, T>::EnableMetaData( bool bEnable )
{
	if(bEnable)
	{
		if(!m_pMetaData)
		{
			m_pMetaData = (kv3metadata_t *)g_pMemAlloc->RegionAlloc( MEMALLOC_REGION_ALLOC_4, (NumAllocated() * sizeof(KV3MetaData_t)) + 8 );
			m_pMetaData->m_AllocatedElements = NumAllocated();
		}
	}
	else
	{
		PurgeMetaData();
	}
}

template<size_t SIZE, typename T>
void CKeyValues3ClusterImpl<SIZE, T>::ClearMetaData()
{
	if(m_pMetaData)
	{
		for(int i = 0; i < m_pMetaData->m_AllocatedElements; i++)
		{
			m_pMetaData->m_elements[i].Clear();
		}
	}
}

template<size_t SIZE, typename T>
void CKeyValues3ClusterImpl<SIZE, T>::PurgeMetaData()
{
	if(m_pMetaData)
	{
		for(int i = 0; i < m_pMetaData->m_AllocatedElements; i++)
		{
			m_pMetaData->m_elements[i].Purge();
		}

		g_pMemAlloc->RegionFree( MEMALLOC_REGION_FREE_4, m_pMetaData );
	}

	m_pMetaData = nullptr;
}

template<size_t SIZE, typename T>
void CKeyValues3ClusterImpl<SIZE, T>::PurgeMetaData( int element )
{
	if(!m_pMetaData)
		return;

	Assert( element >= 0 && element < m_pMetaData->m_AllocatedElements );
	GetMetaData( element )->Clear();
}

template<size_t SIZE, typename T>
KV3MetaData_t *CKeyValues3ClusterImpl<SIZE, T>::GetMetaData( int element ) const
{
	if(!m_pMetaData)
		return nullptr;

	Assert( element >= 0 && element < m_pMetaData->m_AllocatedElements );
	return &m_pMetaData->m_elements[element];
}

#include "tier0/memdbgoff.h"

#endif // KEYVALUES3_CLUSTER_H
