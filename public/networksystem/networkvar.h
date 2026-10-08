#ifndef NETWORKSYSTEM_NETWORKVAR_H
#define NETWORKSYSTEM_NETWORKVAR_H

#pragma once

#include "tier1/utlvector.h"
#include "tier1/utldelegateimpl.h"
#include "entity2/entityidentity.h"
#include "entity2/entityinstance.h"
#include "entity2/entitynetwork.h"

#include "fieldpath.h"
#include "schemasystem/schemametatag.h"

enum NetworkResolveFlags_t : uint8
{
	NETWORK_RESOLVE_PATH_RESOLVED = 1 << 0, // Is resolved
	NETWORK_RESOLVE_PATH_PENDING = 1 << 1, // Is to be resolved before its next use
	NETWORK_RESOLVE_CHILD_INDICES_QUERIED = 1 << 3, // NETWORK_RESOLVE_HAS_CHILD_INDICES holds the GetFieldPathChildIndices() result
	NETWORK_RESOLVE_HAS_CHILD_INDICES = 1 << 4,
};

// Memory policy the engine threads through every networked vector. Only the form it is
// instantiated in is known:
//
//		CUtlVector< T, int, CNetworkUtlVector_MemoryType< T, -1 > >
//
// The internals below are reconstructed - the one confirmed part is that GROW_SIZE is threaded down
// from CNetworkUtlVectorBase. Every networked-vector field observed so far passes -1.
template < class T, int GROW_SIZE = -1 >
class CNetworkUtlVector_MemoryType : public CUtlVectorMemory_Growable< T, int, GROW_SIZE >
{
};

template < class Derived, class Vector >
struct NetworkUtlVectorDerived_t
{
	typedef Derived Type;
};

template < class Vector >
struct NetworkUtlVectorDerived_t< void, Vector >
{
	typedef Vector Type;
};

// Schema field type of a networked CUtlVector. The schema system records it as, e.g.
//
//		CNetworkUtlVectorBase< int, NetworkVar_m_nBodyGroupChoices, -1, int >
//		CNetworkUtlVectorBase< CHandle< CBasePlayerPawn >, NetworkVar_m_aPlayers, -1, int >
//
// Changer is the per-field tag class the CNetworkUtlVector macro generates, exactly as for
// CNetworkVarBase. The client spells this template C_NetworkUtlVectorBase.
//
// It mirrors the CUtlVectorBase interface, but every write into an element is paired with
// NetworkStateChanged( nElement ), and every change of the element count with NetworkStateChanged():
//
//		vec.Set( i, value ); // or vec[ i ] = value;
//		vec.GetForModify( i )->m_nField = 1;
//		for ( auto element : vec.ForModify() ) element->m_nField = 1;
//		vec.AddToTail( value ); // notifies the count and the new element
//
// The storage base is not public, so no plain T & to an element is handed out. Reading through
// a non-const vector goes through CElementRef, which converts to const T & without notifying.
// Every notification follows the store. GetForModify() hands out a CElementModify, which notifies
// when it goes away - at the end of the full expression for a temporary, at the end of the scope
// for a named one - so the writes made through it are already in place.
//
// What a change hands the owner is built by GetStateChanged() of the most derived vector: a variant
// passes itself as Derived and hides GetStateChanged() with its own form, and every notification
// made by this class - the writes, the structural changes, CElementModify - picks that form up.
template < class T, class Changer, int GROW_SIZE = -1, typename I = int, class Derived = void >
class CNetworkUtlVectorBase : protected CUtlVectorBase< T, I, CNetworkUtlVector_MemoryType< T, GROW_SIZE > >
{
	typedef CUtlVectorBase< T, I, CNetworkUtlVector_MemoryType< T, GROW_SIZE > > BaseClass;
	typedef CNetworkUtlVectorBase< T, Changer, GROW_SIZE, I, Derived > ThisClass;
	typedef typename NetworkUtlVectorDerived_t< Derived, ThisClass >::Type DerivedClass;

public:
	using typename BaseClass::ElemType_t;
	using typename BaseClass::IndexType_t;
	using BaseClass::IsUtlVector;

	using BaseClass::BaseClass;

