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

class CBasePulseGraphInstance;
class CEntityClass;
class CEntityInstance;
class CPulseAPIExtensionRegistrationContext;
class CPulseExecCursor;
class CPulseRuntimeMethodArg;
class KeyValues3;

enum PulseApiFeature_t : uint32
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

enum PulseValueType_t : int32
{
	PVAL_VOID						= -1,
	PVAL_BOOL						= 0,
	PVAL_INT						= 1,
	PVAL_FLOAT						= 2,
	PVAL_STRING						= 3,
	PVAL_VEC2						= 4,
	PVAL_VEC3						= 5,
	PVAL_QANGLE						= 6,
	PVAL_VEC3_WORLDSPACE			= 7,
	PVAL_VEC4						= 8,
	PVAL_TRANSFORM					= 9,
	PVAL_TRANSFORM_WORLDSPACE		= 10,
	PVAL_COLOR_RGB					= 11,
	PVAL_GAMETIME					= 12,
	PVAL_EHANDLE					= 13,
	PVAL_RESOURCE					= 14,
	PVAL_RESOURCE_NAME				= 15,
	PVAL_SNDEVT_GUID				= 16,
	PVAL_SNDEVT_NAME				= 17,
	PVAL_ENTITY_NAME				= 18,
	PVAL_OPAQUE_HANDLE				= 19,
	PVAL_TYPESAFE_INT				= 20,
	PVAL_MODEL_MATERIAL_GROUP		= 21,
	PVAL_CURSOR_FLOW				= 22,
	PVAL_VARIANT					= 23,
	PVAL_UNKNOWN					= 24,
	PVAL_SCHEMA_ENUM				= 25,
	PVAL_PANORAMA_PANEL_HANDLE		= 26,
	PVAL_TEST_HANDLE				= 27,
	PVAL_ARRAY						= 28,
	PVAL_TYPESAFE_INT64				= 29,
	PVAL_PARTICLE_EHANDLE			= 30,
	PVAL_ANIM_SEQUENCE				= 31,
	PVAL_VDATA_CHOICE				= 32,
	PVAL_COUNT						= 33,
};

class CPulseValueFullType
{
public:
	PulseValueType_t m_nType;
	void* m_pUnk0008;
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
	int32 m_nCount;
	void* m_pValues[16];
	void* m_pImpl;
};

struct PulseBindingDesc_t
{
	typedef void ( *GetArgListFunc_t )( PulseMethodArgList_t *pOut );
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

class CBaseDynamicIOSignature
{
public:
	virtual ~CBaseDynamicIOSignature() = 0;

public:
	uint64 m_unk0008;
	CUtlVector< uint16 > m_inputNames;
	CUtlVector< uint16 > m_outputNames;
	CUtlVector< PulseSignatureOutput_t > m_outputs;
	CUtlHashtable< uint16, int32 > m_outputNameToIndex;
	CUtlHashtable< uint16, int32 > m_inputNameToIndex;
	void *m_pInstanceListHead;
};

class CEntityClassPulseSignature : public CBaseDynamicIOSignature
{
public:
	virtual bool AcceptInput( CEntityInstance *pEntity, CUtlSymbolLarge *pInputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const CVariant *pValue, void *pUnk, KeyValues3 *pParams ) = 0;

public:
	CEntityClass* m_pOwnerClass;
	CEntityClassPulseSignature* m_pBaseSignature;
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
