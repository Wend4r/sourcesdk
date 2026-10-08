#ifndef KEYVALUES3_H
#define KEYVALUES3_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/dbg.h"
#include "tier0/bufferstring.h"
#include "tier0/strtools.h"
#include "tier0/utlbuffer.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlstring.h"
#include "tier1/generichash.h"
#include "tier1/utlhashtable.h"
#include "tier1/utlmap.h"
#include "tier1/utlsymbollarge.h"
#include "mathlib/vector4d.h"
#include "color.h"
#include "bitvec.h"
#include "entityhandle.h"

#include <cstddef>
#include <type_traits>

#include "tier0/memdbgon.h"

#include "tier0/keyvalues3.h"

class KeyValues3;
class CKeyValues3Array;
class CKeyValues3Table;
class CKV3Arena;
class CKV3ArenaImpl;
struct KV3MetaData_t;
struct KV3BinaryBlob_t;
struct KV1ToKV3Translation_t;
struct KV3ToKV1Translation_t;

/* 
	KeyValues3 is a data storage format. See https://developer.valvesoftware.com/wiki/KeyValues3
	Supports various specific data types targeted at the Source2.
	Each specific type corresponds to one of the basic types.

	There are 2 ways to create KeyValues3:

	1. Via CKV3Arena:
	- KV's, arrays and tables are stored in fixed memory blocks (clusters) and therefore memory is allocated only when clusters are created.
	- Supports metadata and some other things.

	2. Directly through the constructor.
*/

// Quick way to iterate across whole kv3, to access currently iterated kv3 use iter.Get()
// Mostly useful to iterate unnamed data, like arrays of primitives
#define FOR_EACH_KV3( kv, iter ) \
	for ( CKeyValues3Iterator iter( kv ); iter.IsValid(); iter.Advance() )

using KV3MemberId_t = int32;
#define KV3_INVALID_MEMBER ((KV3MemberId_t)-1)

#define KV3_INVALID_CLUSTER_ELEMENT (~0)

enum
{
	KV3_ARRAY_MAX_FIXED_MEMBERS = 6,
	KV3_TABLE_MAX_FIXED_MEMBERS = 8,

	KV3_CONTEXT_SIZE = 4608,

	KV3_ARRAY_INIT_SIZE = 32,
	KV3_TABLE_INIT_SIZE = 64,

	KV3_CLUSTER_MAX_ELEMENTS = 253
};

enum KV3Type_t : uint8
{
	KV3_TYPE_INVALID = 0,
	KV3_TYPE_NULL,
	KV3_TYPE_BOOL,
	KV3_TYPE_INT,
	KV3_TYPE_UINT,
	KV3_TYPE_DOUBLE,
	KV3_TYPE_STRING,
	KV3_TYPE_BINARY_BLOB,
	KV3_TYPE_ARRAY,
	KV3_TYPE_TABLE,

	KV3_TYPE_COUNT,
};

enum KV3TypeOpt_t : uint8
{
	KV3_TYPEOPT_NONE = 0,
	
	KV3_TYPEOPT_STRING_SHORT,
	KV3_TYPEOPT_STRING_EXTERN,
	
	KV3_TYPEOPT_BINARY_BLOB_EXTERN,
	
	KV3_TYPEOPT_ARRAY_FLOAT32,
	KV3_TYPEOPT_ARRAY_FLOAT64,
	KV3_TYPEOPT_ARRAY_INT16,
	KV3_TYPEOPT_ARRAY_INT32,
	KV3_TYPEOPT_ARRAY_UINT8_SHORT,
	KV3_TYPEOPT_ARRAY_INT16_SHORT,
};

enum KV3TypeEx_t : uint8
{
	KV3_TYPEEX_INVALID = 0,
	KV3_TYPEEX_NULL,
	KV3_TYPEEX_BOOL,
	KV3_TYPEEX_INT,
	KV3_TYPEEX_UINT,
	KV3_TYPEEX_DOUBLE,

	KV3_TYPEEX_STRING			= KV3_TYPE_STRING,
	KV3_TYPEEX_STRING_SHORT		= (KV3_TYPEEX_STRING|(KV3_TYPEOPT_STRING_SHORT << 4)),
	KV3_TYPEEX_STRING_EXTERN	= (KV3_TYPEEX_STRING|(KV3_TYPEOPT_STRING_EXTERN << 4)),

	KV3_TYPEEX_BINARY_BLOB			= KV3_TYPE_BINARY_BLOB,
	KV3_TYPEEX_BINARY_BLOB_EXTERN	= (KV3_TYPEEX_BINARY_BLOB|(KV3_TYPEOPT_BINARY_BLOB_EXTERN << 4)),

	KV3_TYPEEX_ARRAY				= KV3_TYPE_ARRAY,
	KV3_TYPEEX_ARRAY_FLOAT32		= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_FLOAT32 << 4)),
	KV3_TYPEEX_ARRAY_FLOAT64		= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_FLOAT64 << 4)),
	KV3_TYPEEX_ARRAY_INT16			= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_INT16 << 4)),
	KV3_TYPEEX_ARRAY_INT32			= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_INT32 << 4)),
	KV3_TYPEEX_ARRAY_UINT8_SHORT	= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_UINT8_SHORT << 4)),
	KV3_TYPEEX_ARRAY_INT16_SHORT	= (KV3_TYPEEX_ARRAY|(KV3_TYPEOPT_ARRAY_INT16_SHORT << 4)),

	KV3_TYPEEX_TABLE = KV3_TYPE_TABLE,
};

enum KV3SubType_t : uint8
{
	KV3_SUBTYPE_INVALID = 0,

	// string types
	KV3_SUBTYPE_RESOURCE,
	KV3_SUBTYPE_RESOURCE_NAME,
	KV3_SUBTYPE_PANORAMA,
	KV3_SUBTYPE_SOUNDEVENT,
	KV3_SUBTYPE_SUBCLASS, // table type
	KV3_SUBTYPE_ENTITY_NAME, // string type
	KV3_SUBTYPE_LOCALIZE,

	KV3_SUBTYPE_UNSPECIFIED,
	KV3_SUBTYPE_NULL,
	KV3_SUBTYPE_BINARY_BLOB,
	KV3_SUBTYPE_ARRAY,
	KV3_SUBTYPE_TABLE,
	KV3_SUBTYPE_BOOL8,
	KV3_SUBTYPE_CHAR8,
	KV3_SUBTYPE_UCHAR32,
	KV3_SUBTYPE_INT8,
	KV3_SUBTYPE_UINT8,
	KV3_SUBTYPE_INT16,
	KV3_SUBTYPE_UINT16,
	KV3_SUBTYPE_INT32,
	KV3_SUBTYPE_UINT32,
	KV3_SUBTYPE_INT64,
	KV3_SUBTYPE_UINT64,
	KV3_SUBTYPE_FLOAT32,
	KV3_SUBTYPE_FLOAT64,
	KV3_SUBTYPE_STRING,
	KV3_SUBTYPE_POINTER,
	KV3_SUBTYPE_COLOR32,

