//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef DT_UTLVECTOR_COMMON_H
#define DT_UTLVECTOR_COMMON_H
#ifdef _WIN32
#pragma once
#endif


#include "utlvector.h"


typedef void (*EnsureCapacityFn)( void *pVoid, int offsetToUtlVector, int len );
typedef void (*ResizeUtlVectorFn)( void *pVoid, int offsetToUtlVector, int len );

template< class T, typename I = size_t >
void UtlVector_InitializeAllocatedElements( T *pBase, I count )
{
	memset( pBase, 0, count * sizeof( T ) );
}

template< class T, class I, class A >
class UtlVectorTemplate
{
public:
	static void ResizeUtlVector( void *pStruct, I offsetToUtlVector, I len )
	{
		CUtlVector<T,I,A> *pVec = (CUtlVector<T,I,A>*)((char*)pStruct + offsetToUtlVector);
		if ( pVec->Count() < len )
			pVec->AddMultipleToTail( len - pVec->Count() );
		else if ( pVec->Count() > len )
			pVec->RemoveMultiple( len, pVec->Count()-len );

		// Ensure capacity
		pVec->EnsureCapacity( len );

		I nNumAllocated = pVec->NumAllocated();

		// This is important to do because EnsureCapacity doesn't actually call the constructors
		// on the elements, but we need them to be initialized, otherwise it'll have out-of-range
		// values which will piss off the datatable encoder.
		UtlVector_InitializeAllocatedElements( pVec->Base() + pVec->Count(), nNumAllocated - pVec->Count() );
	}

	static void EnsureCapacity( void *pStruct, I offsetToUtlVector, I len )
	{
		CUtlVector<T,I,A> *pVec = (CUtlVector<T,I,A>*)((char*)pStruct + offsetToUtlVector);

		pVec->EnsureCapacity( len );
		
		I nNumAllocated = pVec->NumAllocated();

		// This is important to do because EnsureCapacity doesn't actually call the constructors
		// on the elements, but we need them to be initialized, otherwise it'll have out-of-range
		// values which will piss off the datatable encoder.
		UtlVector_InitializeAllocatedElements( pVec->Base() + pVec->Count(), nNumAllocated - pVec->Count() );
	}
};

template< class T, typename I, class A >
inline ResizeUtlVectorFn GetResizeUtlVectorTemplate( CUtlVector<T,I,A> &vec )
{
	return &UtlVectorTemplate<T,A>::ResizeUtlVector;
}

template< class T, typename I, class A >
inline EnsureCapacityFn GetEnsureCapacityTemplate( CUtlVector<T,I,A> &vec )
{
	return &UtlVectorTemplate<T,A>::EnsureCapacity;
}


// Format and allocate a string.
char* AllocateStringHelper( const char *pFormat, ... );

// Allocates a string for a data table name. Data table names must be unique, so this will
// assert if you try to allocate a duplicate.
char* AllocateUniqueDataTableName( bool bSendTable, const char *pFormat, ... );


#endif // DT_UTLVECTOR_COMMON_H
