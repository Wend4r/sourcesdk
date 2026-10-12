#ifndef IPOSTPROCESSINGSYSTEM_H
#define IPOSTPROCESSINGSYSTEM_H

#ifdef _WIN32
	#pragma once
#endif

#include <appframework/iappsystem.h>
#include <resourcefile/resourcetype.h>

#include <resourcesystem/resourcehandle.h>
#include <tier0/platform.h>
#include <tier2/tier2.h>

class CPostProcessingState;
struct PostProcessingResource_t;

abstract_class IPostProcessingSystem : public IAppSystem
{
public:
	virtual CPostProcessingState *CreatePostProcessingState() = 0;
	virtual void DestroyPostProcessingState( CPostProcessingState *pState ) = 0;

	virtual void GetNoopColorCorrection( void *pColorCorrection ) = 0;

	virtual void EnableColorCorrectionEditorOverride( bool bEnable ) = 0;
	virtual bool IsColorCorrectionEditorOverrideEnabled() = 0;

	// Keeps a strong reference to hLookup, NULL releases it
	virtual void SetColorCorrectionEditorOverrideLookup( ResourceHandle_t hLookup ) = 0;
	virtual void SetColorCorrectionEditorOverrideParams( const PostProcessingResource_t *pParams ) = 0;

	virtual uint64 AddColorCorrectionLookup( const char *pName, const char *pResourceName ) = 0;
	virtual bool RemoveColorCorrectionLookup( uint64 hLookup ) = 0;

	virtual bool ComputeOverrideColorCorrection( void *p, const void *pParams, void *pColorCorrection ) = 0;

	virtual void unk021( void *p ) = 0;
};

//-----------------------------------------------------------------------------
// Implementation of IPostProcessingSystem
//-----------------------------------------------------------------------------
class CPostProcessingSystem : public CTier2AppSystem< IPostProcessingSystem >
{
public:
	bool m_bColorCorrectionEditorOverrideEnabled;

	// Holds a strong reference to the lookup texture
	CStrongHandleBase m_hColorCorrectionEditorOverrideLookup;

	const PostProcessingResource_t *m_pColorCorrectionEditorOverrideParams;

	// CUtlVector of 24-byte lookups, each found by the handle that AddColorCorrectionLookup() returns
	uint8 m_unk040[ 0x18 ];

	// "Tonemapping_LocalOperator" of "Engine2/RenderingPipeline" in gameinfo
	bool m_bTonemappingLocalOperator;
};

#endif // IPOSTPROCESSINGSYSTEM_H