	// vector types
	KV3_SUBTYPE_VECTOR,
	KV3_SUBTYPE_VECTOR2D,
	KV3_SUBTYPE_VECTOR4D,
	KV3_SUBTYPE_ROTATION_VECTOR,
	KV3_SUBTYPE_QUATERNION,
	KV3_SUBTYPE_QANGLE,
	KV3_SUBTYPE_MATRIX3X4,
	KV3_SUBTYPE_TRANSFORM,

	KV3_SUBTYPE_STRING_TOKEN,
	KV3_SUBTYPE_EHANDLE,

	KV3_SUBTYPE_COUNT,
};

enum KV3ArrayAllocType_t
{
	KV3_ARRAY_ALLOC_EXTERN = 0,
	KV3_ARRAY_ALLOC_NORMAL = 1,
	KV3_ARRAY_ALLOC_EXTERN_FREE = 2,
};

enum KV3ToStringFlags_t
{
	KV3_TO_STRING_NONE = 0,
	KV3_TO_STRING_DONT_CLEAR_BUFF = (1 << 0),
	KV3_TO_STRING_DONT_APPEND_STRINGS = (1 << 1),
	KV3_TO_STRING_APPEND_ONLY_NUMERICS = (1 << 2),
	KV3_TO_STRING_RETURN_NON_NUMERICS = (1 << 3),
};

enum KV3MetaDataFlags_t
{
	KV3_METADATA_MULTILINE_STRING = (1 << 0),
	KV3_METADATA_SINGLE_QUOTED_STRING = (1 << 1),
};

enum KeyValues3Flag_t : uint8
{
	KEYVALUES3_FLAG_NONE = 0,
	KEYVALUES3_FLAG_RESOURCE_REFERENCE = (1 << 0),
	KEYVALUES3_FLAG_MULTILINE_STRING = (1 << 1),
	KEYVALUES3_FLAG_LAST_VALUE = (1 << 2)
};

union KeyValues3Array_t
{
	float32* m_f32;
	Vector *m_vec;
	Vector2D *m_vec2;
	Vector4D *m_vec4;
	Quaternion *m_quat;
	QAngle *m_ang;
	matrix3x4_t *m_mat;
	float64* m_f64;
	int16* m_i16;
	int32* m_i32;
	uint8 m_u8Short[8];
	int16 m_i16Short[4];

	CKeyValues3Array* m_pRoot;
};

using KeyValues3LowercaseHash_t = CUtlStringToken;
using CKV3MemberHash = KeyValues3LowercaseHash_t;

class CKV3MemberName : public CKV3MemberHash
{
public:
	template< uintp N > constexpr CKV3MemberName( const char (&szInit)[N] ) : CKV3MemberHash( szInit ), m_iSymLarge( UTL_INVAL_SYMBOL_LARGE ), m_pszString( (const char *)szInit ) {}
	CKV3MemberName( const char *pszString, int nLen ) : CKV3MemberHash( MakeStringToken2( pszString, nLen ) ), m_iSymLarge( UTL_INVAL_SYMBOL_LARGE ), m_pszString( pszString ) {}
	CKV3MemberName( uint32 nHash = 0, UtlSymLargeId_t index = UTL_INVAL_SYMBOL_LARGE, const char* pszString = StringFuncs<char>::EmptyString() ) : CKV3MemberHash( nHash ), m_iSymLarge( index ), m_pszString( pszString ) {}
	CKV3MemberName( const CKV3MemberName &other ) = default;

	CKV3MemberName &operator=( const CKV3MemberName &other )
	{
		CKV3MemberHash::operator=( other );
		m_iSymLarge = other.m_iSymLarge;
		m_pszString = other.m_pszString;

		return *this;
	}

	using CKV3MemberHash::operator==;
	using CKV3MemberHash::operator!=;
	bool operator==( const CKV3MemberName &other ) const { return GetHashCode() == other.GetHashCode(); }
	bool operator!=( const CKV3MemberName &other ) const { return !operator==( other ); }

	static CKV3MemberName Make( const char *pszInit, int nLen = -1 )
	{
		Assert( pszInit && pszInit[0] );

		return CKV3MemberName( pszInit, nLen );
	}

	static CKV3MemberName Make( std::string_view view )
	{
		AssertMsg(view.data()[view.size()] == '\0', "string_view must reference a null-terminated string");

		return CKV3MemberName( view.data(), static_cast<int>(view.length()) );
	}

	UtlSymLargeId_t GetSymLargeId() const { return m_iSymLarge; }
	const char *GetString() const { return m_pszString; }
	bool IsEmpty() const { return !m_pszString || !m_pszString[0]; }

private:
	UtlSymLargeId_t m_iSymLarge;
	const char *m_pszString;
};

using CKeyValues3StringAndHash = CKV3MemberName;

// Pulse thing
class CKV3MemberNameWithStorage : public CKV3MemberName
{
public:
	template< uintp N > constexpr CKV3MemberNameWithStorage( const char (&szInit)[N] ) : CKV3MemberName( szInit ), m_Storage( (const char*)szInit, N - 1 ) {}
	CKV3MemberNameWithStorage( const char* pszString, int nLen ): CKV3MemberName( pszString, nLen ), m_Storage( pszString, nLen ) {}
	CKV3MemberNameWithStorage( uint32 nHash = 0, UtlSymLargeId_t index = 0, const char* pszString = StringFuncs<char>::EmptyString(), int nLen = -1  ) : CKV3MemberName( nHash, index, pszString ), m_Storage( pszString, nLen ) {}

	// Assigning a name keeps the storage
	using CKV3MemberName::operator=;

	const CBufferString &GetStorage() const { return m_Storage; }

	// An empty name has the hash 0
	void Set( const char *pszString )
	{
		m_Storage.Set( pszString ? pszString : "" );

		const uint32 nHash = ( pszString && *pszString ) ? MakeStringToken( pszString ) : 0;

		*this = CKV3MemberName( nHash, GetSymLargeId(), m_Storage.Get() );
	}

private:
	CBufferStringN< 32 > m_Storage;
};
COMPILE_TIME_ASSERT( sizeof( CKV3MemberNameWithStorage ) == 56 );

// Pulse thing
using CKV3MemberNameSet = KeyValues3; // Allocates with KV_TYPE_ARRAY.

template<size_t SIZE, typename T>
class CKeyValues3ClusterImpl;

using CKeyValues3Cluster = CKeyValues3ClusterImpl<KV3_CLUSTER_MAX_ELEMENTS, KeyValues3>;
using CKeyValues3TableCluster = CKeyValues3ClusterImpl<KV3_TABLE_INIT_SIZE, CKeyValues3Table>;
using CKeyValues3ArrayCluster = CKeyValues3ClusterImpl<KV3_ARRAY_INIT_SIZE, CKeyValues3Array>;

class KeyValues3
{
public:
	KeyValues3( KV3TypeEx_t type = KV3_TYPEEX_NULL, KV3SubType_t subtype = KV3_SUBTYPE_UNSPECIFIED );
	KeyValues3( int cluster_elem, KV3TypeEx_t type, KV3SubType_t subtype );
	KeyValues3( const KeyValues3& other ) : KeyValues3() { CopyFrom( other ); }
	~KeyValues3();

	KeyValues3& operator=( const KeyValues3& copyFrom );

