#ifndef RESOURCENAME_H
#define RESOURCENAME_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/bufferstring.h"
#include "resourcefile/resourcetype.h"

typedef CBufferStringN< 200 > CPathBufferString;

// Name of a resource as data files store it: the path, and the id and type the resource system
// resolves it to.
class CResourceName
{
public:
	const char *GetName() const { return m_Name.Get(); }

	CPathBufferString m_Name;
	ResourceId_t m_Id;
	ResourceType_t m_Type;
};

// A resource name for one handle type, e.g. CResourceNameTyped< CWeakHandle< InfoForResourceTypeCModel > >.
// It adds nothing to CResourceName; the handle type only tells the tools which resources may be named.
template < typename THandle >
class CResourceNameTyped : public CResourceName
{
public:
	typedef THandle Handle_t;
};

#endif // RESOURCENAME_H
