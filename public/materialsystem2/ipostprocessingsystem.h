#ifndef IPOSTPROCESSINGSYSTEM_H
#define IPOSTPROCESSINGSYSTEM_H

#ifdef _WIN32
	#pragma once
#endif

#include <appframework/iappsystem.h>
#include <resourcefile/resourcetype.h>

#include <tier0/platform.h>

class CPostProcessingState;
struct PostProcessingResource_t;

//-----------------------------------------------------------------------------
// Post processing (color correction) system of the material system.
//
// Slots 14-19 keep their Dota 2 (2018, Mach-O symbols) names: the CS2 bodies do
// what the Dota methods of the same names did (editor override flag, override
// lookup texture, override params pointer, lookup add/remove by name).
//-----------------------------------------------------------------------------
abstract_class IPostProcessingSystem : public IAppSystem
{
public:
	virtual CPostProcessingState *CreatePostProcessingState() = 0;
	virtual void DestroyPostProcessingState( CPostProcessingState *pState ) = 0;

	virtual void GetNoopColorCorrection( void *pColorCorrection ) = 0;

	virtual void EnableColorCorrectionEditorOverride( bool bEnable ) = 0;
	virtual bool IsColorCorrectionEditorOverrideEnabled() const = 0;

	// Keeps a counted reference to the lookup texture, NULL releases it
	virtual void SetColorCorrectionEditorOverrideLookup( ResourceHandle_t hLookupTexture ) = 0;
	virtual void SetColorCorrectionEditorOverrideParams( const PostProcessingResource_t *pParams ) = 0;

	// Returns the handle of the (existing or new) lookup entry
	virtual uint64 AddColorCorrectionLookup( const char *pName, const char *pResourceName ) = 0;
	virtual bool RemoveColorCorrectionLookup( uint64 hLookup ) = 0;

	virtual bool ComputeOverrideColorCorrection( void *p, const void *pParams, void *pColorCorrection ) = 0;

	// Named by what it does: the pointer at +8 of the resource data of the handle, NULL for a
	// NULL handle
	virtual void *GetPostProcessingResourceData( ResourceHandle_t hResource ) = 0;
};

#endif // IPOSTPROCESSINGSYSTEM_H