	// Assigning a value replaces the current type, e.g. kv[ "name" ][ "value" ] = "text";
	KeyValues3 &operator=( std::nullptr_t ) { SetToNull(); return *this; }
	KeyValues3 &operator=( bool value ) { SetBool( value ); return *this; }
	KeyValues3 &operator=( int32 value ) { SetInt( value ); return *this; }
	KeyValues3 &operator=( uint32 value ) { SetUInt( value ); return *this; }
	KeyValues3 &operator=( int64 value ) { SetInt64( value ); return *this; }
	KeyValues3 &operator=( uint64 value ) { SetUInt64( value ); return *this; }
	KeyValues3 &operator=( float32 value ) { SetFloat( value ); return *this; }
	KeyValues3 &operator=( float64 value ) { SetDouble( value ); return *this; }
	KeyValues3 &operator=( const char *pString ) { SetString( pString ); return *this; }
	KeyValues3 &operator=( CUtlStringToken token ) { SetStringToken( token ); return *this; }
	KeyValues3 &operator=( CEntityHandle ehandle ) { SetEHandle( ehandle ); return *this; }
	KeyValues3 &operator=( const Color &color ) { SetColor( color ); return *this; }
	KeyValues3 &operator=( const Vector &vec ) { SetVector( vec ); return *this; }
	KeyValues3 &operator=( const Vector2D &vec2d ) { SetVector2D( vec2d ); return *this; }
	KeyValues3 &operator=( const Vector4D &vec4d ) { SetVector4D( vec4d ); return *this; }
	KeyValues3 &operator=( const Quaternion &quat ) { SetQuaternion( quat ); return *this; }
	KeyValues3 &operator=( const QAngle &ang ) { SetQAngle( ang ); return *this; }
	KeyValues3 &operator=( const matrix3x4_t &matrix ) { SetMatrix3x4( matrix ); return *this; }

	void CopyFrom( const KeyValues3& other );
	void CopyFrom( const KeyValues3 *pOther ) { CopyFrom( *pOther ); }
	void OverlayKeysFrom( const KeyValues3 &other, bool depth = false );

	CKV3Arena* GetContext() const;
	CKV3Arena *GetParentContext() const { return GetContext(); }
	KV3MetaData_t* GetMetaData( CKV3Arena** ppCtx = nullptr ) const;

	// Metadata exists only for values allocated in an arena with metadata enabled
	bool HasMetadata() const { return GetMetaData() != nullptr; }
	int Metadata_GetLineNumber() const;
	int Metadata_GetColumnNumber() const;

	bool HasFlag( KeyValues3Flag_t flag ) const { return (m_nFlags & flag) != 0; }
	bool HasAnyFlags() const { return m_nFlags != 0; }
	KeyValues3Flag_t GetAllFlags() const { return (KeyValues3Flag_t)m_nFlags; }
	void SetAllFlags( KeyValues3Flag_t flags ) { m_nFlags |= flags; }
	void SetFlag( KeyValues3Flag_t flag, bool state = true )
	{
		if(state)
			m_nFlags |= flag;
		else
			m_nFlags &= ~flag;
	}

	KV3Type_t GetType() const		{ return ( KV3Type_t )( m_TypeEx & 0xF ); }
	KV3TypeEx_t GetTypeEx() const	{ return ( KV3TypeEx_t )m_TypeEx; }
	KV3SubType_t GetSubType() const	{ return ( KV3SubType_t )m_SubType; }

	bool HasInvalidMemberNames() const;
	void SetHasInvalidMemberNames( bool bValue = true );

	const char* GetTypeAsString() const;
	const char* GetSubTypeAsString() const;

	const char* ToString( CBufferString& buff, uint flags = KV3_TO_STRING_NONE ) const;

	bool IsNull() const { return GetType() == KV3_TYPE_NULL; }
	void SetToNull() { PrepareForType( KV3_TYPEEX_NULL, KV3_SUBTYPE_NULL ); }

	bool IsArray() const { return GetType() == KV3_TYPE_ARRAY; }
	bool IsKV3Array() const { return GetTypeEx() == KV3_TYPEEX_ARRAY; }
	bool IsTable() const { return GetType() == KV3_TYPE_TABLE; }
	bool IsString() const { return GetType() == KV3_TYPE_STRING; }

	bool GetBool( bool defaultValue = false ) const			{ return GetValue<bool>( defaultValue ); }
	char8 GetChar( char8 defaultValue = 0 ) const			{ return GetValue<char8>( defaultValue ); }
	uchar32 GetUChar32( uchar32 defaultValue = 0 ) const	{ return GetValue<uint32>( defaultValue ); }
	int8 GetInt8( int8 defaultValue = 0 ) const				{ return GetValue<int8>( defaultValue ); }
	uint8 GetUInt8( uint8 defaultValue = 0 ) const			{ return GetValue<uint8>( defaultValue ); }
	int16 GetShort( int16 defaultValue = 0 ) const			{ return GetValue<int16>( defaultValue ); }
	uint16 GetUShort( uint16 defaultValue = 0 ) const		{ return GetValue<uint16>( defaultValue ); }
	int32 GetInt( int32 defaultValue = 0 ) const			{ return GetValue<int32>( defaultValue ); }
	uint32 GetUInt( uint32 defaultValue = 0 ) const			{ return GetValue<uint32>( defaultValue ); }
	int64 GetInt64( int64 defaultValue = 0 ) const			{ return GetValue<int64>( defaultValue ); }
	uint64 GetUInt64( uint64 defaultValue = 0 ) const		{ return GetValue<uint64>( defaultValue ); }
	float32 GetFloat( float32 defaultValue = 0.0f ) const	{ return GetValue<float32>( defaultValue ); }
	float64 GetDouble( float64 defaultValue = 0.0 ) const	{ return GetValue<float64>( defaultValue ); }

	void SetBool( bool value )		{ SetValue<bool>( value, KV3_TYPEEX_BOOL, KV3_SUBTYPE_BOOL8 ); }
	void SetChar( char8 value )		{ SetValue<char8>( value, KV3_TYPEEX_INT, KV3_SUBTYPE_CHAR8 ); }
	void SetUChar32( uchar32 value ){ SetValue<uint32>( value, KV3_TYPEEX_UINT, KV3_SUBTYPE_UCHAR32 ); }
	void SetInt8( int8 value )		{ SetValue<int8>( value, KV3_TYPEEX_INT, KV3_SUBTYPE_INT8 ); }
	void SetUInt8( uint8 value )	{ SetValue<uint8>( value, KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT8 ); }
	void SetShort( int16 value )	{ SetValue<int16>( value, KV3_TYPEEX_INT, KV3_SUBTYPE_INT16 ); }
	void SetUShort( uint16 value )	{ SetValue<uint16>( value, KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT16 ); }
	void SetInt( int32 value )		{ SetValue<int32>( value, KV3_TYPEEX_INT, KV3_SUBTYPE_INT32 ); }
	void SetUInt( uint32 value )	{ SetValue<uint32>( value, KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT32 ); }
	void SetInt64( int64 value )	{ SetValue<int64>( value, KV3_TYPEEX_INT, KV3_SUBTYPE_INT64 ); }
	void SetUInt64( uint64 value )	{ SetValue<uint64>( value, KV3_TYPEEX_UINT, KV3_SUBTYPE_UINT64 ); }
	void SetFloat( float32 value )	{ SetValue<float32>( value, KV3_TYPEEX_DOUBLE, KV3_SUBTYPE_FLOAT32 ); }
	void SetDouble( float64 value )	{ SetValue<float64>( value, KV3_TYPEEX_DOUBLE, KV3_SUBTYPE_FLOAT64 ); }

