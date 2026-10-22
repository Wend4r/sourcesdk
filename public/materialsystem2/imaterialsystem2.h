#ifndef IMATERIALSYSTEM2_H
#define IMATERIALSYSTEM2_H

#ifdef _WIN32
	#pragma once
#endif

#include "imaterial2.h"

#include <appframework/iappsystem.h>
#include <resourcesystem/resourcehandle.h>
#include <resourcefile/resourcetype.h>

#include <mathlib/vector4d.h>
#include <tier0/platform.h>
#include <tier0/threadtools.h>
#include <tier0/utlhashtable.h>
#include <tier0/utlstring.h>
#include <tier0/utlstringtoken.h>
#include <tier0/utlsymbol.h>
#include <tier0/utlvector.h>
#include <tier2/tier2.h>

class KeyValues3;
class IRenderContext;
class CUtlString;
class IMaterialMode;
class CRenderAttributes;
struct MaterialRenderablePass_t;
struct MaterialPassConstantBufferInfo_t;
struct MaterialSystem2GlobalStats_t;
struct RsStencilStateOverride_t;
struct RsDepthBiasStateOverride_t;
class CMaterial2;
class CMaterialTypeManager;
FORWARD_DECLARE_HANDLE( RenderInputLayout_t );
FORWARD_DECLARE_HANDLE( RenderShaderHandle_t );
FORWARD_DECLARE_HANDLE( RenderBufferHandle_t );
FORWARD_DECLARE_HANDLE( ConstantBufferHandle_t );
FORWARD_DECLARE_HANDLE( RsRasterizerStateHandle_t );
FORWARD_DECLARE_HANDLE( RsDepthStencilStateHandle_t );
FORWARD_DECLARE_HANDLE( RsBlendStateHandle_t );