	// Write access to one element: notifies the element once it goes away, after the writes made
	// through it. Neither copyable nor movable, so exactly one notification follows the writes.
	class CElementModify
	{
	public:
		CElementModify( I nElement, ThisClass *pVector ) : m_nElement( nElement ), m_pVector( pVector ) {}
		CElementModify( const CElementModify & ) = delete;
		CElementModify &operator=( const CElementModify & ) = delete;

		~CElementModify() { m_pVector->NetworkStateChanged( m_nElement ); }

		T &Get() const { return m_pVector->BaseClass::Element( m_nElement ); }
		operator T &() const { return Get(); }
		T *operator->() const { return &Get(); }

	private:
		I m_nElement;
		ThisClass *m_pVector;
	};

	// One element of a const vector: read access only.
	class CElementConstRef
	{
	public:
		CElementConstRef( I nElement, const ThisClass *pVector ) : m_nElement( nElement ), m_pVector( pVector ) {}

		operator const T &() const { return m_pVector->Get( m_nElement ); }
		const T &Get() const { return m_pVector->Get( m_nElement ); }
		const T *operator->() const { return &Get(); }

	private:
		I m_nElement;
		const ThisClass *m_pVector;
	};

	// One element of a non-const vector. Reads do not notify, writes go through Set().
	class CElementRef
	{
	public:
		CElementRef( I nElement, ThisClass *pVector ) : m_nElement( nElement ), m_pVector( pVector ) {}

		operator const T &() const { return m_pVector->Get( m_nElement ); }
		const T &Get() const { return m_pVector->Get( m_nElement ); }
		CElementModify GetForModify() const { return m_pVector->GetForModify( m_nElement ); }

		CElementRef &operator=( const T &copySrc ) { m_pVector->Set( m_nElement, copySrc ); return *this; }
		CElementRef &operator=( T &&moveSrc ) { m_pVector->Set( m_nElement, Move( moveSrc ) ); return *this; }
		CElementRef &operator=( const CElementRef &other ) { m_pVector->Set( m_nElement, other.Get() ); return *this; }

	private:
		I m_nElement;
		ThisClass *m_pVector;
	};

	// Iterates the vector by index and dereferences to Reference - CElementConstRef, CElementRef or
	// CElementModify - built on the spot, so every element access goes through its notification rules.
	// Vector is const ThisClass for CElementConstRef.
	template < class Reference, class Vector = ThisClass >
	class CElementIterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = I;
		using pointer = void;
		using reference = Reference;

		CElementIterator( I nElement, Vector *pVector ) : m_nElement( nElement ), m_pVector( pVector ) {}

		Reference operator*() const { return Reference( m_nElement, m_pVector ); }
		Reference operator[]( I nOffset ) const { return Reference( m_nElement + nOffset, m_pVector ); }
		I Index() const { return m_nElement; }

		CElementIterator &operator++() { ++m_nElement; return *this; }
		CElementIterator operator++( int ) { CElementIterator prev = *this; ++m_nElement; return prev; }
		CElementIterator &operator--() { --m_nElement; return *this; }
		CElementIterator operator--( int ) { CElementIterator prev = *this; --m_nElement; return prev; }

		CElementIterator &operator+=( I nOffset ) { m_nElement += nOffset; return *this; }
		CElementIterator &operator-=( I nOffset ) { m_nElement -= nOffset; return *this; }
		CElementIterator operator+( I nOffset ) const { return CElementIterator( m_nElement + nOffset, m_pVector ); }
		CElementIterator operator-( I nOffset ) const { return CElementIterator( m_nElement - nOffset, m_pVector ); }
		friend CElementIterator operator+( I nOffset, const CElementIterator &it ) { return it + nOffset; }
		I operator-( const CElementIterator &other ) const { return m_nElement - other.m_nElement; }

		bool operator==( const CElementIterator &other ) const { return m_nElement == other.m_nElement; }
		bool operator!=( const CElementIterator &other ) const { return m_nElement != other.m_nElement; }
		bool operator<( const CElementIterator &other ) const { return m_nElement < other.m_nElement; }
		bool operator>( const CElementIterator &other ) const { return m_nElement > other.m_nElement; }
		bool operator<=( const CElementIterator &other ) const { return m_nElement <= other.m_nElement; }
		bool operator>=( const CElementIterator &other ) const { return m_nElement >= other.m_nElement; }