	void* GetPointer( void *defaultValue = ( void* )0 ) const { return ( GetSubType() == KV3_SUBTYPE_POINTER ) ? ( void* )m_Data.m_UInt : defaultValue; }
	void SetPointer( void* ptr ) { SetValue<uint64>( ( uint64 )ptr, KV3_TYPEEX_UINT, KV3_SUBTYPE_POINTER ); }
	
	CUtlStringToken GetStringToken( CUtlStringToken defaultValue = CUtlStringToken() ) const { return ( GetSubType() == KV3_SUBTYPE_STRING_TOKEN ) ? CUtlStringToken( ( uint32 )m_Data.m_UInt ) : defaultValue; }
	void SetStringToken( CUtlStringToken token ) { SetValue<uint32>( token.GetHashCode(), KV3_TYPEEX_UINT, KV3_SUBTYPE_STRING_TOKEN ); }

	CEntityHandle GetEHandle( CEntityHandle defaultValue = CEntityHandle() ) const { return ( GetSubType() == KV3_SUBTYPE_EHANDLE ) ? CEntityHandle( ( uint32 )m_Data.m_UInt ) : defaultValue; }
	void SetEHandle( CEntityHandle ehandle ) { SetValue<uint32>( ehandle.ToInt(), KV3_TYPEEX_UINT, KV3_SUBTYPE_EHANDLE ); }

	const char* GetString( const char *defaultValue = "" ) const;
	void SetString( const char* pString, KV3SubType_t subtype = KV3_SUBTYPE_STRING );
	void SetStringExternal( const char* pString, KV3SubType_t subtype = KV3_SUBTYPE_STRING );
	
	const byte* GetBinaryBlob() const;
	int GetBinaryBlobSize() const;
	void SetToBinaryBlob( const byte* blob, int size );
	void SetToBinaryBlobExternal( const byte* blob, int size, bool free_mem );
	void SetToZeroedBinaryBlob( int size );
	const byte *GetBinaryBlobBase() const { return GetBinaryBlob(); }
	byte GetBinaryBlobByte( int index ) const { Assert( index >= 0 && index < GetBinaryBlobSize() ); return GetBinaryBlob()[ index ]; }

	Color GetColor( const Color &defaultValue = Color( 0, 0, 0, 255 ) ) const;
	void SetColor( const Color &color );

	Vector GetVector( const Vector &defaultValue = Vector( 0.0f, 0.0f, 0.0f ) ) const						{ return GetVecBasedObj<Vector>( 3, defaultValue ); }
	Vector2D GetVector2D( const Vector2D &defaultValue = Vector2D( 0.0f, 0.0f ) ) const						{ return GetVecBasedObj<Vector2D>( 2, defaultValue ); }
	Vector4D GetVector4D( const Vector4D &defaultValue = Vector4D( 0.0f, 0.0f, 0.0f, 0.0f ) ) const			{ return GetVecBasedObj<Vector4D>( 4, defaultValue ); }
	Quaternion GetQuaternion( const Quaternion &defaultValue = Quaternion( 0.0f, 0.0f, 0.0f, 0.0f ) ) const	{ return GetVecBasedObj<Quaternion>( 4, defaultValue ); }
	QAngle GetQAngle( const QAngle &defaultValue = QAngle( 0.0f, 0.0f, 0.0f ) ) const						{ return GetVecBasedObj<QAngle>( 3, defaultValue ); }
	matrix3x4_t GetMatrix3x4( const matrix3x4_t &defaultValue = matrix3x4_t( Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ) ) ) const { return GetVecBasedObj<matrix3x4_t>( 3*4, defaultValue ); }

	void SetVector( const Vector &vec )				{ SetVecBasedObj<Vector>( vec, 3, KV3_SUBTYPE_VECTOR ); }
	void SetVector2D( const Vector2D &vec2d )		{ SetVecBasedObj<Vector2D>( vec2d, 2, KV3_SUBTYPE_VECTOR2D ); }
	void SetVector4D( const Vector4D &vec4d )		{ SetVecBasedObj<Vector4D>( vec4d, 4, KV3_SUBTYPE_VECTOR4D ); }
	void SetQuaternion( const Quaternion &quat )	{ SetVecBasedObj<Quaternion>( quat, 4, KV3_SUBTYPE_QUATERNION ); }
	void SetQAngle( const QAngle &ang )				{ SetVecBasedObj<QAngle>( ang, 3, KV3_SUBTYPE_QANGLE ); }
	void SetMatrix3x4( const matrix3x4_t &matrix )	{ SetVecBasedObj<matrix3x4_t>( matrix, 3*4, KV3_SUBTYPE_MATRIX3X4 ); }

	// Reads a numeric array or a space separated string, zero-fills the tail and returns false if the element count differs
	bool GetValueFloatArray( int count, float32 *pOutValues ) const { return ReadArrayFloat32( count, pOutValues ); }
	void SetValueFloatArray( int count, const float32 *pValues ) { NormalizeArray< float32 >( KV3_TYPEEX_DOUBLE, KV3_SUBTYPE_FLOAT32, count, pValues, false ); }

	template < typename T > T GetValueAsNumeric() const { return GetValue< T >( T() ); }
	void GetValueAsString( char *pOutData, int nBufSize ) const { CBufferStringN< 128 > buff; V_strncpy( pOutData, ToString( buff ), nBufSize ); }
	void GetValueAsString( CUtlString *pOutString ) const { CBufferStringN< 128 > buff; pOutString->Set( ToString( buff ) ); }

	bool GetValueBool() const { return GetBool(); }
	int32 GetValueInt() const { return GetInt(); }
	int64 GetValueInt64() const { return GetInt64(); }
	uint64 GetValueUint64() const { return GetUInt64(); }
	float32 GetValueFloat() const { return GetFloat(); }
	float64 GetValueDouble() const { return GetDouble(); }
	const char *GetValueString( const char *defaultValue = "" ) const { return GetString( defaultValue ); }
	bool GetValueVector( Vector *pOutValue ) const { return ReadArrayFloat32( 3, pOutValue->Base() ); }
	bool GetValueVector2D( Vector2D *pOutValue ) const { return ReadArrayFloat32( 2, pOutValue->Base() ); }
	bool GetValueVector4D( Vector4D *pOutValue ) const { return ReadArrayFloat32( 4, pOutValue->Base() ); }
	bool GetValueQAngle( QAngle *pOutValue ) const { return ReadArrayFloat32( 3, pOutValue->Base() ); }
	bool GetValueQuaternion( Quaternion *pOutValue ) const { return ReadArrayFloat32( 4, pOutValue->Base() ); }
	bool GetValueMatrix( matrix3x4_t *pOutValue ) const { return ReadArrayFloat32( 3*4, pOutValue->Base() ); }

	void SetValueResourceString( const char *pString ) { SetString( pString, KV3_SUBTYPE_RESOURCE ); }

	KeyValues3Array_t *GetArray() { return IsArray() ? &m_Data.m_Array : nullptr; }
	const KeyValues3Array_t *GetArray() const { return const_cast<KeyValues3 *>(this)->GetArray(); };
	CKeyValues3Array *GetKV3Array() { return IsKV3Array() ? m_Data.m_Array.m_pRoot : nullptr; }
	const CKeyValues3Array *GetKV3Array() const { return const_cast<KeyValues3 *>(this)->GetKV3Array(); };

	int GetArrayElementCount() const;
	void SetArrayElementCount( int count, KV3TypeEx_t type = KV3_TYPEEX_NULL, KV3SubType_t subtype = KV3_SUBTYPE_UNSPECIFIED );

	int GetArrayLength() const { return GetArrayElementCount(); }

	// Keeps the elements when this is already an array of the requested length
	void EnsureIsAnyArray( int count ) { if ( !IsArray() || GetArrayElementCount() != count ) SetArrayElementCount( count ); }

	void SetToEmptyKV3Array() { PrepareForType( KV3_TYPEEX_ARRAY, KV3_SUBTYPE_ARRAY ); }
	void SetToEmptyArray() { SetToEmptyKV3Array(); }
	KeyValues3** GetArrayBase();

	KeyValues3* GetArrayElement( int elem );
	const KeyValues3 *GetArrayElement( int elem ) const { return const_cast<KeyValues3 *>(this)->GetArrayElement( elem ); }

	KeyValues3* ArrayInsertElementBefore( int elem );
	KeyValues3* ArrayInsertElementAfter( int elem ) { return ArrayInsertElementBefore( elem + 1 ); }
	KeyValues3* ArrayAddElementToTail();
	KeyValues3 *ArrayAddToTail() { return ArrayAddElementToTail(); }
	void ArrayInsertMultipleBefore( int elem, int num );

	void ArraySwapItems( int idx1, int idx2 );

	void ArrayRemoveElements( int elem, int num );
	void ArrayRemoveElement( int elem ) { ArrayRemoveElements( elem, 1 ); }
	void ArrayRemoveMultiple( int elem, int num ) { ArrayRemoveElements( elem, num ); }

	CKeyValues3Table *GetTable() { return IsTable() ? m_Data.m_pTable : nullptr; }
	const CKeyValues3Table *GetTable() const { return const_cast<KeyValues3 *>(this)->GetTable(); }

	void SetToEmptyTable();
	int GetMemberCount() const;

	KeyValues3* GetMember( KV3MemberId_t id );
	const KeyValues3* GetMember( KV3MemberId_t id ) const { return const_cast<KeyValues3*>(this)->GetMember( id ); }
	CKV3MemberHash GetMemberHash( KV3MemberId_t id ) const;
	const char* GetMemberName( KV3MemberId_t id ) const;
	CKV3MemberName GetKV3MemberName( KV3MemberId_t id ) const;

