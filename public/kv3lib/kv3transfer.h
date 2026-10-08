#ifndef KV3TRANSFER_H
#define KV3TRANSFER_H

#ifdef _WIN32
#pragma once
#endif

#include "kv3lib/keyvalues3.h"
#include "kv3lib/kv3transfer_constants.h"
#include "entitytypes.h"
#include "spawngrouptypes.h"
#include "tier0/bufferstring.h"
#include "tier0/utlscratchmemory.h"
#include "tier0/utlstring.h"
#include "tier1/utlleanvector.h"
#include "tier1/utlvector.h"

#include <type_traits>

class ISave;
class IRestore;

//-----------------------------------------------------------------------------
// A message a transfer context reports. NoteFailure reports one with severity
// KV3TRANSFER_MESSAGE_SEVERITY_ERROR, which fails the transfer.
//-----------------------------------------------------------------------------
enum KV3TransferMessageSeverity_t
{
	KV3TRANSFER_MESSAGE_SEVERITY_ERROR = 2,
};

struct KV3TransferMessage_t
{
	CBufferString m_sMessage;
	int m_nSeverity;

	// Prefixes the message; CKV3TransferContextBase::GetSourceName supplies it when empty.
	CUtlString m_sLocation;
	int m_nLine;
	int m_nColumn;

	// An owned object deleted through its second virtual; null in every message the contexts build.
	void *m_pUnk028;
};
static_assert( sizeof( KV3TransferMessage_t ) == 0x30 );

//-----------------------------------------------------------------------------
// Errors and the interfaces registered for a transfer. There is no virtual destructor.
//-----------------------------------------------------------------------------
class CKV3TransferContextBase
{
public:
	CKV3TransferContextBase( const char *pszSourceName = "" );

	// Appends "[<location>: ]<message>" as a new line of the error message; an error fails the transfer.
	virtual void ReportMessage( const KV3TransferMessage_t &message );

	// Appends "<location>[(<line>[,<column>])]|<context path>", dropping the "|" without a context path,
	// then pszSuffix when anything was appended. Returns whether anything was appended.
	virtual bool FormatMessageLocation( const KV3TransferMessage_t &message, CBufferString &sOut, const char *pszSuffix );

	// pLineAndColumn holds the line and the column of the message and may be updated.
	virtual const char *GetSourceName( int *pLineAndColumn ) { return m_pszSourceName; }

	// Appends the path to the value being transferred. Returns false when there is none.
	virtual bool AppendContextPath( CBufferString &sOut ) { return false; }

	virtual int Unk_04() { return -1; }
	virtual void Unk_05() {}

	void NoteFailure( PRINTF_FORMAT_STRING const char *pszFormat, ... ) FMTFUNCTION( 2, 3 );

	KV3TransferResult_t GetResult() const { return m_Result; }
	const char *GetErrorMessage() const { return m_sErrorMessage.Get(); }

	// Lets a caller try a transfer without failing the outer one: SaveResult resets the result,
	// RestoreResult puts the saved one back unless the transfer failed meanwhile.
	struct ResultState_t
	{
		KV3TransferResult_t m_Result;
		int m_nErrorMessageLength;
	};

	void SaveResult( ResultState_t *pState )
	{
		pState->m_Result = m_Result;
		pState->m_nErrorMessageLength = m_sErrorMessage.Length();

		m_Result = KV3TRANSFER_SUCCESS;
	}

	void RestoreResult( const ResultState_t &state )
	{
		if ( m_Result == KV3TRANSFER_SUCCESS )
			m_Result = state.m_Result;
	}

	// The error lines reported since SaveResult.
	const char *GetErrorMessageSince( const ResultState_t &state ) const;

	// Interfaces are keyed by the hash of their name, "IKV3TransferInterface_*" in the game.
	// Missing a required interface fails the transfer with "Missing required transfer interface: %s".
	void *FindInterface( const CKV3MemberName &name, bool bOptional );
	void AddInterface( const CKV3MemberName &name, void *pInterface );

	template < class TInterface >
	TInterface *FindInterface( const CKV3MemberName &name, bool bOptional )
	{
		return static_cast< TInterface * >( FindInterface( name, bOptional ) );
	}

protected:
	KV3TransferResult_t m_Result;
	bool m_bUnk00C;

	CBufferString m_sErrorMessage;
	const char *m_pszSourceName;

