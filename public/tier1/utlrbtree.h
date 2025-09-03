//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Header: $
// $NoKeywords: $
//=============================================================================//

#ifndef UTLRBTREE_H
#define UTLRBTREE_H

#include "platform.h"
#include "utlleanvector.h"
#include "utlfixedmemory.h"
#include "utlblockmemory.h"


// This is a useful macro to iterate from start to end in order in a map
#define FOR_EACH_UTLRBTREE( treeName, iteratorName ) \
	for ( int iteratorName = treeName.FirstInorder(); (treeName).IsUtlRBTree && iteratorName != treeName.InvalidIndex(); iteratorName = treeName.NextInorder( iteratorName ) )


//-----------------------------------------------------------------------------
// Tool to generate a default compare function for any type that implements
// operator<, including all simple types
//-----------------------------------------------------------------------------

template <typename T>
class CLessFunc
{
public:
	static bool LessFunc( const T &lhs, const T &rhs )	{ return ( lhs < rhs );	}
};

#define DefLessFunc( type ) CLessFunc< type >::LessFunc

template <typename T>
class CDefLess
{
public:
	CDefLess() {}
	CDefLess( int i ) {}
	inline bool operator!() const { return false; }
	inline bool operator()( const T &lhs, const T &rhs ) const { return ( lhs < rhs );	}
};

//-------------------------------------

inline bool StringLessThan( const char * const &lhs, const char * const &rhs)			
{
	if ( !lhs ) return false;
	if ( !rhs ) return true;
	return ( strcmp( lhs, rhs ) < 0 );  
}

inline bool CaselessStringLessThan( const char * const &lhs, const char * const &rhs )	
{ 
	if ( !lhs ) return false;
	if ( !rhs ) return true;
	return ( stricmp( lhs, rhs ) < 0 ); 
}

inline bool FastCaselessStringLessThan( const char * const &lhs, const char * const &rhs )	
{ 
	if ( !lhs ) return false;
	if ( !rhs ) return true;
	return ( V_stricmp_fast( lhs, rhs ) < 0 ); 
}

// Same as CaselessStringLessThan, but it ignores differences in / and \.
inline bool CaselessStringLessThanIgnoreSlashes( const char * const &lhs, const char * const &rhs )	
{ 
	const char *pa = lhs;
	const char *pb = rhs;
	while ( *pa && *pb )
	{
		char a = *pa;
		char b = *pb;
		
		// Check for dir slashes.
		if ( a == '/' || a == '\\' )
		{
			if ( b != '/' && b != '\\' )
				return ('/' < b);
		}
		else
		{
			if ( a >= 'a' && a <= 'z' )
				a = 'A' + (a - 'a');
			
			if ( b >= 'a' && b <= 'z' )
				b = 'A' + (b - 'a');
				
			if ( a > b )
				return false;
			else if ( a < b )
				return true;
		}
		++pa;
		++pb;
	}
	
	// Filenames also must be the same length.
	if ( *pa != *pb )
	{
		// If pa shorter than pb then it's "less"
		return ( !*pa );
	}

	return false;
}

class CDefStringLess
{
public:
	CDefStringLess() {}
	CDefStringLess( int i ) {}
	inline bool operator()( const char * const &lhs, const char * const &rhs ) const { return ( strcmp( lhs, rhs ) < 0 ); }
	inline bool operator!() const { return false; }
};

class CDefCaselessStringLess
{
public:
	CDefCaselessStringLess() {}
	CDefCaselessStringLess( int i ) {}
	inline bool operator()( const char * const &lhs, const char * const &rhs ) const { return ( stricmp( lhs, rhs ) < 0 ); }
	inline bool operator!() const { return false; }
};

class CDefFastCaselessStringLess
{
public:
	CDefFastCaselessStringLess() {}
	CDefFastCaselessStringLess( int i ) {}
	inline bool operator()( const char * const &lhs, const char * const &rhs ) const { return ( V_stricmp_fast( lhs, rhs ) < 0 ); }
	inline bool operator!() const { return false; }
};

class CDefCaselessStringLessIgnoreSlashes
{
public:
	CDefCaselessStringLessIgnoreSlashes() {}
	CDefCaselessStringLessIgnoreSlashes( int i ) {}
	inline bool operator()( const char * const &lhs, const char * const &rhs ) const { return CaselessStringLessThanIgnoreSlashes( lhs, rhs ); }
	inline bool operator!() const { return false; }
};

//-------------------------------------
// inline these two templates to stop multiple definitions of the same code
template <> inline bool CLessFunc<const char *>::LessFunc( const char * const &lhs, const char * const &rhs )	{ return StringLessThan( lhs, rhs ); }
template <> inline bool CLessFunc<char *>::LessFunc( char * const &lhs, char * const &rhs )						{ return StringLessThan( lhs, rhs ); }

//-------------------------------------

template <typename RBTREE_T>
void SetDefLessFunc( RBTREE_T &RBTree )
{
	RBTree.SetLessFunc( DefLessFunc( typename RBTREE_T::KeyType_t ) );
}

// For use with FindClosest
// Move these to a common area if anyone else ever uses them
enum CompareOperands_t
{
	k_EEqual = 0x1,
	k_EGreaterThan = 0x2,
	k_ELessThan = 0x4,
	k_EGreaterThanOrEqualTo = k_EGreaterThan | k_EEqual,
	k_ELessThanOrEqualTo = k_ELessThan | k_EEqual,
};

//-----------------------------------------------------------------------------
// A red-black binary search tree
//-----------------------------------------------------------------------------

template < class I >
struct UtlRBTreeLinks_t
{
	I  m_Left;
	I  m_Right;
	I  m_Parent;
	I  m_Tag;
};

template < class T, class I >
struct UtlRBTreeNode_t : public UtlRBTreeLinks_t< I >
{
	T  m_Data;
};

template < class T, typename L, class I = unsigned short, class M = CUtlLeanVector< UtlRBTreeNode_t< T, I >, I > >
class CUtlRBTree
{
public:

	typedef T KeyType_t;
	typedef T ElemType_t;
	typedef I IndexType_t;
	enum { IsUtlRBTree = true }; // Used to match this at compiletime 

	// Less func typedef
	// Returns true if the first parameter is "less" than the second
	typedef L LessFunc_t;