protected:
	KeyValues3* Internal_FindMember( const CKV3MemberName &name, KV3MemberId_t &next, KeyValues3* defaultValue = nullptr );

public:
	KeyValues3* FindMember( const CKV3MemberName &name, KeyValues3* defaultValue = nullptr ) { KV3MemberId_t next = KV3_INVALID_MEMBER; return Internal_FindMember( name, next, defaultValue ); }
	const KeyValues3 *FindMember( const CKV3MemberName &name, KeyValues3 *defaultValue = nullptr ) const { return const_cast<KeyValues3 *>(this)->FindMember( name, defaultValue ); };
	KeyValues3* FindOrCreateMember( const CKV3MemberName &name, bool *pCreated = nullptr );

	// Non-const access creates the member, or grows the array up to the index.
	// Const access never modifies and returns a shared null value when the member or element is missing.
	KeyValues3 &operator[]( const CKV3MemberName &name ) { return *FindOrCreateMember( name ); }
	const KeyValues3 &operator[]( const CKV3MemberName &name ) const { const KeyValues3 *kv = FindMember( name ); return kv ? *kv : GetNullValue(); }
	KeyValues3 &operator[]( int elem );
	const KeyValues3 &operator[]( int elem ) const { const KeyValues3 *kv = GetArrayElement( elem ); return kv ? *kv : GetNullValue(); }

	static const KeyValues3 &GetNullValue();
	CKV3MemberHash RenameMember( const CKV3MemberName &name, const CKV3MemberName &newName );
	bool RemoveMember( KV3MemberId_t id );
	bool RemoveMember( const KeyValues3* kv );
	bool RemoveMember( const CKV3MemberName &name );

	bool GetMemberBool( const CKV3MemberName &name, bool defaultValue = false ) const { auto kv = FindMember( name ); return kv ? kv->GetBool( defaultValue ) : defaultValue; }
	char8 GetMemberChar( const CKV3MemberName &name, char8 defaultValue = 0 ) const { auto kv = FindMember( name ); return kv ? kv->GetChar( defaultValue ) : defaultValue; }
	uchar32 GetMemberUChar32( const CKV3MemberName &name, uchar32 defaultValue = 0 ) const { auto kv = FindMember( name ); return kv ? kv->GetUChar32( defaultValue ) : defaultValue; }
	int8 GetMemberInt8( const CKV3MemberName &name, int8 defaultValue = 0 ) const { auto kv = FindMember( name ); return kv ? kv->GetInt8( defaultValue ) : defaultValue; }
	uint8 GetMemberUInt8( const CKV3MemberName &name, uint8 defaultValue = 0 ) const { auto kv = FindMember( name ); return kv ? kv->GetUInt8( defaultValue ) : defaultValue; }
	int16 GetMemberShort( const CKV3MemberName &name, int16 defaultValue = 0 ) const { auto kv = FindMember( name ); return kv ? kv->GetShort( defaultValue ) : defaultValue; }
	uint16 GetMemberUShort( const CKV3MemberName &name, uint16 defaultValue = 0 ) const { auto kv = FindMember( name ); return kv ? kv->GetUShort( defaultValue ) : defaultValue; }
	int32 GetMemberInt( const CKV3MemberName &name, int32 defaultValue = 0 ) const { auto kv = FindMember( name ); return kv ? kv->GetInt( defaultValue ) : defaultValue; }
	uint32 GetMemberUInt( const CKV3MemberName &name, uint32 defaultValue = 0 ) const { auto kv = FindMember( name ); return kv ? kv->GetUInt( defaultValue ) : defaultValue; }
	int64 GetMemberInt64( const CKV3MemberName &name, int64 defaultValue = 0 ) const { auto kv = FindMember( name ); return kv ? kv->GetInt64( defaultValue ) : defaultValue; }
	uint64 GetMemberUInt64( const CKV3MemberName &name, uint64 defaultValue = 0 ) const { auto kv = FindMember( name ); return kv ? kv->GetUInt64( defaultValue ) : defaultValue; }
	float32 GetMemberFloat( const CKV3MemberName &name, float32 defaultValue = 0.0f ) const { auto kv = FindMember( name ); return kv ? kv->GetFloat( defaultValue ) : defaultValue; }
	float64 GetMemberDouble( const CKV3MemberName &name, float64 defaultValue = 0.0 ) const { auto kv = FindMember( name ); return kv ? kv->GetDouble( defaultValue ) : defaultValue; }
	void *GetMemberPointer( const CKV3MemberName &name, void *defaultValue = (void *)0 ) const { auto kv = FindMember( name ); return kv ? kv->GetPointer( defaultValue ) : defaultValue; }
	CUtlStringToken GetMemberStringToken( const CKV3MemberName &name, CUtlStringToken defaultValue = CUtlStringToken() ) const { auto kv = FindMember( name ); return kv ? kv->GetStringToken( defaultValue ) : defaultValue; }
	CEntityHandle GetMemberEHandle( const CKV3MemberName &name, CEntityHandle defaultValue = CEntityHandle() ) const { auto kv = FindMember( name ); return kv ? kv->GetEHandle( defaultValue ) : defaultValue; }
	const char *GetMemberString( const CKV3MemberName &name, const char *defaultValue = "" ) const { auto kv = FindMember( name ); return kv ? kv->GetString( defaultValue ) : defaultValue; }
	Color GetMemberColor( const CKV3MemberName &name, const Color &defaultValue = Color( 0, 0, 0, 255 ) ) const { auto kv = FindMember( name ); return kv ? kv->GetColor( defaultValue ) : defaultValue; }
	Vector GetMemberVector( const CKV3MemberName &name, const Vector &defaultValue = Vector( 0.0f, 0.0f, 0.0f ) ) const { auto kv = FindMember( name ); return kv ? kv->GetVector( defaultValue ) : defaultValue; }
	Vector2D GetMemberVector2D( const CKV3MemberName &name, const Vector2D &defaultValue = Vector2D( 0.0f, 0.0f ) ) const { auto kv = FindMember( name ); return kv ? kv->GetVector2D( defaultValue ) : defaultValue; }
	Vector4D GetMemberVector4D( const CKV3MemberName &name, const Vector4D &defaultValue = Vector4D( 0.0f, 0.0f, 0.0f, 0.0f ) ) const { auto kv = FindMember( name ); return kv ? kv->GetVector4D( defaultValue ) : defaultValue; }
	Quaternion GetMemberQuaternion( const CKV3MemberName &name, const Quaternion &defaultValue = Quaternion( 0.0f, 0.0f, 0.0f, 0.0f ) ) const { auto kv = FindMember( name ); return kv ? kv->GetQuaternion( defaultValue ) : defaultValue; }
	QAngle GetMemberQAngle( const CKV3MemberName &name, const QAngle &defaultValue = QAngle( 0.0f, 0.0f, 0.0f ) ) const { auto kv = FindMember( name ); return kv ? kv->GetQAngle( defaultValue ) : defaultValue; }
	matrix3x4_t GetMemberMatrix3x4( const CKV3MemberName &name, const matrix3x4_t &defaultValue = matrix3x4_t( Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ) ) ) const { auto kv = FindMember( name ); return kv ? kv->GetMatrix3x4( defaultValue ) : defaultValue; }

	uint64 GetMemberUint64( const CKV3MemberName &name, uint64 defaultValue = 0 ) const { return GetMemberUInt64( name, defaultValue ); }
	void GetMemberAsString( const CKV3MemberName &name, char *pOutData, int nBufSize, const char *defaultValue = "" ) const { auto kv = FindMember( name ); if ( kv ) kv->GetValueAsString( pOutData, nBufSize ); else V_strncpy( pOutData, defaultValue, nBufSize ); }
	void GetMemberAsString( const CKV3MemberName &name, CUtlString *pOutString, const char *defaultValue = "" ) const { auto kv = FindMember( name ); if ( kv ) kv->GetValueAsString( pOutString ); else pOutString->Set( defaultValue ); }

	// Out-parameter overloads return false when the member is missing or has a different element count
	bool GetMemberFloatArray( const CKV3MemberName &name, int count, float32 *pOutValues ) const { auto kv = FindMember( name ); return kv ? kv->GetValueFloatArray( count, pOutValues ) : false; }
	bool GetMemberVector( const CKV3MemberName &name, Vector *pOutValue ) const { return GetMemberFloatArray( name, 3, pOutValue->Base() ); }
	bool GetMemberVector2D( const CKV3MemberName &name, Vector2D *pOutValue ) const { return GetMemberFloatArray( name, 2, pOutValue->Base() ); }
	bool GetMemberVector4D( const CKV3MemberName &name, Vector4D *pOutValue ) const { return GetMemberFloatArray( name, 4, pOutValue->Base() ); }
	bool GetMemberQAngle( const CKV3MemberName &name, QAngle *pOutValue ) const { return GetMemberFloatArray( name, 3, pOutValue->Base() ); }
	bool GetMemberQuaternion( const CKV3MemberName &name, Quaternion *pOutValue ) const { return GetMemberFloatArray( name, 4, pOutValue->Base() ); }
	bool GetMemberMatrix( const CKV3MemberName &name, matrix3x4_t *pOutValue ) const { return GetMemberFloatArray( name, 3*4, pOutValue->Base() ); }

	void SetMemberToNull( const CKV3MemberName &name ) { FindOrCreateMember( name )->SetToNull(); }
	void SetMemberToEmptyArray( const CKV3MemberName &name ) { FindOrCreateMember( name )->SetToEmptyKV3Array(); }
	void SetMemberToEmptyTable( const CKV3MemberName &name ) { FindOrCreateMember( name )->SetToEmptyTable(); }
	void SetMemberToBinaryBlob( const CKV3MemberName &name, const byte *blob, int size ) { FindOrCreateMember( name )->SetToBinaryBlob( blob, size ); }
	void SetMemberToBinaryBlobExternal( const CKV3MemberName &name, const byte *blob, int size, bool free_mem ) { FindOrCreateMember( name )->SetToBinaryBlobExternal( blob, size, free_mem ); }
	void SetMemberToCopyOfValue( const CKV3MemberName &name, KeyValues3 *other ) { FindOrCreateMember( name )->CopyFrom( *other ); }

	void SetMemberBool( const CKV3MemberName &name, bool value ) { FindOrCreateMember( name )->SetBool( value ); }
	void SetMemberChar( const CKV3MemberName &name, char8 value ) { FindOrCreateMember( name )->SetChar( value ); }
	void SetMemberUChar32( const CKV3MemberName &name, uchar32 value ) { FindOrCreateMember( name )->SetUChar32( value ); }
	void SetMemberInt8( const CKV3MemberName &name, int8 value ) { FindOrCreateMember( name )->SetInt8( value ); }
	void SetMemberUInt8( const CKV3MemberName &name, uint8 value ) { FindOrCreateMember( name )->SetUInt8( value ); }
	void SetMemberShort( const CKV3MemberName &name, int16 value ) { FindOrCreateMember( name )->SetShort( value ); }
	void SetMemberUShort( const CKV3MemberName &name, uint16 value ) { FindOrCreateMember( name )->SetUShort( value ); }
	void SetMemberInt( const CKV3MemberName &name, int32 value ) { FindOrCreateMember( name )->SetInt( value ); }
	void SetMemberUInt( const CKV3MemberName &name, uint32 value ) { FindOrCreateMember( name )->SetUInt( value ); }
	void SetMemberInt64( const CKV3MemberName &name, int64 value ) { FindOrCreateMember( name )->SetInt64( value ); }
	void SetMemberUInt64( const CKV3MemberName &name, uint64 value ) { FindOrCreateMember( name )->SetUInt64( value ); }
	void SetMemberFloat( const CKV3MemberName &name, float32 value ) { FindOrCreateMember( name )->SetFloat( value ); }
	void SetMemberDouble( const CKV3MemberName &name, float64 value ) { FindOrCreateMember( name )->SetDouble( value ); }
	void SetMemberPointer( const CKV3MemberName &name, void *ptr ) { FindOrCreateMember( name )->SetPointer( ptr ); }
	void SetMemberStringToken( const CKV3MemberName &name, CUtlStringToken token ) { FindOrCreateMember( name )->SetStringToken( token ); }
	void SetMemberEHandle( const CKV3MemberName &name, CEntityHandle ehandle ) { FindOrCreateMember( name )->SetEHandle( ehandle ); }
	void SetMemberString( const CKV3MemberName &name, const char *pString, KV3SubType_t subtype = KV3_SUBTYPE_STRING ) { FindOrCreateMember( name )->SetString( pString, subtype ); }
	void SetMemberStringExternal( const CKV3MemberName &name, const char *pString, KV3SubType_t subtype = KV3_SUBTYPE_STRING ) { FindOrCreateMember( name )->SetStringExternal( pString, subtype ); }
	void SetMemberColor( const CKV3MemberName &name, const Color &color ) { FindOrCreateMember( name )->SetColor( color ); }
	void SetMemberVector( const CKV3MemberName &name, const Vector &vec ) { FindOrCreateMember( name )->SetVector( vec ); }
	void SetMemberVector2D( const CKV3MemberName &name, const Vector2D &vec2d ) { FindOrCreateMember( name )->SetVector2D( vec2d ); }
	void SetMemberVector4D( const CKV3MemberName &name, const  Vector4D &vec4d ) { FindOrCreateMember( name )->SetVector4D( vec4d ); }
	void SetMemberQuaternion( const CKV3MemberName &name, const Quaternion &quat ) { FindOrCreateMember( name )->SetQuaternion( quat ); }
	void SetMemberQAngle( const CKV3MemberName &name, const QAngle &ang ) { FindOrCreateMember( name )->SetQAngle( ang ); }
	void SetMemberMatrix3x4( const CKV3MemberName &name, const matrix3x4_t &matrix ) { FindOrCreateMember( name )->SetMatrix3x4( matrix ); }

	void SetMemberUint64( const CKV3MemberName &name, uint64 value ) { SetMemberUInt64( name, value ); }
	void SetMemberResourceString( const CKV3MemberName &name, const char *pString ) { SetMemberString( name, pString, KV3_SUBTYPE_RESOURCE ); }
	void SetMemberFloatArray( const CKV3MemberName &name, int count, const float32 *pValues ) { FindOrCreateMember( name )->SetValueFloatArray( count, pValues ); }
	void SetMemberMatrix( const CKV3MemberName &name, const matrix3x4_t &matrix ) { SetMemberMatrix3x4( name, matrix ); }

	union Data_t
	{
		Data_t() : m_nMemory(0)
		{
		}

		bool	m_Bool;
		int64	m_Int;
		uint64	m_UInt;
		float64	m_Double;

		const char* m_pString;
		char m_szStringShort[8];

		KV3BinaryBlob_t* m_pBinaryBlob;

		KeyValues3Array_t m_Array;
		CKeyValues3Table* m_pTable;

		uint64 m_nMemory;
		void* m_pMemory;
		char m_Memory[1];
	};

