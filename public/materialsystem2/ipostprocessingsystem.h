#ifndef IPOSTPROCESSINGSYSTEM_H
#define IPOSTPROCESSINGSYSTEM_H

#ifdef _WIN32
	#pragma once
#endif

#include <appframework/iappsystem.h>
#include <resourcefile/resourcetype.h>

#include <tier0/platform.h>

class CPostProcessingState;

abstract_class IPostProcessingSystem : public IAppSystem
{
public:
	virtual CPostProcessingState *CreatePostProcessingState() = 0;
	virtual void DestroyPostProcessingState( CPostProcessingState *pState ) = 0;

	virtual void GetNoopColorCorrection( void *pColorCorrection ) = 0;

	// Setter and getter of the same flag
	virtual void unk014( bool bValue ) = 0;
	virtual bool unk015() = 0;

	// Keeps a strong reference to hResource, NULL releases it
	virtual void unk016( ResourceHandle_t hResource ) = 0;
	virtual void unk017( void *p ) = 0;

	virtual uint64 unk018( const char *pName, const char *pResourceName ) = 0;
	virtual bool unk019( uint64 hEntry ) = 0;

	virtual bool ComputeOverrideColorCorrection( void *p, const void *pParams, void *pColorCorrection ) = 0;

	virtual void unk021( void *p ) = 0;
};

#endif // IPOSTPROCESSINGSYSTEM_H