	// constructor, destructor
	// Left at growSize = 0, the memory will first allocate 1 element and double in size
	// at each increment.
	// LessFunc_t is required, but may be set after the constructor using SetLessFunc() below
	explicit CUtlRBTree( int growSize = 0, int initSize = 0, LessFunc_t lessfunc = LessFunc_t{} );
	explicit CUtlRBTree( LessFunc_t lessfunc );
	CUtlRBTree( const CUtlRBTree<T, L, I, M> &copyFrom );
	CUtlRBTree( CUtlRBTree<T, L, I, M> &&moveFrom );
	~CUtlRBTree( );

	CUtlRBTree<T, L, I, M>& operator=( const CUtlRBTree<T, L, I, M> &copyFrom );
	CUtlRBTree<T, L, I, M>& operator=( CUtlRBTree<T, L, I, M> &&moveFrom );
	CUtlRBTree<T, L, I, M> &CopyFrom( const CUtlRBTree<T, L, I, M> &other );
	CUtlRBTree<T, L, I, M> &MoveFrom( CUtlRBTree<T, L, I, M> &&other );

	void EnsureCapacity( int num );

	// gets particular elements
	T&		Element( I i );
	T const	&Element( I i ) const;
	T&		operator[]( I i );
	T const	&operator[]( I i ) const;

	// Gets the root
	I  Root() const;

	// Num elements
	unsigned int Count() const;

	// Max "size" of the vector
	// it's not generally safe to iterate from index 0 to MaxElement()-1 (you could do this as a potential
	// iteration optimization, IF CUtlLeanVector is the allocator, and IF IsValidIndex() is tested for each element...
	//  but this should be implemented inside the CUtlRBTree iteration API, if anywhere)
	I  MaxElement() const;

	// Gets the children                               
	I  Parent( I i ) const;
	I  LeftChild( I i ) const;
	I  RightChild( I i ) const;

	// Tests if a node is a left or right child
	bool IsLeftChild( I i ) const;
	bool IsRightChild( I i ) const;

	// Tests if root or leaf
	bool IsRoot( I i ) const;
	bool IsLeaf( I i ) const;

	// Checks if a node is valid and in the tree
	bool IsValidIndex( I i ) const;

	// Checks if the tree as a whole is valid
	bool IsValid() const;

	// Invalid index
	static I InvalidIndex();

	// returns the tree depth (not a very fast operation)
	int Depth( I node ) const;
	int Depth() const;

	// Sets the less func
	void SetLessFunc( const LessFunc_t &func );

	// Allocation method
	I NewNode( bool bConstructElement );

	// Insert method (inserts in order)
	// NOTE: the returned 'index' will be valid as long as the element remains in the tree
	//       (other elements being added/removed will not affect it)
	I Insert( const T &insert );
	I Insert( T &&moveInsert );
	void Insert( const T *pArray, int nItems );
	I InsertIfNotFound( const T &insert );
	I InsertIfNotFound( T &&moveInsert );

	// Find method
	I Find( T const &search ) const;

	// FindFirst method ( finds first inorder if there are duplicates )
	I FindFirst( T const &search ) const;

	// First element >= key
	I FindClosest( T const &search, CompareOperands_t eFindCriteria ) const;

	// Remove methods
	void RemoveAt( I i );
	bool Remove( T const &remove );
	void RemoveAll( );
	void Purge();

	// Allocation, deletion
	void FreeNode( I i );

	// Iteration
	I FirstInorder() const;
	I NextInorder( I i ) const;
	I PrevInorder( I i ) const;
	I LastInorder() const;

	I FirstPreorder() const;
	I NextPreorder( I i ) const;
	I PrevPreorder( I i ) const;
	I LastPreorder( ) const;

	I FirstPostorder() const;
	I NextPostorder( I i ) const;

	// If you change the search key, this can be used to reinsert the 
	// element into the tree.
	void Reinsert( I elem );

	// swap in place
	void Swap( CUtlRBTree< T, I, L > &that );

protected:
	enum NodeColor_t
	{
		RED = 0,
		BLACK
	};

	typedef UtlRBTreeNode_t< T, I > Node_t;
	typedef UtlRBTreeLinks_t< I > Links_t;

	// Sets the children
	void SetParent( I i, I parent );
	void SetLeftChild( I i, I child  );
	void SetRightChild( I i, I child  );
	void LinkToParent( I i, I parent, bool isLeft );

	// Gets at the links
	Links_t const	&Links( I i ) const;
	Links_t			&Links( I i );

	// Checks if a link is red or black
	bool IsRed( I i ) const;
	bool IsBlack( I i ) const;

	// Sets/gets node color
	NodeColor_t Color( I i ) const;
	void        SetColor( I i, NodeColor_t c );

	// operations required to preserve tree balance
	void RotateLeft(I i);
	void RotateRight(I i);
	void InsertRebalance(I i);
	void RemoveRebalance(I i);

	// Insertion, removal
	I  InsertAt( I parent, bool leftchild, bool bConstructElement );

	// Inserts a node into the tree, doesn't copy the data in.
	void FindInsertionPosition( T const &insert, I &parent, bool &leftchild );

	// Remove and add back an element in the tree.
	void	Unlink( I elem );
	void	Link( I elem );

	// Used for sorting.
	LessFunc_t m_LessFunc;

	M m_Elements;
	I m_Root;
	I m_NumElements;
	I m_FirstFree;
	typename M::Iterator_t m_LastAlloc; // the last index allocated

	// Node_t* m_pElements;

	FORCEINLINE M const &Elements( void ) const
	{
		return m_Elements;
	}

	void ResetDbgInfo()
	{
		// m_pElements = (Node_t*)m_Elements.Base();
	}
};

// this is kind of ugly, but until C++ gets templatized typedefs in C++0x, it's our only choice
template < class T, class I = int, typename L = bool (*)( const T &, const T & )  >
class CUtlFixedRBTree : public CUtlRBTree< T, I, L, CUtlFixedMemory< UtlRBTreeNode_t< T, I > > >
{
public:

	typedef L LessFunc_t;

	CUtlFixedRBTree( int growSize = 0, int initSize = 0, const LessFunc_t &lessfunc = 0 )
		: CUtlRBTree< T, I, L, CUtlFixedMemory< UtlRBTreeNode_t< T, I > > >( growSize, initSize, lessfunc ) {}
	CUtlFixedRBTree( const LessFunc_t &lessfunc )
		: CUtlRBTree< T, I, L, CUtlFixedMemory< UtlRBTreeNode_t< T, I > > >( lessfunc ) {}