private:
	void Alloc( int initial_size = 0, Data_t data = {}, int bytes_available = 0, bool should_free = false );

	CKeyValues3Array *AllocArray( int initial_size = 0 );
	CKeyValues3Table *AllocTable( int initial_size = 0 );

	void AllocArrayInPlace( int initial_size, Data_t data, int preallocated_size, bool should_free );
	void AllocTableInPlace( int initial_size, Data_t data, int preallocated_size, bool should_free );

	template <typename T>
	T *AllocateOnHeap( int initial_size = 0 );

	template <typename T>
	void FreeOnHeap( T *element );

	void FreeArray( CKeyValues3Array *element, bool clearing_context = false );
	void FreeTable( CKeyValues3Table *element, bool clearing_context = false );

	KeyValues3 *AllocMember( KV3TypeEx_t type = KV3_TYPEEX_NULL, KV3SubType_t subtype = KV3_SUBTYPE_UNSPECIFIED );
	void FreeMember( KeyValues3 *member );

	void Free( bool bClearingContext = false );
	void ResolveUnspecified();
	void PrepareForType( KV3TypeEx_t type, KV3SubType_t subtype, int initial_size = 0, Data_t data = {}, int bytes_available = 0, bool should_free = false );

	bool HasCluster() const { return m_nClusterElement != KV3_INVALID_CLUSTER_ELEMENT; }
	int GetClusterElement() const { return m_nClusterElement; }
	void SetClusterElement( int element ) { m_bContextIndependent = ( element == KV3_INVALID_CLUSTER_ELEMENT ); m_nClusterElement = element; }
	CKeyValues3Cluster* GetCluster() const;

	template < typename T > T FromString( T defaultValue ) const;
	template < typename T > void SetDirect( T value );

	template < typename T > T GetValue( T defaultValue ) const;
	template < typename T > void SetValue( T value, KV3TypeEx_t type, KV3SubType_t subtype );

	template < typename T > T GetVecBasedObj( int size, const T &defaultValue ) const;
	template < typename T > void SetVecBasedObj( const T &obj, int size, KV3SubType_t subtype );

	template < typename T >
	void NormalizeArray( KV3TypeEx_t type, KV3SubType_t subtype, int size, const T* data, bool bFree );
	void NormalizeArray();

	template < typename T >
	void AllocArray( int size, const T* data, KV3ArrayAllocType_t alloc_type, KV3TypeEx_t type_short, KV3TypeEx_t type_ptr, KV3SubType_t subtype, KV3TypeEx_t type_elem, KV3SubType_t subtype_elem );

	bool ReadArrayInt32( int size, int32* data ) const;
	bool ReadArrayFloat32( int size, float32* data ) const;

	static constexpr size_t TotalSizeOf( int initial_size ) { return sizeof(KeyValues3); }
	static constexpr size_t TotalSizeOfData( int size ) { return sizeof(Data_t); }
	static constexpr size_t TotalSizeWithoutStaticData() { return sizeof(KeyValues3) - TotalSizeOfData( 0 ); }