//-----------------------------------------------------------------------------
// Registered with IMaterialSystem2::AddShaderReloadListener()
//-----------------------------------------------------------------------------
abstract_class IShaderReloadListener
{
public:
	// Called by the dynamic shader compile before every material is unloaded for a shader reload
	virtual void OnPreShaderReload() = 0;

	// Called by the dynamic shader compile after the shaders were compiled and the materials reloaded
	virtual void OnPostShaderReload() = 0;
};

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
	virtual void EnableExtendedMaterialInfo() = 0;
	virtual void EnableHeadlessMode() = 0;

	virtual HMaterialWeak FindOrCreateMaterialFromResource( const char *pMaterialName ) = 0;

	virtual bool SetRenderStateForRenderablePass( const CRenderAttributes *pAttributes, const CRenderAttributes *pAttributes2, IRenderContext *pRenderContext, RenderInputLayout_t hInputLayout, const MaterialRenderablePass_t &renderablePass, const RsStencilStateOverride_t *pOverrideStencilState, const RsDepthBiasStateOverride_t *pOverrideDepthBiasState, int nRequiredMipSize, uint32 nRenderStateSetNotAllowed ) = 0;
	virtual bool SetRenderStateForRenderablePass( const CRenderAttributes *pAttributes, IRenderContext *pRenderContext, RenderInputLayout_t hInputLayout, const MaterialRenderablePass_t &renderablePass, const RsStencilStateOverride_t *pOverrideStencilState, const RsDepthBiasStateOverride_t *pOverrideDepthBiasState, int nRequiredMipSize, uint32 nRenderStateSetNotAllowed ) = 0;

	virtual RenderShaderHandle_t GetShaderForRenderablePass( int nShaderType, const MaterialRenderablePass_t &renderablePass ) = 0;
	virtual int GetConstantBufferInfoForRenderablePass( int nShaderType, const MaterialRenderablePass_t &renderablePass, MaterialPassConstantBufferInfo_t *pBufferInfo, int nBufferInfoMax ) = 0;
	virtual bool GetRenderStatesForRenderablePass( int nShaderType, const MaterialRenderablePass_t &renderablePass, void *pRenderStates ) = 0;

	virtual void FrameUpdate() = 0;

	virtual void EnableDynamicShaderCompile() = 0;
	virtual bool IsDynamicShaderCompileEnabled() = 0;

	virtual bool SetMaterialParamFromString( ResourceHandle_t hMaterial, const char *pParamName, const char *pValue ) = 0;

	// Callbacks are run at the start of every FrameUpdate
	virtual void AddFrameUpdateCallback( void ( *pfnCallback )() ) = 0;
	virtual void RemoveFrameUpdateCallback( void ( *pfnCallback )() ) = 0;

	virtual void QueueMaterialReload() = 0;

	// Names the copy "<source>_copy<n>"
	virtual HMaterialWeak CreateProceduralMaterialCopy( ResourceHandle_t hSrcMaterial, int nMode, bool bRecreateStaticBuffers ) = 0;

	// Creates a material from scratch out of a KeyValues3 description.
	virtual HMaterialWeak CreateMaterial( const char *pMaterialName, KeyValues3 *pData, int nFlags, bool bLoadAsync ) = 0;

	// Creates a material derived from an already loaded one, applying overrides on top of it.
	virtual HMaterialWeak CreateMaterial( const char *pMaterialName, ResourceHandle_t hSrcMaterial, KeyValues3 *pOverrides, int nFlags, bool bCreateAsync ) = 0;

	// Creates a material from the KeyValues3 description pData and keeps a strong reference to hResource in it ( see IMaterial2::unk028() )
	virtual HMaterialWeak unk030( const char *pMaterialName, KeyValues3 *pData, ResourceHandle_t hResource, bool bUnk ) = 0;

	// Value of setting.shaderquality
	virtual int GetShaderQuality() = 0;

	// Zeroes the 36-byte block at pStats and returns false
	virtual bool GetGlobalStats( MaterialSystem2GlobalStats_t *pStats, bool bClear ) = 0;

	virtual int GetMaterialGeneration() = 0;
	virtual void IncrementMaterialGeneration() = 0;

	virtual void GetRenderModes( CUtlVector< CUtlStringToken > *pRenderModes, const CUtlVector< const char * > *pModeTypes ) = 0;

	virtual bool GetKV3MaterialDescription( ResourceHandle_t hMaterial, KeyValues3 *pKV ) = 0;

	virtual void unk037( ResourceHandle_t hMaterial, CUtlVector< ResourceHandle_t > *pResources ) = 0;

	virtual void AddShaderReloadListener( IShaderReloadListener *pListener ) = 0;
	virtual void RemoveShaderReloadListener( IShaderReloadListener *pListener ) = 0;

	virtual bool SetRenderStateForMode( const IMaterialMode *pMode, const CRenderAttributes *pAttributes, IRenderContext *pRenderContext ) = 0;
	virtual bool SetRenderStateForMode( const IMaterialMode *pMode, const CRenderAttributes *pAttributes, IRenderContext *pRenderContext, RenderInputLayout_t hInputLayout, const RsStencilStateOverride_t *pOverrideStencilState, const RsDepthBiasStateOverride_t *pOverrideDepthBiasState, int nRequiredMipSize, uint32 nRenderStateSetNotAllowed ) = 0;

	// Requests the textures of every material through IRenderDevice::RequestTextureMipLevels(), with the largest value given for each material
	virtual void RequestTextureMipLevels( int nCount, IMaterial2 *const *ppMaterials, const uint16 *pValues ) = 0;

	// Loaded material whose unique id (CMaterial2::m_nUniqueId) is nUniqueId
	virtual HMaterialWeak unk043( uint32 nUniqueId ) = 0;

	// "pc" or "vulkan" depending on the render device
	virtual void unk044( CUtlString &sName ) = 0;

	// 2 when rendering through Vulkan, 0 otherwise
	virtual int unk045() = 0;

	// Sets CMaterialSystem2::m_bUnk2C0 and returns its previous value
	virtual bool unk046( bool bValue ) = 0;

	virtual void AddMaterialParamChangedListener( void *pListener ) = 0;
	virtual void RemoveMaterialParamChangedListener( void *pListener ) = 0;

	virtual ~IMaterialSystem2() {}
};

//-----------------------------------------------------------------------------
// Implementation of IMaterialSystem2. Containers whose element types are not
// declared in the SDK are kept as padding of their size.
//-----------------------------------------------------------------------------
class CMaterialSystem2 : public CTier2AppSystem< IMaterialSystem2 >
{
public:
	struct ModeInfo_t
	{
		CUtlString m_modeString;
		CUtlStringToken m_modeToken;

		// "game" unless gameinfo gives a type for the mode
		CUtlString m_modeCategory;
	};

	// Set by QueueMaterialReload(), the next FrameUpdate() reloads the materials
	bool m_bMaterialReloadQueued;

