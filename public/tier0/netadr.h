//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// netadr.h
#ifndef NETADR_H
#define NETADR_H
#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include <steam/steamclientpublic.h>

#undef SetPort

class bf_read;
class bf_write;

typedef enum
{ 
	NA_NULL = 0,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
} netadrtype_t;

struct netadr_t
{
public:
	netadr_t() { SetIP( 0 ); SetPort( 0 ); SetType( NA_IP ); }
	netadr_t( uint unIP, uint16 usPort ) { SetIP( unIP ); SetPort( usPort ); SetType( NA_IP ); }
	netadr_t( const char *pch ) { SetFromString( pch ); }
	DLL_CLASS_IMPORT void	Clear();	// invalids Address

	DLL_CLASS_IMPORT void	SetType( netadrtype_t type );
	DLL_CLASS_IMPORT void	SetPort( unsigned short port );
	DLL_CLASS_IMPORT bool	SetFromSockadr(const struct sockaddr *s);
	DLL_CLASS_IMPORT void	SetIP(uint8 b1, uint8 b2, uint8 b3, uint8 b4);
	DLL_CLASS_IMPORT void	SetIP(uint unIP);									// Sets IP.  unIP is in host order (little-endian)
	DLL_CLASS_IMPORT void	SetIPAndPort( uint unIP, unsigned short usPort ) { SetIP( unIP ); SetPort( usPort ); }
	DLL_CLASS_IMPORT bool	SetFromString(const char *pch, bool bUseDNS = false ); // if bUseDNS is true then do a DNS lookup if needed
	
	DLL_CLASS_IMPORT bool	CompareAdr (const netadr_t &a, bool onlyBase = false) const;
	DLL_CLASS_IMPORT bool	CompareClassBAdr (const netadr_t &a) const;
	DLL_CLASS_IMPORT bool	CompareClassCAdr (const netadr_t &a) const;

	DLL_CLASS_IMPORT netadrtype_t	GetType() const;
	DLL_CLASS_IMPORT unsigned short	GetPort() const;

	// DON'T CALL THIS
	DLL_CLASS_IMPORT const char*		ToString( bool onlyBase = false ) const; // returns xxx.xxx.xxx.xxx:ppppp
	DLL_CLASS_IMPORT void				ToString( char *pchBuffer, uint32 unBufferSize, bool onlyBase = false ) const; // returns xxx.xxx.xxx.xxx:ppppp

	template< size_t maxLenInChars >
	void	ToString_safe( char (&pDest)[maxLenInChars], bool onlyBase = false ) const
	{
		ToString( &pDest[0], maxLenInChars, onlyBase );
	}

	DLL_CLASS_IMPORT void			ToSockadr(struct sockaddr *s) const;

	// Returns 0xAABBCCDD for AA.BB.CC.DD on all platforms, which is the same format used by SetIP().
	// (So why isn't it just named GetIP()?  Because previously there was a fucntion named GetIP(), and
	// it did NOT return back what you put into SetIP().  So we nuked that guy.)
	DLL_CLASS_IMPORT unsigned int	GetIPHostByteOrder() const;

	// Returns a number that depends on the platform.  In most cases, this probably should not be used.
	DLL_CLASS_IMPORT unsigned int	GetIPNetworkByteOrder() const;

	DLL_CLASS_IMPORT bool	IsLocalhost() const; // true, if this is the localhost IP 
	DLL_CLASS_IMPORT bool	IsLoopback() const;	// true if engine loopback buffers are used
	DLL_CLASS_IMPORT bool	IsReservedAdr() const; // true, if this is a private LAN IP
	DLL_CLASS_IMPORT bool	IsValid() const;	// ip & port != 0
	DLL_CLASS_IMPORT bool	IsBaseAdrValid() const;	// ip != 0

	DLL_CLASS_IMPORT void    SetFromSocket( int hSocket );

	DLL_CLASS_IMPORT bool	Unserialize( bf_read &readBuf );
	DLL_CLASS_IMPORT bool	Serialize( bf_write &writeBuf );

	bool operator==(const netadr_t &netadr) const {return ( CompareAdr( netadr ) );}
	bool operator!=(const netadr_t &netadr) const {return !( CompareAdr( netadr ) );}
	DLL_CLASS_IMPORT bool operator<(const netadr_t &netadr) const;

public:	// members are public to avoid to much changes

	netadrtype_t	type;
	unsigned char	ip[4];
	unsigned short	port;
};

typedef netadr_t netadr_s;

/// Helper class to render a netadr_t.  Use this when formatting a net address
/// in a printf.  Don't use adr.ToString()!
class CUtlNetAdrRender
{
public:
	CUtlNetAdrRender( const netadr_t &obj, bool bBaseOnly = false )
	{
		obj.ToString( m_rgchString, sizeof(m_rgchString), bBaseOnly );
	}

	CUtlNetAdrRender( uint32 unIP )
	{
		netadr_t addr( unIP, 0 );
		addr.ToString( m_rgchString, sizeof(m_rgchString), true );
	}

	CUtlNetAdrRender( uint32 unIP, uint16 unPort )
	{
		netadr_t addr( unIP, unPort );
		addr.ToString( m_rgchString, sizeof(m_rgchString), false );
	}

	CUtlNetAdrRender( const struct sockaddr &s )
	{
		netadr_t addr;
		if ( addr.SetFromSockadr( &s ) )
			addr.ToString( m_rgchString, sizeof(m_rgchString), false );
		else
			m_rgchString[0] = '\0';
	}

	const char * String() const
	{ 
		return m_rgchString;
	}

private:

	char m_rgchString[32];
};

#endif // NETADR_H
