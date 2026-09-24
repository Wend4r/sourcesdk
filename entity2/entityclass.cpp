#include "entityclass.h"
#include "entityio.h"
#include "entitysystem.h"

CEntityIOOutput *EntOutput_t::GetOutput( CEntityInstance *pEntity ) const
{
	if ( m_nOutputOffset == ENTITY_INVLAID_OUTPUT_OFFSET )
		return nullptr;

	void *pOwner = m_pfnGetOutputOwner ? m_pfnGetOutputOwner( pEntity ) : pEntity;

	if ( !pOwner )
		return nullptr;

	return reinterpret_cast< CEntityIOOutput * >( reinterpret_cast< byte * >( pOwner ) + m_nOutputOffset );
}

CEntityIOOutput *CEntityClass::FindOutput( const char *pszName, CEntityInstance *pEntity )
{
	CEntityClass *pClass = this;

	do
	{
		// m_classOutputInfos is sorted by the address of the pooled name.
		CUtlSymbolLarge sName = GameEntitySystem()->AllocPooledString( pszName );
		int iLow = 0;
		int iHigh = pClass->m_classOutputInfos.Count() - 1;

		while ( iLow <= iHigh )
		{
			int iMid = ( iLow + iHigh ) >> 1;
			const ClassOutputInfo_t &info = pClass->m_classOutputInfos[ iMid ];

			if ( info.m_sName < sName )
				iLow = iMid + 1;
			else if ( sName < info.m_sName )
				iHigh = iMid - 1;
			else
				return info.m_pOutput->GetOutput( pEntity );
		}

		if ( !pClass->m_pBaseClassInfo )
			break;

		pClass = pClass->m_pBaseClassInfo->m_pClass;
	}
	while ( pClass );

	return nullptr;
}

bool CEntityClass::AcceptInput( CEntityInstance *pEntity, const CUtlSymbolLarge &sInputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const variant_t &value, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap )
{
	for ( CEntityClass *pClass = this; pClass; pClass = pClass->m_pBaseClassInfo ? pClass->m_pBaseClassInfo->m_pClass : nullptr )
	{
		CEntityClassPulseSignature *pSignature = pClass->m_pSharedPulseSignature;

		if ( !pSignature )
			continue;

		CUtlSymbolLarge sName = sInputName;

		if ( pSignature->AcceptInput( pEntity, &sName, pActivator, pCaller, &value, pArgs, pParamMap ) )
			return true;
	}

	return false;
}
