#ifndef IMATERIALSYSTEM2_H
#define IMATERIALSYSTEM2_H

#ifdef _WIN32
	#pragma once
#endif

#include "imaterial2.h"

#include <appframework/iappsystem.h>
#include <resourcefile/resourcehandle.h>
#include <resourcefile/resourcetype.h>

#include <tier0/platform.h>
#include <tier0/utlstringtoken.h>
#include <tier1/utlvector.h>

class KeyValues3;
class IRenderContext;
class CUtlString;

//-----------------------------------------------------------------------------
// Source 2 material system, exposed as MATERIAL_SYSTEM2_INTERFACE_VERSION
// ( see g_pMaterialSystem2 ).
//
//
//-----------------------------------------------------------------------------
abstract_class IMaterialSystem2 : public IAppSystem
{
public:
	virtual const char *GetRenderModeName( const CUtlStringToken &renderMode ) = 0;

	// Set two bits of an internal flag byte
	virtual void unk012() = 0;
	virtual void unk013() = 0;

	virtual HMaterialWeak FindOrCreateMaterialFromResource( const char *pMaterialName ) = 0;

	virtual bool Unk_RenderablePass( void *p1, void *p2, void *p3, void *p4, void *pPass, void *p6, void *p7, int n8, int n9 ) = 0;
	virtual bool Unk_RenderablePass( void *p1, void *p2, void *p3, void *pPass, void *p5, void *p6, int n7, int n8 ) = 0;

	virtual void *unk017( int nMode, void *pPass ) = 0;
	virtual int unk018( int nMode, void *pPass, int n3, int n4 ) = 0;
	virtual bool unk019( void *p, void *pPass ) = 0;

	virtual void FrameUpdate() = 0;

	virtual void EnableDynamicShaderCompile() = 0;
	virtual bool IsDynamicShaderCompileEnabled() = 0;

	virtual bool SetMaterialParamFromString( ResourceHandle_t hMaterial, const char *pParamName, const char *pValue ) = 0;

	// Callbacks are run at the start of every FrameUpdate
	virtual void AddFrameUpdateCallback( void ( *pfnCallback )() ) = 0;
	virtual void RemoveFrameUpdateCallback( void ( *pfnCallback )() ) = 0;

	virtual void QueueMaterialReload() = 0;

	virtual HMaterialWeak unk027( ResourceHandle_t hSrcMaterial, int nMode, bool bUnk ) = 0;

	// Creates a material from scratch out of a KeyValues3 description.
	virtual HMaterialWeak CreateMaterial( const char *pMaterialName, KeyValues3 *pData, int nFlags, bool bUnk ) = 0;

	// Creates a material derived from an already loaded one, applying overrides on top of it.
	virtual HMaterialWeak CreateMaterial( const char *pMaterialName, ResourceHandle_t hSrcMaterial, KeyValues3 *pOverrides, int nFlags, bool bUnk ) = 0;

	virtual HMaterialWeak unk030( const char *pMaterialName, KeyValues3 *pData, ResourceHandle_t hResource, bool bUnk ) = 0;

	// Value of setting.shaderquality
	virtual int GetShaderQuality() = 0;

	// Zeroes the 36-byte block at pOut and returns false
	virtual bool unk032( void *pOut ) = 0;

	virtual int GetMaterialGeneration() = 0;
	virtual void IncrementMaterialGeneration() = 0;

	virtual void GetRenderModes( CUtlVector< CUtlStringToken > *pRenderModes, const CUtlVector< const char * > *pModeTypes ) = 0;

	virtual bool unk036( ResourceHandle_t hMaterial, KeyValues3 *pKV ) = 0;

	virtual void unk037( ResourceHandle_t hMaterial, CUtlVector< ResourceHandle_t > *pResources ) = 0;

	virtual void AddShaderReloadListener( void *pListener ) = 0;
	virtual void RemoveShaderReloadListener( void *pListener ) = 0;

	virtual bool SetRenderStateForMode( void *p1, void *p2, IRenderContext *pRenderContext ) = 0;
	virtual bool SetRenderStateForMode( void *p1, void *p2, IRenderContext *pRenderContext, void *p4, void *p5, void *p6, int n7, int n8 ) = 0;

	virtual void unk042( int nCount, IMaterial2 *const *ppMaterials, const uint16 *pValues ) = 0;

	virtual HMaterialWeak unk043( int nMaterialId ) = 0;

	// "pc" or "vulkan" depending on the render device
	virtual void unk044( CUtlString &sName ) = 0;

	// 2 when rendering through Vulkan, 0 otherwise
	virtual int unk045() = 0;

	virtual bool unk046( bool bValue ) = 0;

	virtual void AddMaterialParamChangedListener( void *pListener ) = 0;
	virtual void RemoveMaterialParamChangedListener( void *pListener ) = 0;

	virtual ~IMaterialSystem2() {}
};

#endif // IMATERIALSYSTEM2_H
