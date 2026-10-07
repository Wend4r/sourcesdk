#ifndef KEYVALUES3_TABLE_H
#define KEYVALUES3_TABLE_H

#ifdef _WIN32
#pragma once
#endif

#include "kv3lib/keyvalues3.h"
#include "keyvalues3_helpers.h"

#include "tier0/memdbgon.h"

class CKeyValues3Table
{
public:

	typedef KeyValues3LowercaseHash_t	Hash_t;
	typedef KeyValues3*					Member_t;

	union Name_t
	{
		const char *m_pString;
		UtlSymLargeId_t m_iSymLarge;

		Name_t( const char* pString = nullptr ) : m_pString( pString ) {}
		Name_t( UtlSymLargeId_t iSymLarge ) : m_iSymLarge( iSymLarge ) {}

		bool IsValid() const { return m_pString != nullptr; }
	};

	enum
	{
		MEMBER_FLAG_EXTERNAL_NAME = 1 << 0,
		MEMBER_FLAG_LARGE_SYMBOL = 1 << 1,

		MEMBER_FLAGS_NONE = 0,
		MEMBER_FLAGS_ALL = MEMBER_FLAG_EXTERNAL_NAME | MEMBER_FLAG_LARGE_SYMBOL
	};
	typedef uint8 Flags_t;

	static constexpr size_t DATA_SIZE = KV3_TABLE_MAX_FIXED_MEMBERS;
	static constexpr size_t DATA_ALIGNMENT = KV3Helpers::PackAlignOf<Hash_t, Member_t, Name_t, Flags_t>();

	CKeyValues3Table( int cluster_elem = KV3_INVALID_CLUSTER_ELEMENT, int alloc_size = DATA_SIZE );
	~CKeyValues3Table() { Free(); }

	bool HasCluster() const { return m_nClusterElement != KV3_INVALID_CLUSTER_ELEMENT; }
	int GetClusterElement() const { return m_nClusterElement; }
	void SetClusterElement( int element ) { m_nClusterElement = element; }

	bool HasInvalidMemberNames() const { return m_bHasInvalidMemberNames; }
	void SetHasInvalidMemberNames( bool bValue = true ) { m_bHasInvalidMemberNames = bValue; }

	CKeyValues3TableCluster* GetCluster() const;
	CKV3Arena* GetContext() const;

	// Gets the base address (can change when adding elements!)
	void *Base() { return IsBaseStatic() ? &m_StaticBuffer : m_pDynamicBuffer; };
	Hash_t *HashesBase() { return reinterpret_cast<Hash_t *>((uint8 *)Base() + OffsetToHashesBase( GetAllocatedChunks() )); }
	Member_t *MembersBase() { return reinterpret_cast<Member_t *>((uint8 *)Base() + OffsetToMembersBase( GetAllocatedChunks() )); }
	Name_t *NamesBase() { return reinterpret_cast<Name_t *>((uint8 *)Base() + OffsetToNamesBase( GetAllocatedChunks() )); }
	Flags_t *FlagsBase() { return reinterpret_cast<Flags_t *>((uint8 *)Base() + OffsetToFlagsBase( GetAllocatedChunks() )); }

	const void *Base() const { return const_cast<CKeyValues3Table *>(this)->Base(); }
	const Hash_t *HashesBase() const { return const_cast<CKeyValues3Table *>(this)->HashesBase(); }
	const Member_t *MembersBase() const { return const_cast<CKeyValues3Table *>(this)->MembersBase(); }
	const Name_t *NamesBase() const { return const_cast<CKeyValues3Table *>(this)->NamesBase(); }
	const Flags_t *FlagsBase() const { return const_cast<CKeyValues3Table *>(this)->FlagsBase(); }

	int GetMemberCount() const { return m_nCount; }
	Member_t GetMember( KV3MemberId_t id );
	const Hash_t GetMemberHash( KV3MemberId_t id ) const;
	Member_t GetMember( KV3MemberId_t id ) const { return const_cast<CKeyValues3Table*>(this)->GetMember( id ); }
	const Name_t GetMemberName( KV3MemberId_t id ) const;
	const char *GetMemberName( const KeyValues3 *parent, KV3MemberId_t id ) const;
	Flags_t GetMemberFlags( KV3MemberId_t id ) const;
	CKV3MemberName GetKV3MemberName( const KeyValues3 *parent, KV3MemberId_t id ) const;

