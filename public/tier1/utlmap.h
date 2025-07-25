//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $Header: $
// $NoKeywords: $
//=============================================================================//

#ifndef UTLMAP_H
#define UTLMAP_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/dbg.h"
#include "tier0/platform.h"
#include "utlrbtree.h"

//-----------------------------------------------------------------------------
//
// Purpose:	An associative container. Pretty much identical to std::map.
//
//-----------------------------------------------------------------------------

// This is a useful macro to iterate from start to end in order in a map
#define FOR_EACH_MAP( mapName, iteratorName ) \
	for ( int iteratorName = ( mapName ).FirstInorder(); ( mapName ).IsUtlMap && iteratorName != ( mapName ).InvalidIndex(); iteratorName = ( mapName ).NextInorder( iteratorName ) )

// faster iteration, but in an unspecified order
#define FOR_EACH_MAP_FAST( mapName, iteratorName )                                                                \
	for ( int iteratorName = 0; ( mapName ).IsUtlMap && iteratorName < ( mapName ).MaxElement(); ++iteratorName ) \
		if ( !( mapName ).IsValidIndex( iteratorName ) )                                                          \
			continue;                                                                                             \
		else

struct base_utlmap_t
{
public:
	// This enum exists so that FOR_EACH_MAP and FOR_EACH_MAP_FAST cannot accidentally
	// be used on a type that is not a CUtlOrderedMapBase. If the code compiles then all is well.
	// The check for IsUtlMap being true should be free.
	// Using an enum rather than a static const bool ensures that this trick works even
	// with optimizations disabled on gcc.
	enum
	{
		IsUtlMap = true
	};
};

template < typename K, typename T, typename L, typename I = int >
class CUtlOrderedMapBase : public base_utlmap_t
{
public:
	typedef K KeyType_t;
	typedef T ElemType_t;
	typedef L LessFunc_t;
	typedef I IndexType_t;

	struct Node_t
	{
		KeyType_t key;
		ElemType_t elem;

		Node_t() : key(), elem() {}

		// Declare a default constructor into your elem type.
		Node_t( const KeyType_t &inKey ) : key( inKey ), elem() {}
		Node_t( KeyType_t &&inKey ) : key( Move( inKey ) ), elem() {}

		Node_t( const KeyType_t &inKey, const ElemType_t &inElem ) : key( inKey ), elem( inElem ) {}
		Node_t( const KeyType_t &inKey, ElemType_t &&moveElem ) : key( inKey ), elem( Move( moveElem ) ) {}
		Node_t( KeyType_t &&moveKey, const ElemType_t &inElem ) : key( Move( moveKey ) ), elem( inElem ) {}
		Node_t( KeyType_t &&moveKey, ElemType_t &&moveElem ) : key( Move( moveKey ) ), elem( Move( moveElem ) ) {}

		Node_t( const Node_t &copyFrom ) : Node_t( copyFrom.key, copyFrom.elem ) {}
		Node_t( Node_t &&moveFrom ) : Node_t( Move( moveFrom.key ), Move( moveFrom.elem ) ) {}

		Node_t &operator=( const Node_t &copyFrom )
		{
			key = copyFrom.key;
			elem = copyFrom.elem;
			return *this;
		}
		Node_t &operator=( Node_t &&moveFrom )
		{
			key = Move( moveFrom.key );
			elem = Move( moveFrom.elem );
			return *this;
		}
	};

	class CKeyLess
	{
	public:
		CKeyLess( const LessFunc_t &lessFunc = LessFunc_t{} ) : m_LessFunc( lessFunc ) {}
		CKeyLess( LessFunc_t &&lessFunc ) : m_LessFunc( Move( lessFunc ) ) {}

		bool operator!() const
		{
			return !m_LessFunc;
		}

		bool operator()( const Node_t &left, const Node_t &right ) const
		{
			return m_LessFunc( left.key, right.key );
		}

		LessFunc_t m_LessFunc;
	};

	typedef CUtlRBTree< Node_t, CKeyLess, I > CTree;

	// constructor, destructor
	// Left at growSize = 0, the memory will first allocate 1 element and double in size
	// at each increment.
	// LessFunc_t is required, but may be set after the constructor using SetLessFunc() below
	CUtlOrderedMapBase( int growSize, int initSize, const LessFunc_t &lessfunc )
	 : m_Tree( growSize, initSize, CKeyLess( lessfunc ) )
	{
	}

	CUtlOrderedMapBase( int growSize, int initSize, LessFunc_t &&lessfunc )
	 : m_Tree( growSize, initSize, CKeyLess( Move( lessfunc ) ) )
	{
	}