	typedef CUtlRBTree< T, I, L, CUtlFixedMemory< UtlRBTreeNode_t< T, I > > > BaseClass;
	bool IsValidIndex( I i ) const
	{
		if ( !BaseClass::Elements().IsIdxValid( i ) )
			return false;

#ifdef _DEBUG // it's safe to skip this here, since the only way to get indices after m_LastAlloc is to use MaxElement()
		if ( BaseClass::Elements().IsIdxAfter( i, this->m_LastAlloc ) )
		{
			Assert( 0 );
			return false; // don't read values that have been allocated, but not constructed
		}
#endif

		return LeftChild(i) != i; 
	}

protected:
	void ResetDbgInfo() {}

private:
	// this doesn't make sense for fixed rbtrees, since there's no useful max pointer, and the index space isn't contiguous anyways
	I  MaxElement() const;
};

// this is kind of ugly, but until C++ gets templatized typedefs in C++0x, it's our only choice
template < class T, class I = unsigned short, typename L = bool (*)( const T &, const T & )  >
class CUtlBlockRBTree : public CUtlRBTree< T, I, L, CUtlBlockMemory< UtlRBTreeNode_t< T, I >, I > >
{
public:
	typedef L LessFunc_t;
	CUtlBlockRBTree( int growSize = 0, int initSize = 0, const LessFunc_t &lessfunc = 0 )
		: CUtlRBTree< T, I, L, CUtlBlockMemory< UtlRBTreeNode_t< T, I >, I > >( growSize, initSize, lessfunc ) {}
	CUtlBlockRBTree( const LessFunc_t &lessfunc )
		: CUtlRBTree< T, I, L, CUtlBlockMemory< UtlRBTreeNode_t< T, I >, I > >( lessfunc ) {}
protected:
	void ResetDbgInfo() {}
};


//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline CUtlRBTree<T, L, I, M>::CUtlRBTree( int growSize, int initSize, LessFunc_t lessfunc ) : 
m_LessFunc( lessfunc ),
m_Elements( growSize, initSize ),
m_Root( InvalidIndex() ),
m_NumElements( 0 ),
m_FirstFree( InvalidIndex() ),
m_LastAlloc( m_Elements.InvalidIterator() )
{
	ResetDbgInfo();
}

template < class T, typename L, class I, class M >
inline CUtlRBTree<T, L, I, M>::CUtlRBTree( LessFunc_t lessfunc ) : 
m_LessFunc( lessfunc ),
m_Elements( ),
m_Root( InvalidIndex() ),
m_NumElements( 0 ),
m_FirstFree( InvalidIndex() ),
m_LastAlloc( m_Elements.InvalidIterator() )
{
	ResetDbgInfo();
}

template < class T, typename L, class I, class M >
inline CUtlRBTree<T, L, I, M>::CUtlRBTree( const CUtlRBTree<T, L, I, M> &copyFrom )
 :  m_LessFunc( copyFrom.m_LessFunc ), 
    m_LastAlloc( copyFrom.m_Elements.InvalidIterator() )
{
	CopyFrom( copyFrom );
}

template < class T, typename L, class I, class M >
inline CUtlRBTree<T, L, I, M>::CUtlRBTree( CUtlRBTree<T, L, I, M> &&moveFrom )
 :  m_LessFunc( moveFrom.m_LessFunc ), 
    m_LastAlloc( moveFrom.m_Elements.InvalidIterator() )
{
	MoveFrom( Move( moveFrom ) );
}

template < class T, typename L, class I, class M > 
inline CUtlRBTree<T, L, I, M>::~CUtlRBTree()
{
	Purge();
}

template < class T, typename L, class I, class M >
CUtlRBTree<T, L, I, M>& CUtlRBTree<T, L, I, M>::operator=( const CUtlRBTree<T, L, I, M> &copyFrom )
{
	CopyFrom( copyFrom );
	return *this;
}

template < class T, typename L, class I, class M >
CUtlRBTree<T, L, I, M>& CUtlRBTree<T, L, I, M>::operator=( CUtlRBTree<T, L, I, M> &&moveFrom )
{
	MoveFrom( moveFrom );
	return *this;
}

template < class T, typename L, class I, class M >
inline CUtlRBTree<T, L, I, M> &CUtlRBTree<T, L, I, M>::CopyFrom( const CUtlRBTree<T, L, I, M> &other )
{
	{
		const uintp nOtherSize = other.m_Elements.Count();

		m_Elements.EnsureCapacity( nOtherSize );

		auto *pBase = m_Elements.Base();

		const auto *pOtherBase = other.m_Elements.Base();

		for ( uintp n = 0; n < nOtherSize; n++ )
		{
			Construct( &pBase[n] );
			pBase[n] = pOtherBase[n];
		}
	}

	m_LessFunc = other.m_LessFunc;
	m_Root = other.m_Root;
	m_NumElements = other.m_NumElements;
	m_FirstFree = other.m_FirstFree;
	m_LastAlloc = other.m_LastAlloc;
	ResetDbgInfo();

	return *this;
}

template < class T, typename L, class I, class M >
inline CUtlRBTree<T, L, I, M> &CUtlRBTree<T, L, I, M>::MoveFrom( CUtlRBTree<T, L, I, M> &&other )
{
	{
		auto &&otherElems = Move( other.m_Elements );

		const uintp nOtherSize = otherElems.Count();

		m_Elements.EnsureCapacity( nOtherSize );

		auto *pBase = m_Elements.Base();

		auto *pOtherBase = otherElems.Base();

		for ( uintp n = 0; n < nOtherSize; n++ )
		{
			MoveConstruct( &pBase[n], Move( pOtherBase[n] ) );
		}
	}

	m_LessFunc = Move( other.m_LessFunc );
	m_Root = Move( other.m_Root );
	m_NumElements = Move( other.m_NumElements );
	m_FirstFree = Move( other.m_FirstFree );
	m_LastAlloc = Move( other.m_LastAlloc );
	ResetDbgInfo();

	return *this;
}

template < class T, typename L, class I, class M >
inline void CUtlRBTree<T, L, I, M>::EnsureCapacity( int num )
{ 
	m_Elements.EnsureCapacity( num );
}

//-----------------------------------------------------------------------------
// gets particular elements
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline T &CUtlRBTree<T, L, I, M>::Element( I i )        
{
	Assert( IsValidIndex( i ) );
	return m_Elements[i].m_Data; 
}

template < class T, typename L, class I, class M >
inline T const &CUtlRBTree<T, L, I, M>::Element( I i ) const  
{ 
	Assert( IsValidIndex( i ) );
	return m_Elements[i].m_Data; 
}