	void PurgeFastSearch();
	void EnableFastSearch();
	void StoreKeyName( KeyValues3 *parent, Name_t &out_buffer, Flags_t &out_flags, const char *input_string, UtlSymLargeId_t sym_id = 0, bool name_external = false );
	void EnsureMemberCapacity( int num, bool force = false, bool dont_move = false );

	KV3MemberId_t Internal_FindMember( const CKV3MemberName &name, KV3MemberId_t &next );
	KV3MemberId_t FindMember( const CKV3MemberName &name ) { KV3MemberId_t next = KV3_INVALID_MEMBER; return Internal_FindMember( name, next ); }
	KV3MemberId_t FindMember( const KeyValues3* kv ) const;
	KV3MemberId_t CreateMember( KeyValues3 *parent, const CKV3MemberName &name, bool name_external = false );

	void CopyFrom( KeyValues3 *parent, const CKeyValues3Table *src );

	void RenameMember( KeyValues3 *parent, KV3MemberId_t id, const CKV3MemberName &newName );
	void RemoveMember( KeyValues3 *parent, KV3MemberId_t id );
	void RemoveAll( KeyValues3 *parent, int new_size = 0 );

	void Free( bool clearing_context = false ) { PurgeBuffers(); }
	void PurgeContent( KeyValues3 *parent, bool bClearingContext = false );
	void PurgeBuffers();

	static constexpr size_t TotalSizeOf( int initial_size ) { return ALIGN_VALUE( TotalSizeWithoutStaticData() + TotalSizeOfData( MAX( initial_size, 0 ) ), 8 ); }
	static constexpr size_t TotalSizeOfData( int size ) { return MAX( (KV3Helpers::PackSizeOf<DATA_ALIGNMENT, Hash_t, Member_t, Name_t, Flags_t>( size )), sizeof(m_pDynamicBuffer) ); }
	static constexpr size_t TotalSizeWithoutStaticData() { return sizeof(CKeyValues3Table) - sizeof(m_StaticBuffer); }

private:
	int GetAllocatedChunks() const { return m_nAllocatedChunks; }
	bool IsBaseStatic() { return !m_bIsDynamicallySized; }

	size_t GetAllocatedBytesSize() const { return TotalSizeOfData( GetAllocatedChunks() ); }

	constexpr size_t OffsetToHashesBase( int size ) const { return 0; }
	constexpr size_t OffsetToMembersBase( int size ) const { return KV3Helpers::PackSizeOf<DATA_ALIGNMENT, Hash_t>( size ); }
	constexpr size_t OffsetToNamesBase( int size ) const { return KV3Helpers::PackSizeOf<DATA_ALIGNMENT, Hash_t, Member_t>( size ); }
	constexpr size_t OffsetToFlagsBase( int size ) const { return KV3Helpers::PackSizeOf<DATA_ALIGNMENT, Hash_t, Member_t, Name_t>( size ); }

private:
	int m_nClusterElement;
	int m_nAllocatedChunks;

	struct kv3tablefastsearch_t
	{
		kv3tablefastsearch_t() : m_ignore( false ), m_ignores_counter( 0 ) {}
		~kv3tablefastsearch_t() { Clear(); }

		void Clear()
		{
			m_ignore = false;
			m_ignores_counter = 0;
			m_member_ids.RemoveAll();
		}

		struct EmptyHashFunctor { unsigned int operator()( uint32 n ) const { return n; } };
		typedef CUtlHashtable<unsigned int, KV3MemberId_t, EmptyHashFunctor> Hashtable_t;

		bool		m_ignore;
		int8		m_ignores_counter;
		Hashtable_t	m_member_ids;
	} *m_pFastSearch;

	int m_nCount;

	uint8 m_nInitialSize;
	bool m_bIsDynamicallySized;

	bool m_bHasInvalidMemberNames;
	bool m_unk002;

	union
	{
		struct
		{
			Hash_t m_Hashes[DATA_SIZE];
			Member_t m_Members[DATA_SIZE];
			Name_t m_Names[DATA_SIZE];
			Flags_t m_Flags[DATA_SIZE];
		} m_StaticBuffer;

		void* m_pDynamicBuffer;
	};
};
COMPILE_TIME_ASSERT(sizeof(CKeyValues3Table) == 192);

#include "tier0/memdbgoff.h"

#endif // KEYVALUES3_TABLE_H
