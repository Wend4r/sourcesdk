#include "concreteentitylist.h"
#include "entitynetwork.h"
#include "tier0/memalloc.h"

bool CConcreteEntityList::CList::Remove( CEntityIdentity *pIdentity )
{
	if ( pIdentity->m_pPrev == pIdentity )
		return false;

	if ( pIdentity->m_pPrev )
		pIdentity->m_pPrev->m_pNext = pIdentity->m_pNext;
	else
		m_pHead = pIdentity->m_pNext;

	if ( pIdentity->m_pNext )
		pIdentity->m_pNext->m_pPrev = pIdentity->m_pPrev;
	else
		m_pTail = pIdentity->m_pPrev;

	pIdentity->m_pPrev = pIdentity;
	pIdentity->m_pNext = pIdentity;
	--m_nCount;

	return true;
}

void CConcreteEntityList::CList::AddToTail( CEntityIdentity *pIdentity )
{
	pIdentity->m_pNext = nullptr;
	pIdentity->m_pPrev = m_pTail;
	m_pTail = pIdentity;

	if ( pIdentity->m_pPrev )
		pIdentity->m_pPrev->m_pNext = pIdentity;
	else
		m_pHead = pIdentity;

	++m_nCount;
}

CEntityIndex CConcreteEntityList::GetEntityIndex( const CEntityIdentity *pIdentity ) const
{
	for ( int i = 0; i < MAX_ENTITY_LISTS / 2; ++i )
	{
		const int nChunks[] = { i, i + MAX_ENTITY_LISTS / 2 };

		if ( !m_pIdentityChunks[ nChunks[ 0 ] ] && !m_pIdentityChunks[ nChunks[ 1 ] ] )
			break;

		for ( int iChunk : nChunks )
		{
			const CEntityIdentity *pChunk = m_pIdentityChunks[ iChunk ];

			if ( pChunk && pIdentity >= pChunk && pIdentity < pChunk + MAX_ENTITIES_IN_LIST )
				return CEntityIndex( iChunk * MAX_ENTITIES_IN_LIST + static_cast< int >( pIdentity - pChunk ) );
		}
	}

	return CEntityIndex();
}

void CConcreteEntityList::FreeIdentity( CEntityIdentity *pIdentity, bool bInvalidateHandle )
{
	if ( pIdentity->m_flags & ( EF_IS_ISOLATED_ALLOCATION | EF_IS_ANONYMOUS_ALLOCATION ) )
	{
		Release( pIdentity );

		return;
	}

	const int nIndex = GetEntityIndex( pIdentity ).Get();

	if ( nIndex < 0 )
		return;

	if ( nIndex < static_cast< int >( m_nNetworkableEntityLimit ) )
		m_PVSBits.Clear( nIndex );

	// The identity is the slot at nIndex; a slot whose handle has another index is not in use.
	if ( pIdentity->m_EHandle.GetEntryIndex() != nIndex )
		return;

	if ( pIdentity->m_flags & EF_IS_DORMANT )
	{
		m_dormantList.Remove( pIdentity );
	}
	else if ( bInvalidateHandle )
	{
		if ( m_pFirstActiveEntity == pIdentity )
			m_pFirstActiveEntity = pIdentity->m_pNext;

		m_usedList.Remove( pIdentity );
	}

	pIdentity->m_EHandle.SetEntryIndex( MAX_TOTAL_ENTITIES - 1 );

	// The slots of the player entities are not returned to a free list.
	if ( nIndex >= static_cast< int >( m_nNetworkableEntityLimit ) )
	{
		m_freeNonNetworkableList.Remove( pIdentity );
		m_freeNonNetworkableList.AddToTail( pIdentity );
	}
	else if ( static_cast< int >( m_nMaxPlayers ) + 1 < nIndex )
	{
		m_freeNetworkableList.Remove( pIdentity );
		m_freeNetworkableList.AddToTail( pIdentity );
	}

	if ( bInvalidateHandle )
		pIdentity->m_EHandle.IncrementSerialNumber();

	pIdentity->Clear();
}