	// Parallel arrays: the name hash and the interface.
	CUtlVectorFixedGrowable< uint32, 16 > m_InterfaceNames;
	CUtlVectorFixedGrowable< void *, 16 > m_Interfaces;
};
static_assert( sizeof( CKV3TransferContextBase ) == 0x118 );

//-----------------------------------------------------------------------------
// Value helpers. CKV3TransferValHelper< T > saves and loads a T; a class saves itself through
// KV3TransferSave and loads itself through KV3TransferLoad.
//-----------------------------------------------------------------------------
template < typename T >
struct CKV3TransferValHelper;

template < typename TEnum >
const char *KV3Transfer_EnumeratorNameFromValue( TEnum nValue )
{
	if constexpr ( KV3Transfer_EnumHelpers_t< TEnum >::is_present )
	{
		const KV3Transfer_EnumHelpers_StringPairList_t< TEnum > list = KV3Transfer_EnumHelpers_t< TEnum >::Pairs();

		for ( size_t i = 0; i < list.m_nCount; ++i )
		{
			if ( list.m_pList[ i ].m_nEnum == nValue )
				return list.m_pList[ i ].m_pString;
		}
	}

	return nullptr;
}

template < typename TEnum >
bool KV3Transfer_EnumeratorValueFromName( const char *pszEnumeratorName, TEnum *pOutValue )
{
	if constexpr ( KV3Transfer_EnumHelpers_t< TEnum >::is_present )
	{
		const KV3Transfer_EnumHelpers_StringPairList_t< TEnum > list = KV3Transfer_EnumHelpers_t< TEnum >::Pairs();

		for ( size_t i = 0; i < list.m_nCount; ++i )
		{
			if ( !V_stricmp_fast( list.m_pList[ i ].m_pString, pszEnumeratorName ) )
			{
				*pOutValue = list.m_pList[ i ].m_nEnum;
				return true;
			}
		}

		*pOutValue = KV3Transfer_EnumHelpers_t< TEnum >::ENUM_DEFAULT;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Builds a KeyValues3 tree from objects.
//-----------------------------------------------------------------------------
class CKV3TransferSaveContext : public CKV3TransferContextBase
{
public:
	CKV3TransferSaveContext( bool bOptionalInterfaces = true );
	explicit CKV3TransferSaveContext( ISave *pSave );

	using CKV3TransferContextBase::FindInterface;

	// Missing interfaces are fine when the context was created with bOptionalInterfaces.
	void *FindInterface( const CKV3MemberName &name ) { return FindInterface( name, m_bOptionalInterfaces ); }

	KeyValues3 *TargetObject() { return m_pTargetObject; }
	int TargetDepth() const { return m_TargetStack.Count(); }
	ISave *GetSave() const { return m_pSave; }

	// Accumulates the 16-byte aligned size of the arrays saved, so that a load can reserve its block allocator.
	uint64 GetBlockAllocationSize() const { return m_nBlockAllocationSize; }
	void AddBlockAllocationSize( uint64 nSize ) { m_nBlockAllocationSize += nSize; }

	// True without an ISave. Otherwise false when the ISave virtual at slot 10, unidentified, returns true,
	// unless nFlags is 2.
	bool CanSaveField( int nFlags ) const;

	// The test of every schema field: with an ISave, a field holding its default value is skipped
	// unless m_bSaveDefaultValues is set.
	bool ShouldSaveField( bool bDefaultValue ) const { return !m_pSave || ( CanSaveField( 0 ) && ( m_bSaveDefaultValues || !bDefaultValue ) ); }

	// Fails on a member that was already saved, and then returns it.
	KeyValues3 *CreateTargetMember( const CKV3MemberName &name );

	template < typename T >
	void SaveValueToMember( const CKV3MemberName &name, const T &sourceValue )
	{
		SaveValueDirect( sourceValue, CreateTargetMember( name ) );
	}

	template < typename T >
	void SaveValueDirect( const T &sourceValue, KeyValues3 *pSaveToValue );

	template < typename T >
	void SaveClassPointer( const T *pClassInstance, KeyValues3 *pSaveToValue )
	{
		if ( !pClassInstance )
		{
			pSaveToValue->SetToNull();
			return;
		}

		const KV3TransferClassBehavior_t nClassBehavior = static_cast< KV3TransferClassBehavior_t >( T::KV3TRANSFER_BEHAVIOR );

		CBufferStringN< KV3TRANSFER_CLASSNAME_MAX_LENGTH > sPolymorphicClassName;

		if constexpr ( T::KV3TRANSFER_BEHAVIOR == KV3TRANSFER_CLASS_AS_POLYMORPHIC_TABLE )
		{
			T::KV3TransferPolymorphicClassname( pClassInstance, sPolymorphicClassName );
		}

		if ( !PrepareTargetForClass( pSaveToValue, nClassBehavior, sPolymorphicClassName.Get() ) )
			return;

		PushTarget( pSaveToValue );

		if ( m_TargetStack.Count() > KV3TRANSFER_MAX_STACK_DEPTH )
			NoteFailure( "Stack depth limit hit (%d)", m_TargetStack.Count() );
		else
			pClassInstance->KV3TransferSave( this );

		PopTarget();
	}

	// Empties pObjectValue for the class: a table, a table with the class name, or null.
	bool PrepareTargetForClass( KeyValues3 *pObjectValue, KV3TransferClassBehavior_t nClassBehavior, const char *pszPolymorphicClassName );

	void PushTarget( KeyValues3 *pTarget );
	void PopTarget();

private:
	uint64 m_nBlockAllocationSize;

	// Always the top of m_TargetStack, or null.
	KeyValues3 *m_pTargetObject;
	CUtlLeanVector< KeyValues3 * > m_TargetStack;

	bool m_bOptionalInterfaces;
	ISave *m_pSave;
	bool m_bSaveDefaultValues;
};
static_assert( sizeof( CKV3TransferSaveContext ) == 0x150 );

//-----------------------------------------------------------------------------
// Fills objects from a KeyValues3 tree.
//-----------------------------------------------------------------------------
class CKV3TransferLoadContext : public CKV3TransferContextBase
{
public:
	// A context path entry holds a value instead of a name when its upper half is one of these.
	enum ContextPathTag_t : uint32
	{
		CONTEXT_PATH_TAG_INDEX = 0xFFFFFFF0, // "[%u]"
		CONTEXT_PATH_TAG_NAME_HASH = 0xFFFFFFF1, // "{0x%x}", a member whose name is empty
	};

	union ContextPathEntry_t
	{
		const char *m_pszName;

		struct
		{
			uint32 m_nValue;
			uint32 m_nTag; // ContextPathTag_t
		};
	};

	CKV3TransferLoadContext();
	explicit CKV3TransferLoadContext( const char *pszSourceName );
	explicit CKV3TransferLoadContext( IRestore *pRestore );

	// "member.member[index]{0xhash}"
	bool AppendContextPath( CBufferString &sOut ) override;

	using CKV3TransferContextBase::FindInterface;

	void *FindInterface( const CKV3MemberName &name ) { return FindInterface( name, false ); }

	IRestore *GetRestore() const { return m_pRestore; }

	// Owned by the caller. Null unless the caller sets one.
	CUtlScratchMemoryPool *GetBlockAllocator() const { return m_pBlockAllocator; }
	void SetBlockAllocator( CUtlScratchMemoryPool *pBlockAllocator ) { m_pBlockAllocator = pBlockAllocator; }

	void *AllocBlock( int nCount, int nElementSize ) { return m_pBlockAllocator->AllocAligned( nCount * nElementSize, 16 ); }

	const KeyValues3 *SourceObject() const { return m_pSourceObject; }
	int SourceDepth() const { return m_SourceStack.Count(); }

	void PushSource( const KeyValues3 *pSource );
	void PopSource();

	void PushContextPath( const CKV3MemberName &name );
	void PushContextPath( const char *pszName );
	void PushContextPathIndex( int nIndex );
	void PopContextPath() { m_ContextPath.RemoveMultipleFromTail( 1 ); }

	const KeyValues3 *FindSourceMember( const CKV3MemberName &name ) const { return m_pSourceObject->FindMember( name ); }

	// True unless nFlags is 1 and the IRestore virtual at slot 6, unidentified, returns true.
	bool CanLoadField( int nFlags ) const;

	// The member of the class being loaded. Null for a null source; fails for a source that is not a table.
	const KeyValues3 *FindClassMember( const CKV3MemberName &name );

	// The load of every schema field. A missing member gets defaultValue, the value loading the field of a
	// default constructed object gives, unless an IRestore is set, which keeps the current value.
	// fnLoad( const KeyValues3 * ) loads the member.
	template < typename T, typename TLoad >
	void LoadSchemaField( const CKV3MemberName &name, T &value, const T &defaultValue, TLoad &&fnLoad )
	{
		if ( m_pRestore && !CanLoadField( 0 ) )
			return;

		const KeyValues3 *pMember = FindClassMember( name );

		if ( pMember )
		{
			PushContextPath( name );
			fnLoad( pMember );
			PopContextPath();
		}
		else if ( !m_pRestore )
		{
			value = defaultValue;
		}
	}

	template < typename T >
	void LoadSchemaField( const CKV3MemberName &name, T &value, const T &defaultValue )
	{
		LoadSchemaField( name, value, defaultValue, [ this, &value ]( const KeyValues3 *pMember ) { LoadValueDirect( value, pMember ); } );
	}

	// A missing member loads from a null value, so that the members of a class still get their defaults.
	template < typename T >
	void LoadValueFromMember( const CKV3MemberName &name, T &destValue )
	{
		const KeyValues3 *pLoadFromMember = FindSourceMember( name );

		if ( pLoadFromMember )
		{
			PushContextPath( name );
			LoadValueDirect( destValue, pLoadFromMember );
			PopContextPath();
		}
		else
		{
			KeyValues3 nullValue;
			LoadValueDirect( destValue, &nullValue );
		}
	}

	template < typename T >
	void LoadValueFromMemberIfPresent( const CKV3MemberName &name, T &destValue )
	{
		const KeyValues3 *pLoadFromMember = FindSourceMember( name );

		if ( !pLoadFromMember )
			return;

		PushContextPath( name );
		LoadValueDirect( destValue, pLoadFromMember );
		PopContextPath();
	}

	template < typename T >
	void LoadValueFromMemberOrDefault( const CKV3MemberName &name, T &destValue, const char *pszDefaultValue )
	{
		const KeyValues3 *pLoadFromMember = FindSourceMember( name );

		if ( pLoadFromMember )
		{
			PushContextPath( name );
			LoadValueDirect( destValue, pLoadFromMember );
			PopContextPath();
		}
		else
		{
			LoadDefaultDirect( destValue, pszDefaultValue );
		}
	}

	template < typename T >
	void LoadValueDirect( T &destValue, const KeyValues3 *pLoadFromValue );

	template < typename T >
	void LoadDefaultDirect( T &destValue, const char *pszDefaultValue );

	// The caller owns the loaded object. A null value loads a null pointer.
	template < typename T >
	void LoadOwningPointer( T *&value, const KeyValues3 *pLoadFromValue )
	{
		Assert( pLoadFromValue );

		if ( pLoadFromValue->IsNull() )
		{
			value = nullptr;
			return;
		}

		if constexpr ( T::KV3TRANSFER_BEHAVIOR == KV3TRANSFER_CLASS_AS_POLYMORPHIC_TABLE )
		{
			const char *pszClassName = pLoadFromValue->GetMemberString( KV3TRANSFER_CLASSNAME_MEMBER );

			if ( !pszClassName[ 0 ] )
			{
				value = nullptr;
				NoteFailure( "Tried to load a polymorphic pointer with no '%s' key", KV3TRANSFER_CLASSNAME_MEMBER );
				return;
			}

			value = T::KV3TransferAllocateClassInstance( pszClassName, nullptr );

			if ( !value )
			{
				NoteFailure( "Failed to allocate an instance of class '%s'", pszClassName );
				return;
			}
		}
		else
		{
			static_assert( T::KV3TRANSFER_BEHAVIOR != KV3TRANSFER_CLASS_UNIMPLEMENTED, "The class does not support KV3 transfer" );

			value = T::KV3TransferAllocateClassInstance( nullptr, nullptr );

			if ( !value )
			{
				NoteFailure( "Failed to allocate an instance of a class" );
				return;
			}
		}

		LoadClassInstance( value, pLoadFromValue );
	}

	template < typename T >
	void LoadClassInstance( T *pClassInstance, const KeyValues3 *pNestedValue )
	{
		PushSource( pNestedValue );

		if ( m_SourceStack.Count() > KV3TRANSFER_MAX_STACK_DEPTH )
			NoteFailure( "Stack depth limit hit (%d)", m_SourceStack.Count() );
		else
			pClassInstance->KV3TransferLoad( this );

		PopSource();
	}

private:
	bool m_bUnk118;
	CUtlScratchMemoryPool *m_pBlockAllocator;

	// Always the top of m_SourceStack, or null.
	const KeyValues3 *m_pSourceObject;
	CUtlLeanVector< const KeyValues3 * > m_SourceStack;

	IRestore *m_pRestore;

	CUtlLeanVector< ContextPathEntry_t > m_ContextPath;
};
static_assert( sizeof( CKV3TransferLoadContext ) == 0x158 );
static_assert( sizeof( CKV3TransferLoadContext::ContextPathEntry_t ) == sizeof( uint64 ) );

//-----------------------------------------------------------------------------
// T: a class or an enum.
//-----------------------------------------------------------------------------
template < typename T >
struct CKV3TransferValHelper
{
	static_assert( std::is_class_v< T > || std::is_enum_v< T >, "No KV3 transfer for this type" );

	static void SaveValue( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const T &value )
	{
		if constexpr ( std::is_class_v< T > )
		{
			pContext->SaveClassPointer( &value, pSaveToValue );
		}
		else
		{
			const char *pszEnumeratorName = KV3Transfer_EnumeratorNameFromValue( value );

			if ( pszEnumeratorName )
				pSaveToValue->SetString( pszEnumeratorName );
			else if constexpr ( sizeof( T ) == sizeof( uint64 ) )
				pSaveToValue->SetInt64( static_cast< int64 >( value ) );
			else
				pSaveToValue->SetInt( static_cast< int32 >( value ) );
		}
	}

	static void LoadValue( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, T &value )
	{
		if constexpr ( std::is_class_v< T > )
		{
			pContext->LoadClassInstance( &value, pLoadFromValue );
		}
		else
		{
			if ( pLoadFromValue->IsString() && KV3Transfer_EnumeratorValueFromName( pLoadFromValue->GetString(), &value ) )
				return;

			value = static_cast< T >( pLoadFromValue->GetInt64() );
		}
	}

	static void LoadDefault( CKV3TransferLoadContext *pContext, const char *pszDefault, T &value )
	{
		static_assert( std::is_enum_v< T >, "No default value for a class" );

		if ( !KV3Transfer_EnumeratorValueFromName( pszDefault, &value ) )
			value = static_cast< T >( V_atoi64( pszDefault ) );
	}
};

// T*: an owning pointer. Cycles and shared objects are not supported.
template < typename T >
struct CKV3TransferValHelper< T * >
{
	static void SaveValue( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, T *const &value )
	{
		pContext->SaveClassPointer( static_cast< const T * >( value ), pSaveToValue );
	}

	static void LoadValue( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, T *&value )
	{
		pContext->LoadOwningPointer( value, pLoadFromValue );
	}
};

#define DECLARE_KV3_TRANSFER_HELPER_FOR_SIMPLE_TYPE( type_name, set_func, get_func, default_parser ) \
	template <> \
	struct CKV3TransferValHelper< type_name > \
	{ \
		static void SaveValue( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const type_name &value ) { pSaveToValue->set_func( value ); } \
		static void LoadValue( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, type_name &value ) { value = static_cast< type_name >( pLoadFromValue->get_func() ); } \
		static void LoadDefault( CKV3TransferLoadContext *pContext, const char *pszDefault, type_name &value ) { value = static_cast< type_name >( default_parser( pszDefault ) ); } \
	};

inline bool KV3Transfer_StringToBool( const char *pszValue )
{
	return V_StringToBool( pszValue, false );
}

DECLARE_KV3_TRANSFER_HELPER_FOR_SIMPLE_TYPE( bool, SetBool, GetBool, KV3Transfer_StringToBool )
DECLARE_KV3_TRANSFER_HELPER_FOR_SIMPLE_TYPE( char, SetChar, GetChar, V_atoi )
DECLARE_KV3_TRANSFER_HELPER_FOR_SIMPLE_TYPE( int8, SetInt8, GetInt8, V_atoi )
DECLARE_KV3_TRANSFER_HELPER_FOR_SIMPLE_TYPE( uint8, SetUInt8, GetUInt8, V_atoi )
DECLARE_KV3_TRANSFER_HELPER_FOR_SIMPLE_TYPE( int16, SetShort, GetShort, V_atoi )
DECLARE_KV3_TRANSFER_HELPER_FOR_SIMPLE_TYPE( uint16, SetUShort, GetUShort, V_atoi )
DECLARE_KV3_TRANSFER_HELPER_FOR_SIMPLE_TYPE( int32, SetInt, GetInt, V_atoi )
DECLARE_KV3_TRANSFER_HELPER_FOR_SIMPLE_TYPE( uint32, SetUInt, GetUInt, V_atoi64 )
DECLARE_KV3_TRANSFER_HELPER_FOR_SIMPLE_TYPE( int64, SetInt64, GetInt64, V_atoi64 )
DECLARE_KV3_TRANSFER_HELPER_FOR_SIMPLE_TYPE( uint64, SetUInt64, GetUInt64, V_atoui64 )
DECLARE_KV3_TRANSFER_HELPER_FOR_SIMPLE_TYPE( float32, SetFloat, GetFloat, V_atof )
DECLARE_KV3_TRANSFER_HELPER_FOR_SIMPLE_TYPE( float64, SetDouble, GetDouble, V_atof )

template <>
struct CKV3TransferValHelper< CUtlString >
{
	static void SaveValue( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const CUtlString &value ) { pSaveToValue->SetString( value.Get() ); }
	static void LoadValue( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, CUtlString &value ) { pLoadFromValue->GetValueAsString( &value ); }
	static void LoadDefault( CKV3TransferLoadContext *pContext, const char *pszDefault, CUtlString &value ) { value = pszDefault; }
};

//-----------------------------------------------------------------------------
// Interfaces the game registers for the types whose value depends on the running game. AddInterface keys them
// by their class name. A missing interface fails the transfer unless the value is the empty one.
//-----------------------------------------------------------------------------
#define KV3TRANSFER_INTERFACE_NAME( classname ) CKV3MemberName( #classname )

class IKV3TransferInterface_GameTime_Save
{
public:
	// Returns the value saved for a game time. The default implementation returns flGameTime; a game save
	// stores the time relative to the save, 0 as -FLT_MAX / 2 and +-FLT_MAX unchanged.
	virtual float64 ToSavedTime( float64 flGameTime ) = 0;
};

class IKV3TransferInterface_GameTime_Load
{
public:
	// Returns the game time for a saved value; the inverse of IKV3TransferInterface_GameTime_Save::ToSavedTime.
	virtual float64 FromSavedTime( float64 flSavedTime ) = 0;
};

class IKV3TransferInterface_EHandle_Save
{
public:
	// The default implementation saves the handle value as a uint64.
	virtual void Save( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const CEntityHandle *pHandle ) = 0;
};

class IKV3TransferInterface_EHandle_Load
{
public:
	// The default implementation loads an integer handle value; any other value loads an invalid handle.
	// The game passes null for pUnk.
	virtual void Load( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, CEntityHandle *pHandle, void *pUnk ) = 0;
};

class IKV3TransferInterface_WorldGroupId_Save
{
public:
	virtual void Save( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const WorldGroupId_t *pWorldGroupId ) = 0;
};

class IKV3TransferInterface_WorldGroupId_Load
{
public:
	virtual void Load( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, WorldGroupId_t *pWorldGroupId ) = 0;
};

// Saved as a double through the interface; a zero time saves null without one.
template <>
struct CKV3TransferValHelper< GameTime_t >
{
	static void SaveValue( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const GameTime_t &value )
	{
		auto pInterface = static_cast< IKV3TransferInterface_GameTime_Save * >( pContext->FindInterface( KV3TRANSFER_INTERFACE_NAME( IKV3TransferInterface_GameTime_Save ), value.GetTime() == 0.0f ) );

		if ( !pInterface )
		{
			pSaveToValue->SetToNull();
			return;
		}

		pSaveToValue->SetDouble( pInterface->ToSavedTime( value.GetTime() ) );
	}

	// Null loads zero, and so does a value without the interface.
	static void LoadValue( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, GameTime_t &value )
	{
		float64 flTime = 0.0;

		if ( !pLoadFromValue->IsNull() )
		{
			auto pInterface = static_cast< IKV3TransferInterface_GameTime_Load * >( pContext->FindInterface( KV3TRANSFER_INTERFACE_NAME( IKV3TransferInterface_GameTime_Load ), false ) );

			if ( pInterface )
				flTime = pInterface->FromSavedTime( pLoadFromValue->GetDouble() );
		}

		value.SetTime( static_cast< float >( flTime ) );
	}
};

// An invalid handle saves null and loads from null.
template <>
struct CKV3TransferValHelper< CEntityHandle >
{
	static void SaveValue( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const CEntityHandle &value )
	{
		if ( !value.IsValid() )
		{
			pSaveToValue->SetToNull();
			return;
		}

		auto pInterface = static_cast< IKV3TransferInterface_EHandle_Save * >( pContext->FindInterface( KV3TRANSFER_INTERFACE_NAME( IKV3TransferInterface_EHandle_Save ), false ) );

		if ( pInterface )
			pInterface->Save( pContext, pSaveToValue, &value );
	}

	// Without the interface the handle keeps its value.
	static void LoadValue( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, CEntityHandle &value )
	{
		if ( pLoadFromValue->IsNull() )
		{
			value = CEntityHandle();
			return;
		}

		auto pInterface = static_cast< IKV3TransferInterface_EHandle_Load * >( pContext->FindInterface( KV3TRANSFER_INTERFACE_NAME( IKV3TransferInterface_EHandle_Load ), false ) );

		if ( pInterface )
			pInterface->Load( pContext, pLoadFromValue, &value, nullptr );
	}
};

// WorldGroupId_t is a CUtlStringToken, so it has functions instead of a value helper.
// The invalid id saves null and loads from null.
inline void KV3Transfer_SaveWorldGroupId( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const WorldGroupId_t &value )
{
	auto pInterface = static_cast< IKV3TransferInterface_WorldGroupId_Save * >( pContext->FindInterface( KV3TRANSFER_INTERFACE_NAME( IKV3TransferInterface_WorldGroupId_Save ), value == WorldGroupId_t( ~0u ) ) );

	if ( pInterface )
		pInterface->Save( pContext, pSaveToValue, &value );
	else
		pSaveToValue->SetToNull();
}

// Without the interface the id loads as 0.
inline void KV3Transfer_LoadWorldGroupId( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, WorldGroupId_t &value )
{
	if ( pLoadFromValue->IsNull() )
	{
		value = WorldGroupId_t( ~0u );
		return;
	}

	WorldGroupId_t worldGroupId( 0u );

	auto pInterface = static_cast< IKV3TransferInterface_WorldGroupId_Load * >( pContext->FindInterface( KV3TRANSFER_INTERFACE_NAME( IKV3TransferInterface_WorldGroupId_Load ), false ) );

	if ( pInterface )
		pInterface->Load( pContext, pLoadFromValue, &worldGroupId );

	value = worldGroupId;
}

template <>
struct CKV3TransferValHelper< KeyValues3 >
{
	static void SaveValue( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const KeyValues3 &value ) { pSaveToValue->CopyFrom( value ); }
	static void LoadValue( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, KeyValues3 &value ) { value.CopyFrom( pLoadFromValue ); }
};

#define DECLARE_KV3_TRANSFER_HELPER_FOR_VECTOR_TYPE( type_name, set_func, get_func, default_parser ) \
	template <> \
	struct CKV3TransferValHelper< type_name > \
	{ \
		static void SaveValue( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const type_name &value ) { pSaveToValue->set_func( value ); } \
		static void LoadValue( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, type_name &value ) { value = pLoadFromValue->get_func(); } \
		static void LoadDefault( CKV3TransferLoadContext *pContext, const char *pszDefault, type_name &value ) { default_parser( pszDefault, value ); } \
	};

DECLARE_KV3_TRANSFER_HELPER_FOR_VECTOR_TYPE( Vector, SetVector, GetVector, V_StringToVector )
DECLARE_KV3_TRANSFER_HELPER_FOR_VECTOR_TYPE( Vector2D, SetVector2D, GetVector2D, V_StringToVector2D )
DECLARE_KV3_TRANSFER_HELPER_FOR_VECTOR_TYPE( Vector4D, SetVector4D, GetVector4D, V_StringToVector4D )
DECLARE_KV3_TRANSFER_HELPER_FOR_VECTOR_TYPE( QAngle, SetQAngle, GetQAngle, V_StringToQAngle )
DECLARE_KV3_TRANSFER_HELPER_FOR_VECTOR_TYPE( Quaternion, SetQuaternion, GetQuaternion, V_StringToQuaternion )
DECLARE_KV3_TRANSFER_HELPER_FOR_VECTOR_TYPE( Color, SetColor, GetColor, V_StringToColor )

template <>
struct CKV3TransferValHelper< matrix3x4_t >
{
	static void SaveValue( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const matrix3x4_t &value ) { pSaveToValue->SetMatrix3x4( value ); }
	static void LoadValue( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, matrix3x4_t &value ) { value = pLoadFromValue->GetMatrix3x4(); }
};

// Arrays push the element index to the context path of a load.
template < typename TVector >
struct CKV3TransferValHelperVector
{
	static void SaveValue( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const TVector &value )
	{
		const int nCount = value.Count();

		pSaveToValue->SetArrayElementCount( nCount );

		for ( int i = 0; i < nCount; ++i )
			pContext->SaveValueDirect( value[ i ], pSaveToValue->GetArrayElement( i ) );
	}

	static void LoadValue( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, TVector &value )
	{
		const int nCount = pLoadFromValue->IsArray() ? pLoadFromValue->GetArrayElementCount() : 0;

		value.SetCount( nCount );

		for ( int i = 0; i < nCount; ++i )
		{
			pContext->PushContextPathIndex( i );
			pContext->LoadValueDirect( value[ i ], pLoadFromValue->GetArrayElement( i ) );
			pContext->PopContextPath();
		}
	}
};

template < typename T, typename I, typename A >
struct CKV3TransferValHelper< CUtlVector< T, I, A > > : CKV3TransferValHelperVector< CUtlVector< T, I, A > > {};

template < typename T, int N >
struct CKV3TransferValHelper< CUtlVectorFixedGrowable< T, N > > : CKV3TransferValHelperVector< CUtlVectorFixedGrowable< T, N > > {};

template < typename T, typename I, typename A >
struct CKV3TransferValHelper< CUtlLeanVector< T, I, A > > : CKV3TransferValHelperVector< CUtlLeanVector< T, I, A > > {};

// T[N]: missing elements load from null values.
template < typename T, size_t N >
struct CKV3TransferValHelper< T[ N ] >
{
	static void SaveValue( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const T ( &value )[ N ] )
	{
		pSaveToValue->SetArrayElementCount( N );

		for ( size_t i = 0; i < N; ++i )
			pContext->SaveValueDirect( value[ i ], pSaveToValue->GetArrayElement( i ) );
	}

	static void LoadValue( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, T ( &value )[ N ] )
	{
		const size_t nCount = pLoadFromValue->IsArray() ? MIN( static_cast< size_t >( pLoadFromValue->GetArrayElementCount() ), N ) : 0;

		size_t i = 0;

		for ( ; i < nCount; ++i )
		{
			pContext->PushContextPathIndex( static_cast< int >( i ) );
			pContext->LoadValueDirect( value[ i ], pLoadFromValue->GetArrayElement( i ) );
			pContext->PopContextPath();
		}

		for ( ; i < N; ++i )
		{
			KeyValues3 nullValue;
			pContext->LoadValueDirect( value[ i ], &nullValue );
		}
	}
};

// char[N] transfers as a string.
template < size_t N >
struct CKV3TransferValHelper< char[ N ] >
{
	static void SaveValue( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const char ( &value )[ N ] ) { pSaveToValue->SetString( value ); }
	static void LoadValue( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, char ( &value )[ N ] ) { pLoadFromValue->GetValueAsString( value, N ); }
	static void LoadDefault( CKV3TransferLoadContext *pContext, const char *pszDefault, char ( &value )[ N ] ) { V_strncpy( value, pszDefault, N ); }
};

template < typename T >
void CKV3TransferSaveContext::SaveValueDirect( const T &sourceValue, KeyValues3 *pSaveToValue )
{
	CKV3TransferValHelper< T >::SaveValue( this, pSaveToValue, sourceValue );
}

template < typename T >
void CKV3TransferLoadContext::LoadValueDirect( T &destValue, const KeyValues3 *pLoadFromValue )
{
	CKV3TransferValHelper< T >::LoadValue( this, pLoadFromValue, destValue );
}

template < typename T >
void CKV3TransferLoadContext::LoadDefaultDirect( T &destValue, const char *pszDefaultValue )
{
	CKV3TransferValHelper< T >::LoadDefault( this, pszDefaultValue, destValue );
}

#endif // KV3TRANSFER_H