	CUtlOrderedMapBase( const CUtlOrderedMapBase< K, T, L, I > &copyFrom )
	{
		CopyFrom( copyFrom );
	}

	CUtlOrderedMapBase( CUtlOrderedMapBase< K, T, L, I > &&moveFrom )
	{
		MoveFrom( Move( moveFrom ) );
	}

	CUtlOrderedMapBase( const CTree &copyFrom )
	{
		CopyFrom( copyFrom );
	}

	CUtlOrderedMapBase( CTree &&moveFrom )
	 : m_Tree( Move( moveFrom ) )
	{
	}

	void EnsureCapacity( int num ) { m_Tree.EnsureCapacity( num ); }

	CUtlOrderedMapBase< K, T, L, I > &operator=( const CUtlOrderedMapBase< K, T, L, I > &other ) { return CopyFrom( other ); }
	CUtlOrderedMapBase< K, T, L, I > &operator=( CUtlOrderedMapBase< K, T, L, I > &&other ) { return MoveFrom( Move( other ) ); }
	CUtlOrderedMapBase< K, T, L, I > &CopyFrom( const CUtlOrderedMapBase< K, T, L, I > &other )
	{
		m_Tree.CopyFrom( other.m_Tree );
		return *this;
	}
	CUtlOrderedMapBase< K, T, L, I > &MoveFrom( CUtlOrderedMapBase< K, T, L, I > &&other )
	{
		m_Tree.MoveFrom( Move( other.m_Tree ) );
		return *this;
	}

	// gets particular elements
	ElemType_t &Element( IndexType_t i ) { return m_Tree.Element( i ).elem; }
	const ElemType_t &Element( IndexType_t i ) const { return m_Tree.Element( i ).elem; }
	ElemType_t &operator[]( IndexType_t i ) { return m_Tree.Element( i ).elem; }
	const ElemType_t &operator[]( IndexType_t i ) const { return m_Tree.Element( i ).elem; }
	KeyType_t &Key( IndexType_t i ) { return m_Tree.Element( i ).key; }
	const KeyType_t &Key( IndexType_t i ) const { return m_Tree.Element( i ).key; }

	// Num elements
	unsigned int Count() const { return m_Tree.Count(); }

	// Max "size" of the vector
	IndexType_t MaxElement() const { return m_Tree.MaxElement(); }

	// Checks if a node is valid and in the map
	bool IsValidIndex( IndexType_t i ) const { return m_Tree.IsValidIndex( i ); }

	// Checks if the map as a whole is valid
	bool IsValid() const { return m_Tree.IsValid(); }

	// Invalid index
	static IndexType_t InvalidIndex() { return CTree::InvalidIndex(); }

	// Sets the less func
	void SetLessFunc( LessFunc_t func )
	{
		m_Tree.SetLessFunc( CKeyLess( func ) );
	}

	// Insert method (inserts in order)
	IndexType_t Insert( const KeyType_t &key, const ElemType_t &insert ) { return m_Tree.Insert( Node_t( key, insert ) ); }
	IndexType_t Insert( const KeyType_t &key, ElemType_t &&insert ) { return m_Tree.Insert( Node_t( key, Move( insert ) ) ); }
	IndexType_t Insert( KeyType_t &&key, const ElemType_t &insert ) { return m_Tree.Insert( Node_t( Move( key ), insert ) ); }
	IndexType_t Insert( KeyType_t &&key, ElemType_t &&insert ) { return m_Tree.Insert( Node_t( Move( key ), Move( insert ) ) ); }
	IndexType_t Insert( const KeyType_t &key ) { return m_Tree.Insert( Node_t( key ) ); }

	// API to macth src2 for Panormama
	// Note in src2 straight Insert() calls will assert on duplicates
	// Chosing not to take that change until discussed further

	IndexType_t InsertWithDupes( const KeyType_t &key, const ElemType_t &insert ) { return m_Tree.Insert( Node_t( key, insert ) ); }
	IndexType_t InsertWithDupes( const KeyType_t &key ) { return m_Tree.Insert( Node_t( key ) ); }

	bool HasElement( const KeyType_t &key ) const { return m_Tree.HasElement( Node_t( key ) ); }

	IndexType_t Find( const KeyType_t &key ) const { return m_Tree.Find( Node_t( key ) ); }

	// This finds the first inorder occurrence of key
	IndexType_t FindFirst( const KeyType_t &key ) const { return m_Tree.FindFirst( Node_t( key ) ); }

	const ElemType_t &FindElement( const KeyType_t &key, const ElemType_t &defaultValue ) const
	{
		IndexType_t i = Find( key );
		if ( i == InvalidIndex() )
			return defaultValue;
		return Element( i );
	}

