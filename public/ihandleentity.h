//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef IHANDLEENTITY_H
#define IHANDLEENTITY_H
#ifdef _WIN32
#pragma once
#endif



// An IHandleEntity-derived class can go into an entity list and use ehandles.
class IHandleEntity
{
public:
	virtual ~IHandleEntity() {}
	virtual void SetRefEHandle( const CBaseHandle &handle ) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
#ifdef _X360
	IHandleEntity() :
		m_bIsStaticProp( false )
	{
	}

	bool m_bIsStaticProp;
#endif
};


#endif // IHANDLEENTITY_H
