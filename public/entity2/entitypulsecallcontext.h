#ifndef ENTITYPULSECALLCONTEXT_H
#define ENTITYPULSECALLCONTEXT_H

#include "entitypulse.h"
#include "tier1/keyvalues3.h"

class CPulseCallContextValue
{
public:
	void *m_pValue;
	CPulseValueFullType m_Type;
};
COMPILE_TIME_ASSERT( sizeof( CPulseCallContextValue ) == 32 );

class CPulseCallContextInfo
{
public:
	// The runtime uses the argument list when present and keeps key/type overrides here
	const PulseMethodArgList_t *m_pArguments;
	int32 m_nKeyCount;
	uint32 m_nKeyCapacityAndFlags;
	CKV3MemberNameWithStorage *m_pKeys;
	int32 m_nTypeCount;
	CPulseValueFullType *m_pTypes[ 16 ];
	int32 m_nValueCount;
	CPulseCallContextValue *m_pValues[ 16 ];
};
COMPILE_TIME_ASSERT( sizeof( CPulseCallContextInfo ) == 296 );
COMPILE_TIME_ASSERT( alignof( CPulseCallContextInfo ) == 8 );

class CPulseArgumentPack
{
public:
	int Count() const { return m_nCount; }
	bool HasValue( int nIndex ) const { return m_pInfo->m_pValues[ nIndex ] != nullptr; }

	// The runtime owns the info block and the objects referenced by its arrays
	int32 m_nCount;
	void *m_pValues[ 16 ];
	CPulseCallContextInfo *m_pInfo;
};
COMPILE_TIME_ASSERT( sizeof( CPulseArgumentPack ) == 144 );
COMPILE_TIME_ASSERT( alignof( CPulseArgumentPack ) == 8 );

using CPulseCallContext = CPulseArgumentPack;

class CPulseInputParamMap
{
public:
	KeyValues3 m_KV3;
	KeyValues3::Data_t m_KV3Data;
};
COMPILE_TIME_ASSERT( sizeof( CPulseInputParamMap ) == 24 );

#endif // ENTITYPULSECALLCONTEXT_H
