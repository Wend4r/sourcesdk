#ifndef ENTITYPULSE_H
#define ENTITYPULSE_H

#if _WIN32
#pragma once
#endif

#include "variant.h"
#include "tier0/utlstring.h"
#include "tier1/utlvector.h"
#include "tier1/utlsymbollarge.h"
#include "tier1/utlhashtable.h"
#include "tier0/murmurhash3.h"

class CBasePulseGraphInstance;
class CEntityClass;
class CEntityInstance;
class CPulseAPIExtensionRegistrationContext;
class CPulseExecCursor;
class CPulseRuntimeMethodArg;
class KeyValues3;
class CPulseArgumentPack;
class CPulseInputParamMap;

schema enum PulseApiFeature_t : uint32
{
	AF_NONE								= 0,
	AF_ENTITIES							= 1 << 0,
	AF_PANORAMA							= 1 << 1,
	AF_PARTICLES						= 1 << 3,
	AF_FAKE_ENTITIES					= 1 << 4,
	AF_SELECTORS_WITHOUT_REQUIREMENTS	= 1 << 5,
};

struct PulseBindingMetadata_t
{
	const char *m_pName;
	const char *m_pValue;
};

schema enum PulseValueType_t : int32
{
	PVAL_VOID						= -1, META( MPropertyFriendlyName = "Void" )
	PVAL_BOOL						= 0, META( MPropertyFriendlyName = "Boolean" )
	PVAL_INT						= 1, META( MPropertyFriendlyName = "Integer" )
	PVAL_FLOAT						= 2, META( MPropertyFriendlyName = "Float" )
	PVAL_STRING						= 3, META( MPropertyFriendlyName = "String" )
	PVAL_VEC2						= 4, META( MPropertyFriendlyName = "Vector2D" )
	PVAL_VEC3						= 5, META( MPropertyFriendlyName = "Vector" )
	PVAL_QANGLE						= 6, META( MPropertyFriendlyName = "Angle" )
	PVAL_VEC3_WORLDSPACE			= 7, META( MPropertyFriendlyName = "World Vector" )
	PVAL_VEC4						= 8, META( MPropertyFriendlyName = "Vector4D" )
	PVAL_TRANSFORM					= 9, META( MPropertyFriendlyName = "Transform" )
	PVAL_TRANSFORM_WORLDSPACE		= 10, META( MPropertyFriendlyName = "World Transform" )
	PVAL_COLOR_RGB					= 11, META( MPropertyFriendlyName = "Color" )
	PVAL_GAMETIME					= 12, META( MPropertyFriendlyName = "Game Time" )
	PVAL_EHANDLE					= 13, META( MPropertyFriendlyName = "Entity Handle" )
	PVAL_RESOURCE					= 14, META( MPropertyFriendlyName = "Resource" )
	PVAL_RESOURCE_NAME				= 15, META( MPropertyFriendlyName = "Resource Name" )
	PVAL_SNDEVT_GUID				= 16, META( MPropertyFriendlyName = "SoundEvent Instance Handle" )
	PVAL_SNDEVT_NAME				= 17, META( MPropertyFriendlyName = "SoundEvent" )
	PVAL_ENTITY_NAME				= 18, META( MPropertyFriendlyName = "Entity Name" )
	PVAL_OPAQUE_HANDLE				= 19, META( MPropertyFriendlyName = "Opaque Handle" )
	PVAL_TYPESAFE_INT				= 20, META( MPropertyFriendlyName = "Typesafe Int" )
	PVAL_MODEL_MATERIAL_GROUP		= 21, META( MPropertyFriendlyName = "Material Group" )
	PVAL_CURSOR_FLOW				= 22, META( MPropertySuppressEnumerator )
	PVAL_VARIANT					= 23, META( MPropertyFriendlyName = "Variant"; MPropertySuppressEnumerator )
	PVAL_UNKNOWN					= 24, META( MPropertyFriendlyName = "Unknown"; MPropertySuppressEnumerator )
	PVAL_SCHEMA_ENUM				= 25, META( MPropertyFriendlyName = "Schema Enum" )
	PVAL_PANORAMA_PANEL_HANDLE		= 26, META( MPropertyFriendlyName = "Panorama Panel Handle" )
	PVAL_TEST_HANDLE				= 27, META( MPropertyFriendlyName = "Test Handle" )
	PVAL_ARRAY						= 28, META( MPropertyFriendlyName = "Array" )
	PVAL_TYPESAFE_INT64				= 29, META( MPropertyFriendlyName = "Typesafe Int64" )
	PVAL_PARTICLE_EHANDLE			= 30, META( MPropertySuppressEnumerator; MPropertyFriendlyName = "Particle Object" )
	PVAL_ANIM_SEQUENCE				= 31, META( MPropertyFriendlyName = "Anim Sequence" )
	PVAL_VDATA_CHOICE				= 32, META( MPropertyFriendlyName = "VData Choice" )
	PVAL_COUNT						= 33, META( MPropertySuppressEnumerator )
};

