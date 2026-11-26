#ifndef PATH_NODE_H
#define PATH_NODE_H

#pragma once

#include "baseentity.h"

class CPathWithDynamicNodes;

//-----------------------------------------------------------------------------
// CPathNode adds 1 method to CPointEntity (through CPointEntity, which adds no method): Linux slot 239, Windows 238.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CPathNodeVTable : public TBase
{
public:
	// Stores the handle in m_hPath. Named by what it does
	virtual void SetPath( CPathWithDynamicNodes *pPath ) = 0;
};

#endif // PATH_NODE_H