	private:
		I m_nElement;
		Vector *m_pVector;
	};

	// Reads only
	using const_iterator = CElementIterator< CElementConstRef, const ThisClass >;
	using const_reverse_iterator = std::reverse_iterator< const_iterator >;

	// Reads without notifying, assigning goes through Set()
	using iterator = CElementIterator< CElementRef >;
	using reverse_iterator = std::reverse_iterator< iterator >;

	// Every dereference is a CElementModify: the element is notified once the access goes away
	using modify_iterator = CElementIterator< CElementModify >;
	using modify_reverse_iterator = std::reverse_iterator< modify_iterator >;

	// The vector seen through modify_iterator:
	//
	//		for ( auto element : vec.ForModify() )
	//			element->m_nField = 1; // notified at the end of the iteration
	class CModifyRange
	{
	public:
		explicit CModifyRange( ThisClass *pVector ) : m_pVector( pVector ) {}

		modify_iterator begin() const { return modify_iterator( 0, m_pVector ); }
		modify_iterator end() const { return modify_iterator( m_pVector->Count(), m_pVector ); }
		modify_reverse_iterator rbegin() const { return modify_reverse_iterator( end() ); }
		modify_reverse_iterator rend() const { return modify_reverse_iterator( begin() ); }

	private:
		ThisClass *m_pVector;
	};

	ThisClass &operator=( const ThisClass &copyFrom )
	{
		BaseClass::operator=( copyFrom );
		OnReplaced();
		return *this;
	}

	ThisClass &operator=( ThisClass &&moveFrom )
	{
		BaseClass::operator=( Move( moveFrom ) );
		OnReplaced();
		return *this;
	}

	// Element access
	const T &operator[]( I i ) const { return BaseClass::operator[]( i ); }
	CElementRef operator[]( I i ) { return CElementRef( i, this ); }
	const T &Element( I i ) const { return BaseClass::Element( i ); }
	CElementRef Element( I i ) { return CElementRef( i, this ); }
	const T &Get( I i ) const { return BaseClass::Element( i ); }
	const T &Head() const { return BaseClass::Head(); }
	const T &Tail() const { return BaseClass::Tail(); }
	const T *Base() const { return BaseClass::Base(); }

	const_iterator begin() const { return const_iterator( 0, this ); }
	const_iterator end() const { return const_iterator( Count(), this ); }
	const_reverse_iterator rbegin() const { return const_reverse_iterator( end() ); }
	const_reverse_iterator rend() const { return const_reverse_iterator( begin() ); }
	iterator begin() { return iterator( 0, this ); }
	iterator end() { return iterator( Count(), this ); }
	reverse_iterator rbegin() { return reverse_iterator( end() ); }
	reverse_iterator rend() { return reverse_iterator( begin() ); }
	CModifyRange ForModify() { return CModifyRange( this ); }

	// Element writes
	void Set( I i, const T &copySrc )
	{
		BaseClass::Element( i ) = copySrc;
		NetworkStateChanged( i );
	}

	void Set( I i, T &&moveSrc )
	{
		BaseClass::Element( i ) = Move( moveSrc );
		NetworkStateChanged( i );
	}

	CElementModify GetForModify( I i ) { return CElementModify( i, this ); }

	// Queries and capacity, none of them touch the elements
	using BaseClass::Count;
	using BaseClass::IsEmpty;
	using BaseClass::IsValidIndex;
	using BaseClass::InvalidIndex;
	using BaseClass::Find;
	using BaseClass::HasElement;
	using BaseClass::EnsureCapacity;
	using BaseClass::Compact;
	using BaseClass::SetGrowSize;
	using BaseClass::NumAllocated;

	// Adds an element, uses default constructor
	I AddToHead() { return OnInserted( BaseClass::AddToHead() ); }
	I AddToTail() { return OnInserted( BaseClass::AddToTail() ); }
	I InsertBefore( I elem ) { return OnInserted( BaseClass::InsertBefore( elem ) ); }
	I InsertAfter( I elem ) { return OnInserted( BaseClass::InsertAfter( elem ) ); }

	// Adds an element, uses copy & move constructor
	I AddToHead( const T &copySrc ) { return OnInserted( BaseClass::AddToHead( copySrc ) ); }
	I AddToHead( T &&moveSrc ) { return OnInserted( BaseClass::AddToHead( Move( moveSrc ) ) ); }
	I AddToTail( const T &copySrc ) { return OnInserted( BaseClass::AddToTail( copySrc ) ); }
	I AddToTail( T &&moveSrc ) { return OnInserted( BaseClass::AddToTail( Move( moveSrc ) ) ); }
	I InsertBefore( I elem, const T &copySrc ) { return OnInserted( BaseClass::InsertBefore( elem, copySrc ) ); }
	I InsertBefore( I elem, T &&moveSrc ) { return OnInserted( BaseClass::InsertBefore( elem, Move( moveSrc ) ) ); }
	I InsertAfter( I elem, const T &copySrc ) { return OnInserted( BaseClass::InsertAfter( elem, copySrc ) ); }
	I InsertAfter( I elem, T &&moveSrc ) { return OnInserted( BaseClass::InsertAfter( elem, Move( moveSrc ) ) ); }

	// Adds multiple elements, uses default constructor
	I AddMultipleToHead( I num ) { return OnInserted( BaseClass::AddMultipleToHead( num ) ); }
	I AddMultipleToTail( I num ) { return OnInserted( BaseClass::AddMultipleToTail( num ) ); }
	I AddMultipleToTail( I num, const T *pToCopy ) { return OnInserted( BaseClass::AddMultipleToTail( num, pToCopy ) ); }
	I InsertMultipleBefore( I elem, I num ) { return OnInserted( BaseClass::InsertMultipleBefore( elem, num ) ); }
	I InsertMultipleBefore( I elem, I num, const T *pToCopy ) { return OnInserted( BaseClass::InsertMultipleBefore( elem, num, pToCopy ) ); }
	I InsertMultipleAfter( I elem, I num ) { return OnInserted( BaseClass::InsertMultipleAfter( elem, num ) ); }

	void SetSize( I size ) { SetCount( size ); }

	void SetCount( I count )
	{
		BaseClass::SetCount( count );
		OnReplaced();
	}

	void SetCountNonDestructively( I count )
	{
		I nOldCount = Count();

		BaseClass::SetCountNonDestructively( count );
		OnResized( nOldCount );
	}

	void EnsureCount( I num )
	{
		I nOldCount = Count();

		BaseClass::EnsureCount( num );
		OnResized( nOldCount );
	}

	void CopyArray( const T *pArray, I size )
	{
		BaseClass::CopyArray( pArray, size );
		OnReplaced();
	}

	void FillWithValue( const T &copySrc )
	{
		BaseClass::FillWithValue( copySrc );
		NetworkStateChangedElements( 0, Count() );
	}

	void Swap( ThisClass &vec )
	{
		BaseClass::Swap( vec );
		OnReplaced();
		vec.OnReplaced();
	}

	// Element removal
	void FastRemove( I elem )
	{
		BaseClass::FastRemove( elem );
		NetworkStateChanged();

		// The former tail moved into the hole
		if ( elem < Count() )
			NetworkStateChanged( elem );
	}

	void Remove( I elem )
	{
		BaseClass::Remove( elem );
		OnRemoved( elem );
	}

	bool FindAndRemove( const T &compSrc )
	{
		I elem = Find( compSrc );

		if ( elem == InvalidIndex() )
			return false;

		Remove( elem );

		return true;
	}

	bool FindAndFastRemove( const T &compSrc )
	{
		I elem = Find( compSrc );

		if ( elem == InvalidIndex() )
			return false;

		FastRemove( elem );

		return true;
	}

	void RemoveMultiple( I elem, I num )
	{
		BaseClass::RemoveMultiple( elem, num );
		OnRemoved( elem );
	}

	void RemoveMultipleFromHead( I num )
	{
		BaseClass::RemoveMultipleFromHead( num );
		OnRemoved( 0 );
	}

	void RemoveMultipleFromTail( I num )
	{
		BaseClass::RemoveMultipleFromTail( num );
		NetworkStateChanged();
	}

	void RemoveAll()
	{
		BaseClass::RemoveAll();
		NetworkStateChanged();
	}

	void Purge()
	{
		BaseClass::Purge();
		NetworkStateChanged();
	}

	void PurgeAndDeleteElements()
	{
		BaseClass::PurgeAndDeleteElements();
		NetworkStateChanged();
	}

	// Reordering
	void Sort( I ( * pfnCompare)( const T *, const T * ) )
	{
		BaseClass::Sort( pfnCompare );
		NetworkStateChangedElements( 0, Count() );
	}

	void Sort()
	{
		BaseClass::Sort();
		NetworkStateChangedElements( 0, Count() );
	}

	template < class F >
	void SortPredicate( F &&predicate )
	{
		BaseClass::SortPredicate( Forward< F >( predicate ) );
		NetworkStateChangedElements( 0, Count() );
	}

	void Reverse()
	{
		BaseClass::Reverse();
		NetworkStateChangedElements( 0, Count() );
	}

	// The element count changed
	void NetworkStateChanged()
	{
		Changer::template NetworkStateChanged< DerivedClass >( this, static_cast< int32 >( InvalidIndex() ) );
	}

	// One element changed
	void NetworkStateChanged( I nElement )
	{
		Changer::template NetworkStateChanged< DerivedClass >( this, static_cast< int32 >( nElement ) );
	}

	// What a change hands the owner - of one element, or of the count for InvalidIndex(): the flattened field offset and the element index. The Changer calls it on DerivedClass, so a variant hides it with its own form.
	NetworkStateChanged_t GetStateChanged( uint32 nFieldOffset, I nElement )
	{
		return NetworkStateChanged_t( nFieldOffset, static_cast< int32 >( nElement ) );
	}