template < class T, typename L, class I, class M >
inline T &CUtlRBTree<T, L, I, M>::operator[]( I i )        
{ 
	return Element(i); 
}

template < class T, typename L, class I, class M >
inline T const &CUtlRBTree<T, L, I, M>::operator[]( I i ) const  
{ 
	return Element(i); 
}

//-----------------------------------------------------------------------------
//
// various accessors
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Gets the root
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline	I  CUtlRBTree<T, L, I, M>::Root() const             
{ 
	return m_Root; 
}

//-----------------------------------------------------------------------------
// Num elements
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline	unsigned int CUtlRBTree<T, L, I, M>::Count() const          
{ 
	return (unsigned int)m_NumElements; 
}

//-----------------------------------------------------------------------------
// Max "size" of the vector
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline	I  CUtlRBTree<T, L, I, M>::MaxElement() const       
{
	return ( I )m_Elements.NumAllocated();
}


//-----------------------------------------------------------------------------
// Gets the children                               
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline	I CUtlRBTree<T, L, I, M>::Parent( I i ) const      
{ 
	return i != InvalidIndex() ? m_Elements[i].m_Parent : InvalidIndex();
}

template < class T, typename L, class I, class M >
inline	I CUtlRBTree<T, L, I, M>::LeftChild( I i ) const   
{ 
	return i != InvalidIndex() ? m_Elements[i].m_Left : InvalidIndex();
}

template < class T, typename L, class I, class M >
inline	I CUtlRBTree<T, L, I, M>::RightChild( I i ) const  
{ 
	return i != InvalidIndex() ? m_Elements[i].m_Right : InvalidIndex();
}

//-----------------------------------------------------------------------------
// Tests if a node is a left or right child
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline	bool CUtlRBTree<T, L, I, M>::IsLeftChild( I i ) const 
{ 
	return LeftChild(Parent(i)) == i; 
}

template < class T, typename L, class I, class M >
inline	bool CUtlRBTree<T, L, I, M>::IsRightChild( I i ) const
{ 
	return RightChild(Parent(i)) == i; 
}


//-----------------------------------------------------------------------------
// Tests if root or leaf
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline	bool CUtlRBTree<T, L, I, M>::IsRoot( I i ) const     
{ 
	return i == m_Root; 
}

template < class T, typename L, class I, class M >
inline	bool CUtlRBTree<T, L, I, M>::IsLeaf( I i ) const     
{ 
	return (LeftChild(i) == InvalidIndex()) && (RightChild(i) == InvalidIndex()); 
}


//-----------------------------------------------------------------------------
// Checks if a node is valid and in the tree
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline	bool CUtlRBTree<T, L, I, M>::IsValidIndex( I i ) const 
{ 
	if ( !m_Elements.IsIdxValid( i ) )
		return false;

	return LeftChild(i) != i; 
}


//-----------------------------------------------------------------------------
// Invalid index
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline I CUtlRBTree<T, L, I, M>::InvalidIndex()         
{
	return ( I )M::InvalidIndex();
}


//-----------------------------------------------------------------------------
// returns the tree depth (not a very fast operation)
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline int CUtlRBTree<T, L, I, M>::Depth() const           
{ 
	return Depth(Root()); 
}

//-----------------------------------------------------------------------------
// Sets the children
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline void  CUtlRBTree<T, L, I, M>::SetParent( I i, I parent )       
{ 
	Links(i).m_Parent = parent; 
}

template < class T, typename L, class I, class M >
inline void  CUtlRBTree<T, L, I, M>::SetLeftChild( I i, I child  )    
{ 
	Links(i).m_Left = child; 
}

template < class T, typename L, class I, class M >
inline void  CUtlRBTree<T, L, I, M>::SetRightChild( I i, I child  )   
{ 
	Links(i).m_Right = child; 
}

//-----------------------------------------------------------------------------
// Gets at the links
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline typename CUtlRBTree<T, L, I, M>::Links_t const &CUtlRBTree<T, L, I, M>::Links( I i ) const 
{
	// Sentinel node, makes life easier
	static const Links_t s_Sentinel = 
	{ 
		// Use M::INVALID_INDEX instead of InvalidIndex() so that this is
		// a compile-time constant -- otherwise it is constructed on the first
		// call!
		M::INVALID_INDEX, M::INVALID_INDEX, M::INVALID_INDEX, CUtlRBTree<T, L, I, M>::BLACK
	};

	return (i != InvalidIndex()) ? m_Elements[i] : s_Sentinel;
}

template < class T, typename L, class I, class M >
inline typename CUtlRBTree<T, L, I, M>::Links_t &CUtlRBTree<T, L, I, M>::Links( I i )       
{ 
	Assert(i != InvalidIndex()); 
	return m_Elements[i];
}

//-----------------------------------------------------------------------------
// Checks if a link is red or black
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline bool CUtlRBTree<T, L, I, M>::IsRed( I i ) const                
{ 
	return Color( i ) == RED; 
}

template < class T, typename L, class I, class M >
inline bool CUtlRBTree<T, L, I, M>::IsBlack( I i ) const             
{ 
	return Color( i ) == BLACK; 
}


//-----------------------------------------------------------------------------
// Sets/gets node color
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
inline typename CUtlRBTree<T, L, I, M>::NodeColor_t  CUtlRBTree<T, L, I, M>::Color( I i ) const            
{ 
	return (NodeColor_t)(i != InvalidIndex() ? m_Elements[i].m_Tag : BLACK); 
}

template < class T, typename L, class I, class M >
inline void CUtlRBTree<T, L, I, M>::SetColor( I i, typename CUtlRBTree<T, L, I, M>::NodeColor_t c ) 
{ 
	Links(i).m_Tag = (I)c; 
}

//-----------------------------------------------------------------------------
// Allocates/ deallocates nodes
//-----------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4389) // '==' : signed/unsigned mismatch
#endif

