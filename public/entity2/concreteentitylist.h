#ifndef CONCRETEENTITYLIST_H
#define CONCRETEENTITYLIST_H

#include "bitvec.h"
#include "entityidentity.h"

class CConcreteEntityList
{
	struct CList
	{
		// An identity that is in no list links to itself
		bool Remove( CEntityIdentity *pIdentity );
		void AddToTail( CEntityIdentity *pIdentity );

		CEntityIdentity* m_pHead;
		CEntityIdentity* m_pTail;
		int m_nCount;
	};
public:
	// -1 when the identity is not in any chunk
	CEntityIndex GetEntityIndex( const CEntityIdentity *pIdentity ) const;

	void FreeIdentity( CEntityIdentity *pIdentity, bool bInvalidateHandle );

public:
	CEntityIdentity* m_pIdentityChunks[MAX_ENTITY_LISTS];
	CEntityIdentity* m_pFirstActiveEntity; // 512
	CConcreteEntityList::CList m_usedList;	// 520
	CConcreteEntityList::CList m_dormantList; // 544
	CConcreteEntityList::CList m_freeNetworkableList; // 568
	CConcreteEntityList::CList m_freeNonNetworkableList; // 592
	uint32 m_nNetworkableEntityLimit; // 0x268
	uint32 m_nNonNetworkableEntityLimit; // 0x26c
	uint32 m_nMaxPlayers;
	CBitVec<16384> m_PVSBits;
};

#endif // CONCRETEENTITYLIST_H