protected:
	void NetworkStateChangedElements( I nFirst, I nCount )
	{
		for ( I i = nFirst; i < nFirst + nCount; i++ )
			NetworkStateChanged( i );
	}

	// Inserting shifts every element after the insertion point, so all of them changed
	I OnInserted( I elem )
	{
		NetworkStateChanged();
		NetworkStateChangedElements( elem, Count() - elem );

		return elem;
	}

	// Removing shifts every element after the removal point into place.
	void OnRemoved( I elem )
	{
		NetworkStateChanged();
		NetworkStateChangedElements( elem, Count() - elem );
	}

	// Only the tail past nOldCount is new; a shrink only changes the count
	void OnResized( I nOldCount )
	{
		NetworkStateChanged();

		if ( Count() > nOldCount )
			NetworkStateChangedElements( nOldCount, Count() - nOldCount );
	}

	void OnReplaced()
	{
		NetworkStateChanged();
		NetworkStateChangedElements( 0, Count() );
	}
};

// Placeholder name - the variant used when the path to the vector runs through one or more
// pointers, so it has to carry its own owner and field path instead of a flat offset. The engine
// records it under the plain CNetworkUtlVectorBase,
// the layout is what tells the two apart. Declare such a member with CNetworkUtlVectorChained.
//
// An element change goes out as a field path rather than an offset: the path to the vector with the
// element index appended, registered through AddChangeAccessorPath(), and the child indices the owner
// reports for the vector path as the local offsets. Until the path to the vector is resolved, or when
// the owner reports no child indices, the change falls back to marking the owner fully changed.
//
// Resolving m_PathToVector goes through the network serializer and is left to the engine; a vector it
// has not resolved yet only ever reports the full change.
template < class T, class Changer, int GROW_SIZE = -1, typename I = int >
class CNetworkUtlVectorBaseChained : public CNetworkUtlVectorBase< T, Changer, GROW_SIZE, I, CNetworkUtlVectorBaseChained< T, Changer, GROW_SIZE, I > >
{
public:
	CEntityInstance *GetOwnerEntity() const { return m_pOwnerEntity.GetObject(); }
	const CFieldPath &GetPathToVector() const { return m_PathToVector; }

