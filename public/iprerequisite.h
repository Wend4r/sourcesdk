#ifndef IPREREQUISITE_H
#define IPREREQUISITE_H

#ifdef _WIN32
#pragma once
#endif

#include "resourcefile/resourcetype.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"

class IGameResourceService;

enum PrerequisiteStatus_t
{
	// Not finished yet, polled again later
	PS_PENDING = 0,
	PS_SATISFIED,
	PS_FAILED,
};

// Status report a prerequisite tree writes itself into, one indented line per prerequisite
class CPrerequisiteDescriptionHelper
{
public:
	struct Line_t
	{
		int m_nIndent;
		CUtlString m_sLabel;
		CUtlString m_sText;
	};

	CUtlVector< Line_t > m_Lines;
	// Length of the longest m_sLabel
	int m_nMaxLabelLength;
};

class IPrerequisite
{
public:
	virtual ~IPrerequisite() {}

	virtual void AddRef() { ++m_nRefCount; }
	virtual void Release() { if ( --m_nRefCount <= 0 ) delete this; }

	// Advances the prerequisite; a sequence stops polling its children once Plat_FloatTime() reaches flEndTime
	// (DBL_MAX: no limit).
	virtual PrerequisiteStatus_t Update( double flEndTime ) = 0;
	virtual const char *GetName() = 0;
	virtual void Describe( int nIndent, CPrerequisiteDescriptionHelper *pHelper );

public:
	int m_nRefCount;
};

// Satisfied once every manifest of m_Manifests is loaded
class CGameResourceManifestPrerequisite : public IPrerequisite
{
public:
	virtual ~CGameResourceManifestPrerequisite();
	virtual PrerequisiteStatus_t Update( double flEndTime ) override;
	virtual const char *GetName() override;
	virtual void Describe( int nIndent, CPrerequisiteDescriptionHelper *pHelper ) override;

	// Called by Update with PS_SATISFIED once all the manifests are loaded
	virtual void OnStatusFinished( PrerequisiteStatus_t nStatus );

public:
	IGameResourceService *m_pGameResourceService;
	CUtlVector< HGameResourceManifest > m_Manifests;
};

#endif // IPREREQUISITE_H
