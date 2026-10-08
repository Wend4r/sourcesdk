#ifndef KV3TRANSFER_CONSTANTS_H
#define KV3TRANSFER_CONSTANTS_H

#ifdef _WIN32
#pragma once
#endif

// Constants, enums and macros only, so that a class declaration can use CLASS_USES_KV3TRANSFER_*
// without pulling in the transfer contexts. Do not add includes.

class CBufferString;
class CKV3TransferSaveContext;
class CKV3TransferLoadContext;

#define KV3TRANSFER_CLASSNAME_MEMBER "_class"
#define KV3TRANSFER_CLASSNAME_MAX_LENGTH 256

// A transfer nested deeper than this fails with "Stack depth limit hit (%d)" instead of recursing.
#define KV3TRANSFER_MAX_STACK_DEPTH 63

enum KV3TransferResult_t
{
	KV3TRANSFER_FAIL,
	KV3TRANSFER_SUCCESS,
};

// How a class transfers to and from a KeyValues3 value.
enum KV3TransferClassBehavior_t
{
	// Transferring the class fails with "Failed to save unsupported class".
	KV3TRANSFER_CLASS_UNIMPLEMENTED,

	// A table of the members. A pointer to the class never refers to a derived class.
	KV3TRANSFER_CLASS_AS_SIMPLE_TABLE,

	// A table of the members with a KV3TRANSFER_CLASSNAME_MEMBER string naming the exact class.
	// The class provides KV3TransferPolymorphicClassname and KV3TransferAllocateClassInstance.
	KV3TRANSFER_CLASS_AS_POLYMORPHIC_TABLE,

	// The class handles the value itself; KV3TransferSave receives a null value.
	KV3TRANSFER_CLASS_AS_DATA,
};

//-----------------------------------------------------------------------------
// Maps the enumerators of a non-schematized enum to strings. Without it an enum transfers as an integer.
//
//	BEGIN_KV3TRANSFER_ENUM_HELPERS( GestureType_t, GESTURE_TYPE_COUNT, GESTURE_TYPE_BOTH )
//		KV3TRANSFER_ENUM_HELPER( GESTURE_TYPE_BOTH, "Both" )
//		KV3TRANSFER_ENUM_HELPER( GESTURE_TYPE_LEFT, "Left" )
//	END_KV3TRANSFER_ENUM_HELPERS()
//-----------------------------------------------------------------------------
template < class T >
struct KV3Transfer_EnumHelpers_t
{
	static constexpr bool is_present = false;
};

template < class TEnum >
struct KV3Transfer_EnumHelpers_StringPair_t
{
	TEnum m_nEnum;
	const char *m_pString;
};

template < class TEnum >
struct KV3Transfer_EnumHelpers_StringPairList_t
{
	const KV3Transfer_EnumHelpers_StringPair_t< TEnum > *m_pList;
	size_t m_nCount;
};

#define BEGIN_KV3TRANSFER_ENUM_HELPERS( _ENUM_NAME, _ENUM_COUNT, _DEFAULT_VALUE ) \
	template <> struct KV3Transfer_EnumHelpers_t< _ENUM_NAME > \
	{ \
		static constexpr bool is_present = true; \
		static constexpr _ENUM_NAME ENUM_COUNT = _ENUM_COUNT; \
		static constexpr _ENUM_NAME ENUM_DEFAULT = _DEFAULT_VALUE; \
		\
		static KV3Transfer_EnumHelpers_StringPairList_t< _ENUM_NAME > Pairs() \
		{ \
			static const KV3Transfer_EnumHelpers_StringPair_t< _ENUM_NAME > s_Elements[] = \
			{

#define KV3TRANSFER_ENUM_HELPER( _VALUE, _STRING ) \
				{ _VALUE, _STRING },

#define END_KV3TRANSFER_ENUM_HELPERS() \
			}; \
			COMPILE_TIME_ASSERT( ARRAYSIZE( s_Elements ) == ENUM_COUNT ); \
			return { s_Elements, ARRAYSIZE( s_Elements ) }; \
		} \
	};

//-----------------------------------------------------------------------------
// Declares the KV3 transfer of a class. The game generates the bodies from the schema fields: each
// KV3Transfer[Save|Load]_<classname> transfers the fields of its own class after chaining to the base
// class, and KV3Transfer[Save|Load] calls it. A class that is transferred through the SDK defines them.
//
// The second argument of KV3TransferAllocateClassInstance is unidentified; the game passes null for it.
//-----------------------------------------------------------------------------
#define CLASS_USES_KV3TRANSFER_DATA( classname ) \
	META_USE_CODEGEN_TAG( MEmitKV3Transfer ); \
	enum { KV3TRANSFER_BEHAVIOR = KV3TRANSFER_CLASS_AS_SIMPLE_TABLE }; \
	enum { KV3TRANSFER_IS_VIRTUAL = 0 }; \
	static classname *KV3TransferAllocateClassInstance( const char *pDerivedClassName, void *pUnk ); \
	void KV3TransferSave( CKV3TransferSaveContext *pContext ) const; \
	void KV3TransferLoad( CKV3TransferLoadContext *pContext ); \
	void KV3TransferSave_##classname( CKV3TransferSaveContext *pContext ) const; \
	void KV3TransferLoad_##classname( CKV3TransferLoadContext *pContext )

// The polymorphic variant: KV3TransferSave and KV3TransferLoad take their vtable slots where the macro is used.
#define CLASS_USES_KV3TRANSFER_VIRTUAL( classname ) \
	META_USE_CODEGEN_TAG( MEmitKV3Transfer ); \
	enum { KV3TRANSFER_BEHAVIOR = KV3TRANSFER_CLASS_AS_POLYMORPHIC_TABLE }; \
	enum { KV3TRANSFER_IS_VIRTUAL = 1 }; \
	static void KV3TransferPolymorphicClassname( const classname *pObject, CBufferString &sOutClassName ); \
	static classname *KV3TransferAllocateClassInstance( const char *pDerivedClassName, void *pUnk ); \
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const; \
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ); \
	void KV3TransferSave_##classname( CKV3TransferSaveContext *pContext ) const; \
	void KV3TransferLoad_##classname( CKV3TransferLoadContext *pContext )

#endif // KV3TRANSFER_CONSTANTS_H