	NetworkStateChanged_t GetStateChanged( uint32 nFieldOffset, I nElement )
	{
		// A count change keeps the flat offset form
		if ( nElement == this->InvalidIndex() )
			return NetworkStateChanged_t( nFieldOffset );

		CEntityInstance *pOwner = GetOwnerEntity();

		if ( !m_bResolved || !pOwner || m_PathToVector.Count() >= CFieldPath::MAX_PATH_DEPTH )
			return NetworkStateChanged_t( true );

		if ( !m_bChildIndicesQueried )
		{
			m_bChildIndicesQueried = true;
			m_bHasChildIndices = pOwner->GetFieldPathChildIndices( m_PathToVector, &m_ChildIndices );
		}

		if ( !m_bHasChildIndices )
			return NetworkStateChanged_t( true );

		CFieldPath elementPath = {};
		const int16 *pPathToVector = m_PathToVector.Base();

		for ( int i = 0; i < m_PathToVector.Count(); i++ )
			elementPath.m_Path[ i ] = pPathToVector[ i ];

		elementPath.m_Path[ m_PathToVector.Count() ] = static_cast< int16 >( nElement );
		elementPath.m_nCount = static_cast< int16 >( m_PathToVector.Count() + 1 );

		NetworkStateChanged_t data;

		data.m_nPathIndex = pOwner->AddChangeAccessorPath( elementPath );

		FOR_EACH_VEC( m_ChildIndices, i )
			data.m_LocalOffsets.AddToTail( static_cast< uint32 >( m_ChildIndices[ i ] ) );

		return data;
	}

public:
	CEntityOwnerPtr m_pOwnerEntity;
	CFieldPath m_PathToVector;