template < class T, typename L, class I, class M >
I  CUtlRBTree<T, L, I, M>::NewNode( bool bConstructElement )
{
	I elem;

	// Nothing in the free list; add.
	if ( m_FirstFree == InvalidIndex() )
	{
		Assert( m_Elements.IsValidIterator( m_LastAlloc ) || m_NumElements == 0 );
		typename M::Iterator_t it = m_Elements.IsValidIterator( m_LastAlloc ) ? m_Elements.Next( m_LastAlloc ) : m_Elements.First();
		if ( !m_Elements.IsValidIterator( it ) )
		{
			MEM_ALLOC_CREDIT_CLASS();
			m_Elements.Grow();

			it = m_Elements.IsValidIterator( m_LastAlloc ) ? m_Elements.Next( m_LastAlloc ) : m_Elements.First();

			Assert( m_Elements.IsValidIterator( it ) );
			if ( !m_Elements.IsValidIterator( it ) )
			{
				Plat_FatalError( "CUtlRBTree overflow with %u elements!\n", Count() );
				DebuggerBreak();
			}
		}
		m_LastAlloc = it;
		elem = m_Elements.GetIndex( m_LastAlloc );
		Assert( m_Elements.IsValidIterator( m_LastAlloc ) );
	}
	else
	{
		elem = m_FirstFree;
		m_FirstFree = RightChild( m_FirstFree );
	}

#ifdef _DEBUG
	// reset links to invalid....
	Links_t &node = Links( elem );
	node.m_Left = node.m_Right = node.m_Parent = InvalidIndex();
#endif

	if ( bConstructElement )
		Construct( &Element( elem ) );

	return elem;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

template < class T, typename L, class I, class M >
void  CUtlRBTree<T, L, I, M>::FreeNode( I i )
{
	Assert( IsValidIndex(i) && (i != InvalidIndex()) );
	Destruct( &Element(i) );
	SetLeftChild( i, i ); // indicates it's in not in the tree
	SetRightChild( i, m_FirstFree );
	m_FirstFree = i;
}


//-----------------------------------------------------------------------------
// Rotates node i to the left
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
void CUtlRBTree<T, L, I, M>::RotateLeft(I elem) 
{
	I rightchild = RightChild(elem);
	SetRightChild( elem, LeftChild(rightchild) );
	if (LeftChild(rightchild) != InvalidIndex())
		SetParent( LeftChild(rightchild), elem );

	if (rightchild != InvalidIndex())
		SetParent( rightchild, Parent(elem) );
	if (!IsRoot(elem))
	{
		if (IsLeftChild(elem))
			SetLeftChild( Parent(elem), rightchild );
		else
			SetRightChild( Parent(elem), rightchild );
	}
	else
		m_Root = rightchild;

	SetLeftChild( rightchild, elem );
	if (elem != InvalidIndex())
		SetParent( elem, rightchild );
}


//-----------------------------------------------------------------------------
// Rotates node i to the right
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
void CUtlRBTree<T, L, I, M>::RotateRight(I elem) 
{
	I leftchild = LeftChild(elem);
	SetLeftChild( elem, RightChild(leftchild) );
	if (RightChild(leftchild) != InvalidIndex())
		SetParent( RightChild(leftchild), elem );

	if (leftchild != InvalidIndex())
		SetParent( leftchild, Parent(elem) );
	if (!IsRoot(elem))
	{
		if (IsRightChild(elem))
			SetRightChild( Parent(elem), leftchild );
		else
			SetLeftChild( Parent(elem), leftchild );
	}
	else
		m_Root = leftchild;

	SetRightChild( leftchild, elem );
	if (elem != InvalidIndex())
		SetParent( elem, leftchild );
}


//-----------------------------------------------------------------------------
// Rebalances the tree after an insertion
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
void CUtlRBTree<T, L, I, M>::InsertRebalance(I elem) 
{
	while ( !IsRoot(elem) && (Color(Parent(elem)) == RED) )
	{
		I parent = Parent(elem);
		I grandparent = Parent(parent);

		/* we have a violation */
		if (IsLeftChild(parent))
		{
			I uncle = RightChild(grandparent);
			if (IsRed(uncle)) 
			{
				/* uncle is RED */
				SetColor(parent, BLACK);
				SetColor(uncle, BLACK);
				SetColor(grandparent, RED);
				elem = grandparent;
			} 
			else 
			{
				/* uncle is BLACK */
				if (IsRightChild(elem))
				{
					/* make x a left child, will change parent and grandparent */
					elem = parent;
					RotateLeft(elem);
					parent = Parent(elem);
					grandparent = Parent(parent);
				}
				/* recolor and rotate */
				SetColor(parent, BLACK);
				SetColor(grandparent, RED);
				RotateRight(grandparent);
			}
		} 
		else 
		{
			/* mirror image of above code */
			I uncle = LeftChild(grandparent);
			if (IsRed(uncle)) 
			{
				/* uncle is RED */
				SetColor(parent, BLACK);
				SetColor(uncle, BLACK);
				SetColor(grandparent, RED);
				elem = grandparent;
			} 
			else 
			{
				/* uncle is BLACK */
				if (IsLeftChild(elem))
				{
					/* make x a right child, will change parent and grandparent */
					elem = parent;
					RotateRight(parent);
					parent = Parent(elem);
					grandparent = Parent(parent);
				}
				/* recolor and rotate */
				SetColor(parent, BLACK);
				SetColor(grandparent, RED);
				RotateLeft(grandparent);
			}
		}
	}
	SetColor( m_Root, BLACK );
}


//-----------------------------------------------------------------------------
// Insert a node into the tree
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
I CUtlRBTree<T, L, I, M>::InsertAt( I parent, bool leftchild, bool bConstructElement )
{
	I i = NewNode( bConstructElement );
	LinkToParent( i, parent, leftchild );
	++m_NumElements;

	Assert(IsValid());

	return i;
}

template < class T, typename L, class I, class M >
void CUtlRBTree<T, L, I, M>::LinkToParent( I i, I parent, bool isLeft )
{
	Links_t &elem = Links(i);
	elem.m_Parent = parent;
	elem.m_Left = elem.m_Right = InvalidIndex();
	elem.m_Tag = RED;

	/* insert node in tree */
	if (parent != InvalidIndex()) 
	{
		if (isLeft)
			Links(parent).m_Left = i;
		else
			Links(parent).m_Right = i;
	} 
	else 
	{
		m_Root = i;
	}

	InsertRebalance(i);	
}

//-----------------------------------------------------------------------------
// Rebalance the tree after a deletion
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
void CUtlRBTree<T, L, I, M>::RemoveRebalance(I elem) 
{
	while (elem != m_Root && IsBlack(elem)) 
	{
		I parent = Parent(elem);

		// If elem is the left child of the parent
		if (elem == LeftChild(parent)) 
		{
			// Get our sibling
			I sibling = RightChild(parent);
			if (IsRed(sibling)) 
			{
				SetColor(sibling, BLACK);
				SetColor(parent, RED);
				RotateLeft(parent);

				// We may have a new parent now
				parent = Parent(elem);
				sibling = RightChild(parent);
			}
			if ( (IsBlack(LeftChild(sibling))) && (IsBlack(RightChild(sibling))) ) 
			{
				if (sibling != InvalidIndex())
					SetColor(sibling, RED);
				elem = parent;
			}
			else
			{
				if (IsBlack(RightChild(sibling)))
				{
					SetColor(LeftChild(sibling), BLACK);
					SetColor(sibling, RED);
					RotateRight(sibling);

					// rotation may have changed this
					parent = Parent(elem);
					sibling = RightChild(parent);
				}
				SetColor( sibling, Color(parent) );
				SetColor( parent, BLACK );
				SetColor( RightChild(sibling), BLACK );
				RotateLeft( parent );
				elem = m_Root;
			}
		}
		else 
		{
			// Elem is the right child of the parent
			I sibling = LeftChild(parent);
			if (IsRed(sibling)) 
			{
				SetColor(sibling, BLACK);
				SetColor(parent, RED);
				RotateRight(parent);

				// We may have a new parent now
				parent = Parent(elem);
				sibling = LeftChild(parent);
			}
			if ( (IsBlack(RightChild(sibling))) && (IsBlack(LeftChild(sibling))) )
			{
				if (sibling != InvalidIndex()) 
					SetColor( sibling, RED );
				elem = parent;
			} 
			else 
			{
				if (IsBlack(LeftChild(sibling)))
				{
					SetColor( RightChild(sibling), BLACK );
					SetColor( sibling, RED );
					RotateLeft( sibling );

					// rotation may have changed this
					parent = Parent(elem);
					sibling = LeftChild(parent);
				}
				SetColor( sibling, Color(parent) );
				SetColor( parent, BLACK );
				SetColor( LeftChild(sibling), BLACK );
				RotateRight( parent );
				elem = m_Root;
			}
		}
	}
	SetColor( elem, BLACK );
}

template < class T, typename L, class I, class M >
void CUtlRBTree<T, L, I, M>::Unlink( I elem )
{
	if ( elem != InvalidIndex() )
	{
		I x, y;

		if ((LeftChild(elem) == InvalidIndex()) || 
			(RightChild(elem) == InvalidIndex()))
		{
			/* y has a NIL node as a child */
			y = elem;
		}
		else
		{
			/* find tree successor with a NIL node as a child */
			y = RightChild(elem);
			while (LeftChild(y) != InvalidIndex())
				y = LeftChild(y);
		}

		/* x is y's only child */
		if (LeftChild(y) != InvalidIndex())
			x = LeftChild(y);
		else
			x = RightChild(y);

		/* remove y from the parent chain */
		if (x != InvalidIndex())
			SetParent( x, Parent(y) );
		if (!IsRoot(y))
		{
			if (IsLeftChild(y))
				SetLeftChild( Parent(y), x );
			else
				SetRightChild( Parent(y), x );
		}
		else
			m_Root = x;

		// need to store this off now, we'll be resetting y's color
		NodeColor_t ycolor = Color(y);
		if (y != elem)
		{
			// Standard implementations copy the data around, we cannot here.
			// Hook in y to link to the same stuff elem used to.
			SetParent( y, Parent(elem) );
			SetRightChild( y, RightChild(elem) );
			SetLeftChild( y, LeftChild(elem) );

			if (!IsRoot(elem))
				if (IsLeftChild(elem))
					SetLeftChild( Parent(elem), y );
				else
					SetRightChild( Parent(elem), y );
			else
				m_Root = y;

			if (LeftChild(y) != InvalidIndex())
				SetParent( LeftChild(y), y );
			if (RightChild(y) != InvalidIndex())
				SetParent( RightChild(y), y );

			SetColor( y, Color(elem) );
		}

		if ((x != InvalidIndex()) && (ycolor == BLACK))
			RemoveRebalance(x);
	}
}

template < class T, typename L, class I, class M >
void CUtlRBTree<T, L, I, M>::Link( I elem )
{
	if ( elem != InvalidIndex() )
	{
		I parent = InvalidIndex();
		bool leftchild = false;

		FindInsertionPosition( Element( elem ), parent, leftchild );

		LinkToParent( elem, parent, leftchild );

		Assert(IsValid());
	}
}

//-----------------------------------------------------------------------------
// Delete a node from the tree
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
void CUtlRBTree<T, L, I, M>::RemoveAt(I elem) 
{
	if ( elem != InvalidIndex() )
	{
		Unlink( elem );

		FreeNode(elem);
		--m_NumElements;

		Assert(IsValid());
	}
}


//-----------------------------------------------------------------------------
// remove a node in the tree
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M > bool CUtlRBTree<T, L, I, M>::Remove( T const &search )
{
	I node = Find( search );
	if (node != InvalidIndex())
	{
		RemoveAt(node);
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------
// Removes all nodes from the tree
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
void CUtlRBTree<T, L, I, M>::RemoveAll()
{
	// Have to do some convoluted stuff to invoke the destructor on all
	// valid elements for the multilist case (since we don't have all elements
	// connected to each other in a list).

	if ( m_LastAlloc == m_Elements.InvalidIterator() )
	{
		Assert( m_Root == InvalidIndex() );
		Assert( m_NumElements == 0 );
		Assert( m_FirstFree == InvalidIndex() );
		return;
	}

	m_Elements.RemoveAll();

	// Clear everything else out
	m_Root = InvalidIndex();
	m_NumElements = 0;
	m_FirstFree = InvalidIndex();

	Assert( IsValid() );
}

//-----------------------------------------------------------------------------
// Removes all nodes from the tree and purges memory
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
void CUtlRBTree<T, L, I, M>::Purge()
{
	RemoveAll();
	m_FirstFree = InvalidIndex();
	m_Elements.Purge();
	m_LastAlloc = m_Elements.InvalidIterator();
}


//-----------------------------------------------------------------------------
// iteration
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
I CUtlRBTree<T, L, I, M>::FirstInorder() const
{
	I i = m_Root;
	I left;
	while ((left = LeftChild(i)) != InvalidIndex())
		i = left;
	return i;
}

template < class T, typename L, class I, class M >
I CUtlRBTree<T, L, I, M>::NextInorder( I i ) const
{
	Assert(IsValidIndex(i));

	I right;
	if ((right = RightChild(i)) != InvalidIndex())
	{
		i = right;
		I left;
		while ((left = LeftChild(i)) != InvalidIndex())
			i = left;
		return i;
	}

	I parent = Parent(i);
	while (IsRightChild(i))
	{
		i = parent;
		if (i == InvalidIndex()) break;
		parent = Parent(i);
	}
	return parent;
}

template < class T, typename L, class I, class M >
I CUtlRBTree<T, L, I, M>::PrevInorder( I i ) const
{
	Assert(IsValidIndex(i));

	I left, right;
	if ((left = LeftChild(i)) != InvalidIndex())
	{
		i = left;
		while ((right = RightChild(i)) != InvalidIndex())
			i = right;
		return i;
	}

	I parent = Parent(i);
	while (IsLeftChild(i))
	{
		i = parent;
		if (i == InvalidIndex()) break;
		parent = Parent(i);
	}
	return parent;
}

template < class T, typename L, class I, class M >
I CUtlRBTree<T, L, I, M>::LastInorder() const
{
	I i = m_Root;
	I right;
	while ((right = RightChild(i)) != InvalidIndex())
		i = right;
	return i;
}

template < class T, typename L, class I, class M >
I CUtlRBTree<T, L, I, M>::FirstPreorder() const
{
	return m_Root;
}

template < class T, typename L, class I, class M >
I CUtlRBTree<T, L, I, M>::NextPreorder( I i ) const
{
	I left, right;
	if ((left = LeftChild(i)) != InvalidIndex())
		return left;

	if ((right = RightChild(i)) != InvalidIndex())
		return right;

	I parent = Parent(i);
	while( parent != InvalidIndex())
	{
		if (IsLeftChild(i) && (RightChild(parent) != InvalidIndex()))
			return RightChild(parent);
		i = parent;
		parent = Parent(parent);
	}
	return InvalidIndex();
}

template < class T, typename L, class I, class M >
I CUtlRBTree<T, L, I, M>::PrevPreorder( I i ) const
{
	Assert(0);  // not implemented yet
	return InvalidIndex();
}

template < class T, typename L, class I, class M >
I CUtlRBTree<T, L, I, M>::LastPreorder() const
{
	I i = m_Root;
	while (1)
	{
		I left, right;
		while ((right = RightChild(i)) != InvalidIndex())
			i = right;

		if ((left = LeftChild(i)) != InvalidIndex())
			i = left;
		else
			break;
	}
	return i;
}

template < class T, typename L, class I, class M >
I CUtlRBTree<T, L, I, M>::FirstPostorder() const
{
	I i = m_Root;
	while (!IsLeaf(i))
	{
		I left;
		if ((left = LeftChild(i)) != InvalidIndex())
			i = left;
		else
			i = RightChild(i);
	}
	return i;
}

template < class T, typename L, class I, class M >
I CUtlRBTree<T, L, I, M>::NextPostorder( I i ) const
{
	I parent = Parent(i);
	if (parent == InvalidIndex())
		return InvalidIndex();

	if (IsRightChild(i))
		return parent;

	if (RightChild(parent) == InvalidIndex())
		return parent;

	i = RightChild(parent);
	while (!IsLeaf(i))
	{
		I left;
		if ((left = LeftChild(i)) != InvalidIndex())
			i = left;
		else
			i = RightChild(i);
	}
	return i;
}


template < class T, typename L, class I, class M >
void CUtlRBTree<T, L, I, M>::Reinsert( I elem )
{
	Unlink( elem );
	Link( elem );
}


//-----------------------------------------------------------------------------
// returns the tree depth (not a very fast operation)
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
int CUtlRBTree<T, L, I, M>::Depth( I node ) const
{
	if (node == InvalidIndex())
		return 0;

	int depthright = Depth( RightChild(node) );
	int depthleft = Depth( LeftChild(node) );
	return MAX( depthright, depthleft ) + 1;
}


//#define UTLTREE_PARANOID

//-----------------------------------------------------------------------------
// Makes sure the tree is valid after every operation
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >
bool CUtlRBTree<T, L, I, M>::IsValid() const
{
	if ( !Count() )
		return true;

	if ( m_LastAlloc == m_Elements.InvalidIterator() )
		return false;

	if ( !m_Elements.IsIdxValid( Root() ) )
		return false;

	if ( Parent( Root() ) != InvalidIndex() )
		return false;

#ifdef UTLTREE_PARANOID

	// First check to see that mNumEntries matches reality.
	// count items on the free list
	int numFree = 0;
	for ( int i = m_FirstFree; i != InvalidIndex(); i = RightChild( i ) )
	{
		++numFree;
		if ( !m_Elements.IsIdxValid( i ) )
			return false;
	}

	// iterate over all elements, looking for validity 
	// based on the self pointers
	int nElements = 0;
	int numFree2 = 0;
	for ( M::Iterator_t it = m_Elements.First(); it != m_Elements.InvalidIterator(); it = m_Elements.Next( it ) )
	{
		I i = m_Elements.GetIndex( it );
		if ( !IsValidIndex( i ) )
		{
			++numFree2;
		}
		else
		{
			++nElements;

			int right = RightChild( i );
			int left = LeftChild( i );
			if ( ( right == left ) && ( right != InvalidIndex() ) )
				return false;

			if ( right != InvalidIndex() )
			{
				if ( !IsValidIndex( right ) ) 
					return false;
				if ( Parent( right ) != i )
					return false;
				if ( IsRed( i ) && IsRed( right ) ) 
					return false;
			}

			if ( left != InvalidIndex() )
			{
				if ( !IsValidIndex( left ) ) 
					return false;
				if ( Parent( left ) != i ) 
					return false;
				if ( IsRed( i ) && IsRed( left ) ) 
					return false;
			}
		}

		if ( it == m_LastAlloc )
			break;
	}
	if ( numFree2 != numFree )
		return false;

	if ( nElements != m_NumElements )
		return false;

#endif // UTLTREE_PARANOID

	return true;
}


//-----------------------------------------------------------------------------
// Sets the less func
//-----------------------------------------------------------------------------

template < class T, typename L, class I, class M >  
void CUtlRBTree<T, L, I, M>::SetLessFunc( const typename CUtlRBTree<T, L, I, M>::LessFunc_t &func )
{
	m_LessFunc = func;
}


//-----------------------------------------------------------------------------
// inserts a node into the tree
//-----------------------------------------------------------------------------

// Inserts a node into the tree, doesn't copy the data in.
template < class T, typename L, class I, class M > 
void CUtlRBTree<T, L, I, M>::FindInsertionPosition( T const &insert, I &parent, bool &leftchild )
{
	Assert( !!m_LessFunc );

	/* find where node belongs */
	I current = m_Root;
	parent = InvalidIndex();
	leftchild = false;
	while (current != InvalidIndex()) 
	{
		parent = current;
		if (m_LessFunc( insert, Element(current) ))
		{
			leftchild = true; current = LeftChild(current);
		}
		else
		{
			leftchild = false; current = RightChild(current);
		}
	}
}

template < class T, typename L, class I, class M > 
I CUtlRBTree<T, L, I, M>::Insert( const T &insert )
{
	// use copy constructor to copy it in
	I parent = InvalidIndex();
	bool leftchild = false;
	FindInsertionPosition( insert, parent, leftchild );
	I newNode = InsertAt( parent, leftchild, false );
	CopyConstruct( &Element( newNode ), insert );
	return newNode;
}

template < class T, typename L, class I, class M > 
I CUtlRBTree<T, L, I, M>::Insert( T &&moveInsert )
{
	// use copy constructor to copy it in
	I parent = InvalidIndex();
	bool leftchild = false;
	FindInsertionPosition( moveInsert, parent, leftchild );
	I newNode = InsertAt( parent, leftchild, false );
	MoveConstruct( &Element( newNode ), Move( moveInsert ) );
	return newNode;
}


template < class T, typename L, class I, class M > 
void CUtlRBTree<T, L, I, M>::Insert( const T *pArray, int nItems )
{
	while ( nItems-- )
	{
		Insert( *pArray++ );
	}
}

template < class T, typename L, class I, class M > 
I CUtlRBTree<T, L, I, M>::InsertIfNotFound( T const &insert )
{
	// use copy constructor to copy it in
	I parent;
	bool leftchild;

	I current = m_Root;
	parent = InvalidIndex();
	leftchild = false;
	while (current != InvalidIndex()) 
	{
		parent = current;
		if (m_LessFunc( insert, Element(current) ))
		{
			leftchild = true; current = LeftChild(current);
		}
		else if (m_LessFunc( Element(current), insert ))
		{
			leftchild = false; current = RightChild(current);
		}
		else
			// Match found, no insertion
			return InvalidIndex();
	}

	I newNode = InsertAt( parent, leftchild, false );
	CopyConstruct( &Element( newNode ), insert );
	return newNode;
}

template < class T, typename L, class I, class M > 
I CUtlRBTree<T, L, I, M>::InsertIfNotFound( T &&moveInsert )
{
	// use copy constructor to copy it in
	I parent;
	bool leftchild;

	I current = m_Root;
	parent = InvalidIndex();
	leftchild = false;
	while (current != InvalidIndex()) 
	{
		parent = current;
		// Move unreferences:
		if (m_LessFunc( moveInsert, Element(current) ))
		{
			leftchild = true; current = LeftChild(current);
		}
		else if (m_LessFunc( Element(current), moveInsert ))
		{
			leftchild = false; current = RightChild(current);
		}
		else
		{
			// Match found, no insertion
			return InvalidIndex();
		}
	}

	I newNode = InsertAt( parent, leftchild, false );
	MoveConstruct( &Element( newNode ), Move( moveInsert ) );
	return newNode;
}


//-----------------------------------------------------------------------------
// finds a node in the tree
//-----------------------------------------------------------------------------
template < class T, typename L, class I, class M > 
I CUtlRBTree<T, L, I, M>::Find( T const &search ) const
{
	Assert( !!m_LessFunc );

	I current = m_Root;
	while (current != InvalidIndex()) 
	{
		if (m_LessFunc( search, Element(current) ))
			current = LeftChild(current);
		else if (m_LessFunc( Element(current), search ))
			current = RightChild(current);
		else 
			break;
	}
	return current;
}


//-----------------------------------------------------------------------------
// finds a the first node (inorder) with this key in the tree
//-----------------------------------------------------------------------------
template < class T, typename L, class I, class M >
I CUtlRBTree<T, L, I, M>::FindFirst( T const &search ) const
{
	Assert( !!m_LessFunc );

	I current = m_Root;
	I best = InvalidIndex();
	while ( current != InvalidIndex() )
	{
		if ( m_LessFunc( search, Element( current ) ) )
			current = LeftChild( current );
		else if ( m_LessFunc( Element( current ), search ) )
			current = RightChild( current );
		else
		{
			best = current;
			current = LeftChild( current );
		}
	}
	return best;
}


//-----------------------------------------------------------------------------
// finds the closest node to the key supplied
//-----------------------------------------------------------------------------
template < class T, typename L, class I, class M >
I CUtlRBTree<T, L, I, M>::FindClosest( T const &search, CompareOperands_t eFindCriteria ) const
{
	Assert( !!m_LessFunc );
	Assert( ( eFindCriteria & ( k_EGreaterThan | k_ELessThan ) ) ^ ( k_EGreaterThan | k_ELessThan ) );

	I current = m_Root;
	I best = InvalidIndex();

	while ( current != InvalidIndex() )
	{
		if ( m_LessFunc( search, Element( current ) ) )
		{
			// current node is > key
			if ( eFindCriteria & k_EGreaterThan )
				best = current;
			current = LeftChild( current );
		}
		else if ( m_LessFunc( Element( current ), search ) )
		{
			// current node is < key
			if ( eFindCriteria & k_ELessThan )
				best = current;
			current = RightChild( current );
		}
		else
		{
			// exact match
			if ( eFindCriteria & k_EEqual )
			{
				best = current;
				break;
			}
			else if ( eFindCriteria & k_EGreaterThan )
			{
				current = RightChild( current );
			}
			else if ( eFindCriteria & k_ELessThan )
			{
				current = LeftChild( current );
			}
		}
	}
	return best;
}


//-----------------------------------------------------------------------------
// swap in place
//-----------------------------------------------------------------------------
template < class T, typename L, class I, class M >
void CUtlRBTree<T, L, I, M>::Swap( CUtlRBTree< T, I, L > &that )
{
	m_Elements.Swap( that.m_Elements );
	V_swap( m_LessFunc, that.m_LessFunc );
	V_swap( m_Root, that.m_Root );
	V_swap( m_NumElements, that.m_NumElements );
	V_swap( m_FirstFree, that.m_FirstFree );
	// V_swap( m_pElements, that.m_pElements );
	V_swap( m_LastAlloc, that.m_LastAlloc );
	Assert( IsValid() );
	Assert( m_Elements.IsValidIterator( m_LastAlloc ) || ( m_NumElements == 0 && m_FirstFree == InvalidIndex() ) );
}


#endif // UTLRBTREE_H
