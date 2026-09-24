#include "entityidentity.h"
#include "const.h"
#include "entityclass.h"
#include "entityinstance.h"
#include "entitynetwork.h"
#include "entitysystem.h"
#include "tier0/strtools.h"

bool CEntityIdentity::IsNetworked() const
{
	if ( m_flags & EF_IS_ISOLATED_ALLOCATION )
		return ( m_flags & EF_IS_ISOLATED_ALLOCATION_NETWORKABLE ) != 0;

	return m_EHandle.IsValid() && m_EHandle.m_Parts.m_EntityIndex < MAX_EDICTS;
}

void CEntityIdentity::NetworkStateChanged( const NetworkStateChanged_t &data )
{
	if ( !m_pInstance )
		return;

	NetworkStateChanged_t changed( data );

	changed.m_nPathIndex = m_PathIndex;
	m_pInstance->NetworkStateChanged( changed );
}

const char *CEntityIdentity::GetDebugName() const
{
	return m_name.IsValid() ? m_name.String() : m_pClass->m_pClassInfo->m_pszClassname;
}

bool CEntityIdentity::NameMatches( const char* pszNameOrWildcard ) const
{
	if ( pszNameOrWildcard && pszNameOrWildcard[0] == '!' )
		return GameEntitySystem()->FindEntityProcedural( pszNameOrWildcard ) == m_pInstance;

	return V_CompareNameWithWildcards( pszNameOrWildcard, m_name.String() ) == 0;
}

bool CEntityIdentity::ClassMatches( const char* pszClassOrWildcard ) const
{
	return V_CompareNameWithWildcards( pszClassOrWildcard, m_designerName.String() ) == 0;
}