private:
	uint64 m_bContextIndependent : 1;
	uint64 m_bFreeArrayMemory : 1;
	uint64 m_TypeEx : 8;
	uint64 m_SubType : 8;
	uint64 m_nClusterElement : 16;
	uint64 m_nNumArrayElements : 5;
	uint64 m_nFlags : 8;
	uint64 m_nReserved : 17;
	Data_t m_Data;

	friend CKeyValues3Cluster;
	friend CKeyValues3ArrayCluster;
	friend CKeyValues3TableCluster;
	friend class CKV3Arena;
	friend class CKV3ArenaImpl;
	friend class CKeyValues3Table;
	friend class CKeyValues3Array;
};
COMPILE_TIME_ASSERT(sizeof(KeyValues3) == 16);

class CKeyValues3Iterator
{
public:
	CKeyValues3Iterator() : m_Stack() {}
	CKeyValues3Iterator( KeyValues3 *kv ) : CKeyValues3Iterator() { Init( kv ); }

	void Init( KeyValues3 *kv );

	void Advance();

	KeyValues3 *Get() const { return IsValid() ? m_Stack[m_Stack.Count() - 1].m_pKV : nullptr; }
	bool IsValid() const { return m_Stack.Count() > 0; }

private:
	struct StackEntry_t
	{
		KeyValues3 *m_pKV;
		int m_nIndex;
	};