	// Set by EnableDynamicShaderCompile()
	bool m_bDynamicShaderCompileEnabled;

	// Registered as "RESOURCE_TYPE_MATERIAL"
	CMaterialTypeManager *m_pMaterialTypeManager;

	// Counted by FrameUpdate(), GetMode() stores it in the material
	uint32 m_nFrameCount;

	// GetMaterialGeneration() and IncrementMaterialGeneration()
	int m_nMaterialGeneration;

	// Shaders, keyed by their symbol in m_shaderNameDictionary
	CUtlHashtable< CUtlSymbol, void *, Mix32HashFunctor > m_Shaders;
	CUtlSymbolTable m_shaderNameDictionary;

	// CUtlHashtable from a CUtlString path to a 0xD0 byte entry, locked by m_unk0E0
	void *m_unk0C0[ 4 ];
	CAtomicMutex m_unk0E0;

	// CUtlVector of 0xF0 byte entries, locked by m_unk0E0 and m_unk120
	void *m_unk0F0[ 3 ];

	// Substrings of the shader names that dynamic shader compile reloads
	CUtlVector< CUtlString > m_DynamicShaderCompileFilters;
	CAtomicMutex m_unk120;

	// "MaterialSystem2/RenderModes" of gameinfo, "Default" when it has none
	CUtlVector< ModeInfo_t > m_modeInfo;

	// CUtlAttributeList< int, 16, int, 16 > from the mode token to its index in m_modeInfo
	ALIGN16 uint8 m_modeAttributeList[ 0x90 ] ALIGN16_POST;

	RsRasterizerStateHandle_t m_hDefaultRasterizerState;
	RsDepthStencilStateHandle_t m_hDefaultDepthStencilState;
	RsBlendStateHandle_t m_hDefaultBlendState;
	ConstantBufferHandle_t m_hEmptyConstantBuffer;

	// AddFrameUpdateCallback() and RemoveFrameUpdateCallback()
	CUtlVector< void ( * )() > m_FrameUpdateFuncs;

	// Materials whose parameters changed, FrameUpdate() rebuilds their buffers
	CUtlVector< CMaterial2 * > m_materialConstantAndCommandBufferRecreationQueue;

	// Set by EnableDynamicShaderCompile(), the shaders are reloaded when it is set
	bool m_bDynamicShaderCompile : 1;
	bool m_bUnk230_1 : 1;

	// "-shaderdebugging"
	bool m_bShaderDebugging : 1;

	bool m_bUnk230_3 : 1;

	// "-batchwarnings" and "-batcherrors"
	bool m_bBatchWarnings : 1;
	bool m_bBatchErrors : 1;

	// EnableExtendedMaterialInfo() and EnableHeadlessMode()
	bool m_bLoadExtendedMaterialInfo : 1;
	bool m_bHeadLessModeEnabled : 1;

	// "-shaderstats"
	bool m_bShaderStats : 1;

	bool m_bUnk231_1 : 1;

	// "MaterialSystem2/ErrorMaterialIsFatalError" of gameinfo
	bool m_bErrorMaterialIsFatalError : 1;

	// Set when shaders were released, the next frame frees the unused ones
	bool m_bShadersReleased : 1;

	// "-materialsystem2_strict"
	bool m_bStrictMode : 1;

	// Set by PreShutdown()
	bool m_bShuttingDown;

	// "MaterialSystem2/ShaderIDColors" of gameinfo, alpha is 1
	CUtlHashtable< CUtlStringToken, Vector4D > m_ShaderIDColors;

	// Bit masks keyed by a 64-bit value
	CUtlHashtable< uint64, uint64 > m_unk258;

	// AddShaderReloadListener() and RemoveShaderReloadListener()
	CUtlVector< IShaderReloadListener * > m_ShaderReloadListeners;

	// AddMaterialParamChangedListener() and RemoveMaterialParamChangedListener()
	CUtlVector< void * > m_MaterialParamChangedListeners;

	// "DefaultTextureBuffer", "DefaultUAVBuffer" and "DefaultUAV.vtex"
	RenderBufferHandle_t m_hDefaultTextureBuffer;
	RenderBufferHandle_t m_hDefaultUAVBuffer;
	CStrongHandleBase m_hDefaultUAVTexture;

	// Set by unk046(), which returns the previous value
	bool m_bUnk2C0;
};

#endif // IMATERIALSYSTEM2_H