	// First element >= key
	IndexType_t FindClosest( const KeyType_t &key, CompareOperands_t eFindCriteria ) const
	{
		Node_t dummyNode;
		dummyNode.key = key;
		return m_Tree.FindClosest( dummyNode, eFindCriteria );
	}

	// Remove methods
	void RemoveAt( IndexType_t i ) { m_Tree.RemoveAt( i ); }
	bool Remove( const KeyType_t &key )
	{
		Node_t dummyNode;
		dummyNode.key = key;
		return m_Tree.Remove( dummyNode );
	}

	void RemoveAll() { m_Tree.RemoveAll(); }
	void Purge() { m_Tree.Purge(); }

	// Purges the list and calls delete on each element in it.
	void PurgeAndDeleteElements();

	// Iteration
	IndexType_t FirstInorder() const { return m_Tree.FirstInorder(); }
	IndexType_t NextInorder( IndexType_t i ) const { return m_Tree.NextInorder( i ); }
	IndexType_t PrevInorder( IndexType_t i ) const { return m_Tree.PrevInorder( i ); }
	IndexType_t LastInorder() const { return m_Tree.LastInorder(); }

	// API Matching src2 for Panorama
	IndexType_t NextInorderSameKey( IndexType_t i ) const
	{
		IndexType_t iNext = NextInorder( i );
		if ( !IsValidIndex( iNext ) )
			return InvalidIndex();
		if ( Key( iNext ) != Key( i ) )
			return InvalidIndex();
		return iNext;
	}

	// If you change the search key, this can be used to reinsert the
	// element into the map.
	void Reinsert( const KeyType_t &key, IndexType_t i )
	{
		m_Tree[i].key = key;
		m_Tree.Reinsert( i );
	}

	IndexType_t InsertOrReplace( const KeyType_t &key, const ElemType_t &insert )
	{
		IndexType_t i = Find( key );
		if ( i != InvalidIndex() )
		{
			Element( i ) = insert;
			return i;
		}

		return Insert( key, insert );
	}

	IndexType_t InsertOrReplace( const KeyType_t &key, ElemType_t &&moveInsert )
	{
		IndexType_t i = Find( key );
		if ( i != InvalidIndex() )
		{
			Element( i ) = Move( moveInsert );
			return i;
		}

		return Insert( key, Move( moveInsert ) );
	}

	void Swap( CUtlOrderedMapBase< K, T, L, I > &that )
	{
		m_Tree.Swap( that.m_Tree );
	}

	CTree *AccessTree() { return &m_Tree; }

protected:
	CTree m_Tree;
};

//-----------------------------------------------------------------------------

// Purges the list and calls delete on each element in it.
template < typename K, typename T, typename L, typename I >
inline void CUtlOrderedMapBase< K, T, L, I >::PurgeAndDeleteElements()
{
	for ( I i = 0; i < MaxElement(); ++i )
	{
		if ( !IsValidIndex( i ) )
			continue;

		delete Element( i );
	}

	Purge();
}

template < typename K, typename T, typename L = CDefLess< K >, typename I = int >
class CUtlOrderedMap : public CUtlOrderedMapBase< K, T, L, I >
{
public:
	using BaseClass = CUtlOrderedMapBase< K, T, L, I >;

	CUtlOrderedMap( int growSize, int initSize, const L &moveLess = L() )
	 : BaseClass( growSize, initSize, moveLess )
	{
	}

	CUtlOrderedMap( int growSize, int initSize, L &&moveLess )
	 : BaseClass( growSize, initSize, Move( moveLess ) )
	{
	}

	CUtlOrderedMap( const L &copyLess = L() )
	 : BaseClass( 0, 0, copyLess )
	{
	}
	CUtlOrderedMap( L &&moveLess )
	 : BaseClass( 0, 0, Move( moveLess ) )
	{
	}
};

// @Wend4r: the order of template arguments is arranged for compatibility with S1 declarations.
template < typename K, typename T, typename I = unsigned short, typename L = bool ( * )( const K &, const K & ) >
class CUtlMap : public CUtlOrderedMapBase< K, T, L, I >
{
public:
	using BaseClass = CUtlOrderedMapBase< K, T, L, I >;

	CUtlMap( int growSize, int initSize, const L &lessFunc = DefLessFunc( const K ) )
	 : BaseClass( growSize, initSize, lessFunc )
	{
	}

	CUtlMap( const L &lessFunc = DefLessFunc( const K ) )
	 : CUtlMap( 0, 0, lessFunc )
	{
	}
};

#endif // UTLMAP_H