	CUtlVectorFixedGrowable<StackEntry_t, 4> m_Stack;
};

class CKV3Arena
{
public:
	CKV3Arena( bool bNoRoot = false );
	~CKV3Arena();

	KeyValues3* AllocKV( KV3TypeEx_t type = KV3_TYPEEX_NULL, KV3SubType_t subtype = KV3_SUBTYPE_UNSPECIFIED );
	// WARNING: kv must belong to this context!!!
	void FreeKV( KeyValues3* kv );

	// gets the pre-allocated kv if we indicated its existence when creating the context
	KeyValues3* Root();
	const KeyValues3* Root() const { return const_cast<CKV3Arena*>(this)->Root(); }

	KeyValues3 *operator->() { return Root(); }
	const KeyValues3 *operator->() const { return Root(); }

	bool IsMetaDataEnabled() const;
	// returns true if the desired format was converted to another after loading via LoadKV3*
	bool IsFormatConverted() const;
	bool IsRootAvailabe() const;

	// filled in after loading via LoadKV3* in binary encoding
	CUtlBuffer& GetBinaryData();

	IErrorListener* GetParsingErrorListener() const;
	void SetParsingErrorListener( IErrorListener* listener );

	void EnableMetaData( bool bEnable );
	void CopyMetaData( KV3MetaData_t* pDest, const KV3MetaData_t* pSrc );

	const char* LookupString( UtlSymLargeId_t symid );
	const char *AllocString( const char *pString, UtlSymLargeId_t *pSymLargeId = nullptr );

	void Clear();
	void Purge();

private:
	CKV3ArenaImpl &Impl();
	const CKV3ArenaImpl &Impl() const;

private:
	// The arena state is private to kv3lib, only its size is part of the public layout.
	uint64 m_Storage[ KV3_CONTEXT_SIZE / sizeof( uint64 ) ];

	friend class KeyValues3;
};
COMPILE_TIME_ASSERT(sizeof(CKV3Arena) == KV3_CONTEXT_SIZE);

#define KV3_FLAG_OBJECT_REFERANCES (1 << 0)

template < typename T > inline T KeyValues3::FromString( T defaultValue ) const { Assert( 0 ); return defaultValue; }
template <> inline bool KeyValues3::FromString( bool defaultValue ) const		{ return V_StringToBool( GetString(), defaultValue ); }
template <> inline char8 KeyValues3::FromString( char8 defaultValue ) const		{ return V_StringToInt8( GetString(), defaultValue ); }
template <> inline int8 KeyValues3::FromString( int8 defaultValue ) const		{ return V_StringToInt8( GetString(), defaultValue ); }
template <> inline uint8 KeyValues3::FromString( uint8 defaultValue ) const		{ return V_StringToUint8( GetString(), defaultValue ); }
template <> inline int16 KeyValues3::FromString( int16 defaultValue ) const		{ return V_StringToInt16( GetString(), defaultValue ); }
template <> inline uint16 KeyValues3::FromString( uint16 defaultValue ) const	{ return V_StringToUint16( GetString(), defaultValue ); }
template <> inline int32 KeyValues3::FromString( int32 defaultValue ) const		{ return V_StringToInt32( GetString(), defaultValue ); }
template <> inline uint32 KeyValues3::FromString( uint32 defaultValue ) const	{ return V_StringToUint32( GetString(), defaultValue ); }
template <> inline int64 KeyValues3::FromString( int64 defaultValue ) const		{ return V_StringToInt64( GetString(), defaultValue ); }
template <> inline uint64 KeyValues3::FromString( uint64 defaultValue ) const	{ return V_StringToUint64( GetString(), defaultValue ); }
template <> inline float32 KeyValues3::FromString( float32 defaultValue ) const	{ return V_StringToFloat32( GetString(), defaultValue ); }
template <> inline float64 KeyValues3::FromString( float64 defaultValue ) const	{ return V_StringToFloat64( GetString(), defaultValue ); }

template < typename T > inline void KeyValues3::SetDirect( T value ) { Assert( 0 ); }
template <> inline void KeyValues3::SetDirect( bool value )		{ m_Data.m_Bool = value; }
template <> inline void KeyValues3::SetDirect( char8 value )	{ m_Data.m_Int = ( int64 )value; }
template <> inline void KeyValues3::SetDirect( int8 value )		{ m_Data.m_Int = ( int64 )value; }
template <> inline void KeyValues3::SetDirect( uint8 value )	{ m_Data.m_UInt = ( uint64 )value; }
template <> inline void KeyValues3::SetDirect( int16 value )	{ m_Data.m_Int = ( int64 )value; }
template <> inline void KeyValues3::SetDirect( uint16 value )	{ m_Data.m_UInt = ( uint64 )value; }
template <> inline void KeyValues3::SetDirect( int32 value )	{ m_Data.m_Int = ( int64 )value; }
template <> inline void KeyValues3::SetDirect( uint32 value )	{ m_Data.m_UInt = ( uint64 )value; }
template <> inline void KeyValues3::SetDirect( int64 value )	{ m_Data.m_Int = value; }
template <> inline void KeyValues3::SetDirect( uint64 value )	{ m_Data.m_UInt = value; }
template <> inline void KeyValues3::SetDirect( float32 value )	{ m_Data.m_Double = ( float64 )value; }
template <> inline void KeyValues3::SetDirect( float64 value )	{ m_Data.m_Double = value; }

template < typename T >
T KeyValues3::GetVecBasedObj( int size, const T &defaultValue ) const
{
	T obj;
	if ( !ReadArrayFloat32( size, obj.Base() ) )
		obj = defaultValue;
	return obj;
}

template < typename T >
void KeyValues3::SetVecBasedObj( const T &obj, int size, KV3SubType_t subtype )
{
	NormalizeArray< float32 >( KV3_TYPEEX_DOUBLE, KV3_SUBTYPE_FLOAT32, size, obj.Base(), false );
}

template < typename T >
T KeyValues3::GetValue( T defaultValue ) const
{
	switch ( GetType() )
	{
		case KV3_TYPE_BOOL:
			return ( T )m_Data.m_Bool;
		case KV3_TYPE_INT:
			return ( T )m_Data.m_Int;
		case KV3_TYPE_UINT:
			return ( GetSubType() != KV3_SUBTYPE_POINTER ) ? ( T )m_Data.m_UInt : defaultValue;
		case KV3_TYPE_DOUBLE:
			return ( T )m_Data.m_Double;
		case KV3_TYPE_STRING:
			return FromString<T>( defaultValue );
		default:
			return defaultValue;
	}
}

template < typename T >
void KeyValues3::SetValue( T value, KV3TypeEx_t type, KV3SubType_t subtype )
{
	PrepareForType( type, subtype );
	SetDirect<T>( value );
}

#include "tier0/memdbgoff.h"

#endif // KEYVALUES3_H