schema class CPulseValueFullType
{
public:
	CPulseValueFullType() : m_nType( PVAL_VOID ), m_pElementType( nullptr ) {}
	CPulseValueFullType( PulseValueType_t nType, const char *pszSubType = nullptr );
	CPulseValueFullType( const CPulseValueFullType &other ) : CPulseValueFullType() { Assign( other ); }
	~CPulseValueFullType() { ReleaseElementType(); }

	CPulseValueFullType &operator=( const CPulseValueFullType &other ) { Assign( other ); return *this; }

	// Compares the types along the element type chain
	bool operator==( const CPulseValueFullType &other ) const;
	bool operator!=( const CPulseValueFullType &other ) const { return !( *this == other ); }

	bool IsValid() const { return m_nType != PVAL_VOID; }
	bool IsType( PulseValueType_t nType ) const { return m_nType == nType; }

	size_t GetSize( size_t *pAlignment = nullptr ) const;

	void ConstructValue( void *pValue ) const;
	void DestructValue( void *pValue ) const;
	void CopyValue( const void *pSrc, void *pDest ) const;

private:
	void Assign( const CPulseValueFullType &other );
	void ReleaseElementType();

public:
	PulseValueType_t m_nType;

	// Element type of an array type, otherwise null
	CPulseValueFullType *m_pElementType;

	CUtlSymbolLarge m_subType;
};

class CPulseRuntimeMethodArg
{
public:
	uint32 m_nNameHash;
	int32 m_nUnk0004;
	const char* m_pName;
	CPulseValueFullType m_Type;
	uint64 m_nUnk0028;
	uint8 m_defaultValue[32];
	uint16 m_nFlags;
	uint8 m_unk0052[6];
	void* m_pUnk0058;
	void* m_pfnUnk0060;
	void* m_pUnk0068;
};

// A view of a static argument array, returned by value
struct PulseMethodArgList_t
{
	int32 m_nCount;
	int32 m_nAllocated;
	const CPulseRuntimeMethodArg* m_pElements;
};

struct PulseHostTable_t
{
	uint32 m_nMask;
	void* m_pHosts[7];
};

struct PulseArgBlock_t
{
	CUtlVectorFixed< void *, 16 > m_Values;
	void* m_pImpl;
};

struct PulseBindingDesc_t
{
	typedef PulseMethodArgList_t ( *GetArgListFunc_t )();
	typedef uint32 ( *InvokeFunc_t )( CBasePulseGraphInstance *pGraphInstance, CPulseExecCursor *pCursor, void *pTarget, const PulseHostTable_t *pHosts, PulseArgBlock_t *pInParams, uint64 nMovableArgsMask, PulseArgBlock_t *pOutParams );

	const char* m_pName;
	const char* m_pDisplayName;
	const char* m_pDescription;
	GetArgListFunc_t m_pfnGetArgs;
	GetArgListFunc_t m_pfnGetReturnValues;
	uint16 m_nMetadataCount;
	PulseBindingMetadata_t* m_pMetadata;
	bool m_bIsLibraryMethod;
	bool m_bIsStep;
	bool m_bIsTargetMethod;
	uint32 m_nRequiredCaps;
	bool m_bDynamicArgs;
	InvokeFunc_t m_pfnInvoke;
};

enum PulseBindingCaps_t : uint32
{
	PULSE_BINDING_CAP_NONE				= 0,
	PULSE_BINDING_CAP_ENTITY_IO			= (1 << 1),
	PULSE_BINDING_CAP_YIELDS			= (1 << 3),
	PULSE_BINDING_CAP_OBSERVABLE_PURE	= (1 << 5),
	PULSE_BINDING_CAP_OBSERVABLE_TARGET	= (1 << 6),
};

struct PulseLibraryRegistration_t
{
	PulseLibraryRegistration_t* m_pNext;
	const char* m_pDomainName;
	PulseApiFeature_t m_nFeature;
	int32 m_nMethodCount;
	PulseBindingDesc_t* m_pMethods;
	int32 m_nHookCount;
	void* m_pHooks;
	const char* m_pLibraryName;
	void ( *m_pfnApply )( CPulseAPIExtensionRegistrationContext *pContext );
	bool m_bIsCellLibrary;
	bool m_bExposeAllMethods;
	bool m_bSkip;
};

struct PulseSignatureOutput_t
{
	const char* m_pName;
	int32 m_nUnk0008;
	void* m_pScope;
	int32 m_nOffset;
};

class CDynamicIOInstance;

class CBaseDynamicIOSignature
{
public:
	virtual ~CBaseDynamicIOSignature() = 0;

	int32 FindOutputIndex( const char *pszName ) const;

public:
	uint64 m_unk0008;
	CUtlVector< uint16 > m_inputNames;
	CUtlVector< uint16 > m_outputNames;
	CUtlVector< PulseSignatureOutput_t > m_outputs;
	CUtlHashtable< uint16, int32, MurmurHash3IntFunctor > m_outputNameToIndex;
	CUtlHashtable< uint16, int32, MurmurHash3IntFunctor > m_inputNameToIndex;
	CDynamicIOInstance *m_pInstanceListHead;
};

class CEntityClassPulseSignature : public CBaseDynamicIOSignature
{
public:
	virtual bool AcceptInput( CEntityInstance *pEntity, CUtlSymbolLarge *pInputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const CVariant *pValue, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap ) = 0;

public:
	CEntityClass *m_pOwnerClass;
	CEntityClassPulseSignature *m_pBaseSignature;
	CUtlVector< CUtlSymbolLarge > m_bindingNames;
	CUtlVector< const PulseBindingDesc_t * > m_bindings;
	CUtlVector< PulseSignatureOutput_t > m_registeredOutputs;
	CUtlVector< CUtlString > m_ownedNameStorage;
	bool m_bUnk0110;
};

class CEntitySharedPulseSignature : public CEntityClassPulseSignature
{
};

#endif // ENTITYPULSE_H