	// Filled by CEntityInstance::GetFieldPathChildIndices() for m_PathToVector
	CUtlVector< int > m_ChildIndices;

	// m_PathToVector is resolved
	bool m_bResolved;

	// Resolving failed and was reported; a later success is reported as a late resolve
	bool m_bResolveFailed;

	// m_bHasChildIndices holds the GetFieldPathChildIndices() result
	bool m_bChildIndicesQueried;
	bool m_bHasChildIndices;
};

// Same, for a vector of embedded (schema struct) elements, e.g.
//
//		CUtlVectorEmbeddedNetworkVar< EntityRenderAttribute_t, NetworkVar_m_vecRenderAttributes, -1, int >
//
// The client spells this one C_UtlVectorEmbeddedNetworkVar.
//
// Every element is chained to the owner: its CNetworkVarChainer __m_pChainEntity holds the change
// accessor path index registered for the path to the vector with the element index appended, so the
// element's own network vars reach the owner through it. A change of a whole element goes out with that
// path index, and with the child indices the owner reports for the vector path as the local offsets.
// A change of the count fires m_pArraySizeChangedDelegate before the owner hears of it.
//
// Without a resolved path or child indices an element change falls back to marking the owner fully
// changed; without an owner entity it is dropped. Resolving m_PathToVector and assigning the element
// path indices go through the network serializer and the element chainers, and are left to the engine.
template < class T, class Changer, int GROW_SIZE = -1, typename I = int >
class CUtlVectorEmbeddedNetworkVar : public CNetworkUtlVectorBase< T, Changer, GROW_SIZE, I, CUtlVectorEmbeddedNetworkVar< T, Changer, GROW_SIZE, I > >
{
public:
	CEntityInstance *GetOwnerEntity() const { return m_pOwnerEntity.GetObject(); }
	const CFieldPath &GetPathToVector() const { return m_PathToVector; }
	bool IsPathResolved() const { return ( m_nResolveFlags & NETWORK_RESOLVE_PATH_RESOLVED ) != 0; }

	// Queries the owner once per resolved path, as the engine does before reporting an element change.
	bool HasChildIndices()
	{
		CEntityInstance *pOwner = GetOwnerEntity();

		if ( !pOwner || !IsPathResolved() )
			return false;

		if ( ( m_nResolveFlags & NETWORK_RESOLVE_CHILD_INDICES_QUERIED ) == 0 )
		{
			m_nResolveFlags |= NETWORK_RESOLVE_CHILD_INDICES_QUERIED;

			if ( pOwner->GetFieldPathChildIndices( m_PathToVector, &m_ChildIndices ) )
				m_nResolveFlags |= NETWORK_RESOLVE_HAS_CHILD_INDICES;
			else
				m_nResolveFlags &= ~NETWORK_RESOLVE_HAS_CHILD_INDICES;
		}

		return ( m_nResolveFlags & NETWORK_RESOLVE_HAS_CHILD_INDICES ) != 0;
	}

	// The change accessor path index the element is chained to the owner with
	ChangeAccessorFieldPathIndex_t GetElementPathIndex( I nElement ) const
	{
		return this->Get( nElement ).__m_pChainEntity.m_PathIndex;
	}

	NetworkStateChanged_t GetStateChanged( uint32 nFieldOffset, I nElement )
	{
		if ( nElement == this->InvalidIndex() )
		{
			if ( m_pArraySizeChangedDelegate )
				( *m_pArraySizeChangedDelegate )();

			return NetworkStateChanged_t( nFieldOffset );
		}

		NetworkStateChanged_t data;

		if ( !GetOwnerEntity() )
		{
			data.m_nPathIndex = -2;

			return data;
		}

		if ( !HasChildIndices() )
			return NetworkStateChanged_t( true );

		FOR_EACH_VEC( m_ChildIndices, i )
			data.m_LocalOffsets.AddToTail( static_cast< uint32 >( m_ChildIndices[ i ] ) );

		data.m_nPathIndex = GetElementPathIndex( nElement );
		data.m_bChainedPath = 1;

		return data;
	}

public:
	CEntityOwnerPtr m_pOwnerEntity;

	// Fired on every change of the element count
	CUtlDelegate< void () > *m_pArraySizeChangedDelegate;

	CFieldPath m_PathToVector;

	// Filled by CEntityInstance::GetFieldPathChildIndices() for m_PathToVector
	CUtlVector< int > m_ChildIndices;

	// While zero the engine assigns the elements their own path indices
	uint8 m_nNetworkingFlags;

	uint8 m_nResolveFlags;
};

struct SchemaNetworkVarName_t
{
	const char *m_pszName;
	const char *m_pszType;
};

DECLARE_SCHEMA_META_TAG( MNetworkVarNames, META_TAG_ON_CLASS, META_VALUE( SchemaNetworkVarName_t ) );
DECLARE_SCHEMA_META_TAG( MNetworkOverride, META_TAG_ON_CLASS, META_VALUE( SchemaNetworkVarName_t ) );
DECLARE_SCHEMA_META_TAG( MNetworkVarTypeOverride, META_TAG_ON_CLASS, META_VALUE( SchemaNetworkVarName_t ) );

DECLARE_SCHEMA_META_TAG( MNetworkExcludeByName, META_TAG_ON_CLASS, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MNetworkExcludeByUserGroup, META_TAG_ON_CLASS, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MNetworkIncludeByName, META_TAG_ON_CLASS, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MNetworkIncludeByUserGroup, META_TAG_ON_CLASS, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MNetworkUserGroupProxy, META_TAG_ON_CLASS, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MNetworkReplayCompatField, META_TAG_ON_CLASS, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MNetworkAlias, META_TAG_ON_FIELD, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MNetworkChangeCallback, META_TAG_ON_FIELD, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MNetworkEncoder, META_TAG_ON_FIELD, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MNetworkSerializer, META_TAG_ON_FIELD, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MNetworkTypeAlias, META_TAG_ON_FIELD, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MNetworkUserGroup, META_TAG_ON_FIELD, META_VALUE( const char * ) );

DECLARE_SCHEMA_META_TAG( MNetworkBitCount, META_TAG_ON_FIELD, META_VALUE( int ) );
DECLARE_SCHEMA_META_TAG( MNetworkEncodeFlags, META_TAG_ON_FIELD, META_VALUE( int ) );
DECLARE_SCHEMA_META_TAG( MNetworkPriority, META_TAG_ON_FIELD, META_VALUE( int ) );
DECLARE_SCHEMA_META_TAG( MNetworkVarEmbeddedFieldOffsetDelta, META_TAG_ON_FIELD, META_VALUE( int ) );

DECLARE_SCHEMA_META_TAG( MNetworkMinValue, META_TAG_ON_FIELD, META_VALUE( float ) );
DECLARE_SCHEMA_META_TAG( MNetworkMaxValue, META_TAG_ON_FIELD, META_VALUE( float ) );

// The chain entity field of a network var chainer, e.g. __m_pChainEntity
DECLARE_SCHEMA_META_TAG( MNetworkVarChainer, META_TAG_ON_FIELD, META_TAG_ONLY() );

// The field path index of a change accessor, e.g. m_PathIndex
DECLARE_SCHEMA_META_TAG( MNetworkChangeAccessorFieldPathIndex, META_TAG_ON_FIELD, META_TAG_ONLY() );

#endif // NETWORKSYSTEM_NETWORKVAR_H
