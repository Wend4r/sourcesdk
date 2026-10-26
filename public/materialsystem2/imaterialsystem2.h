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
#include <tier0/bufferstring.h>
#include <tier0/platform.h>
#include <tier0/threadtools.h>
#include <tier0/utlattributelist.h>
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

	// Creates a material from the KeyValues3 description pData and keeps a strong reference to hResource in it
	// ( see IMaterial2::GetReferencedResource() ). bCheckValid is passed to the lookups of the texture parameters
	virtual HMaterialWeak CreateMaterialReferencingResource( const char *pMaterialName, KeyValues3 *pData, ResourceHandle_t hResource, bool bCheckValid ) = 0;

	// Value of setting.shaderquality
	virtual int GetShaderQuality() = 0;

	// Zeroes the 36-byte block at pStats and returns false
	virtual bool GetGlobalStats( MaterialSystem2GlobalStats_t *pStats, bool bClear ) = 0;

	virtual int GetMaterialGeneration() = 0;
	virtual void IncrementMaterialGeneration() = 0;

	virtual void GetRenderModes( CUtlVector< CUtlStringToken > *pRenderModes, const CUtlVector< const char * > *pModeTypes ) = 0;

	virtual bool GetKV3MaterialDescription( ResourceHandle_t hMaterial, KeyValues3 *pKV ) = 0;

	// Appends the textures of the texture parameters; nothing for a fallback material
	virtual void GetMaterialTextures( ResourceHandle_t hMaterial, CUtlVector< ResourceHandle_t > *pTextures ) = 0;

	virtual void AddShaderReloadListener( IShaderReloadListener *pListener ) = 0;
	virtual void RemoveShaderReloadListener( IShaderReloadListener *pListener ) = 0;

	virtual bool SetRenderStateForMode( const IMaterialMode *pMode, const CRenderAttributes *pAttributes, IRenderContext *pRenderContext ) = 0;
	virtual bool SetRenderStateForMode( const IMaterialMode *pMode, const CRenderAttributes *pAttributes, IRenderContext *pRenderContext, RenderInputLayout_t hInputLayout, const RsStencilStateOverride_t *pOverrideStencilState, const RsDepthBiasStateOverride_t *pOverrideDepthBiasState, int nRequiredMipSize, uint32 nRenderStateSetNotAllowed ) = 0;

	// Requests the textures of every material through IRenderDevice::RequestTextureMipLevels(), with the largest value given for each material
	virtual void RequestTextureMipLevels( int nCount, IMaterial2 *const *ppMaterials, const uint16 *pValues ) = 0;

	// Loaded material whose unique id (CMaterial2::m_nUniqueId) is nUniqueId
	virtual HMaterialWeak FindMaterialByUniqueId( uint32 nUniqueId ) = 0;

	// "pc" or "vulkan" depending on the render device
	virtual void GetShaderPlatformName( CUtlString &sName ) = 0;

	// Index into the shader platforms "PC_SM_5_0", "PC_SM_5_0_VULKAN" and "PC_SM_6_0_VULKAN":
	// 2 when rendering through Vulkan, 0 otherwise
	virtual int GetShaderPlatform() = 0;

	// Returns the previous value
	virtual bool SetCompileMissingShaderCombos( bool bCompile ) = 0;

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

	// Where a shader file was found in the GAME search path
	struct ShaderFileLocation_t
	{
		// The VPK the file is in, or the mod directory it was found under
		CBufferStringN< 150 > m_Path;

		// The mod directory, set when the file is not in a VPK
		CBufferStringN< 32 > m_ModDirectory;

		// The file is opened as "<m_Path>:<name>" instead of "<m_Path>\<name>"
		bool m_bInVPK;
	};

	struct SpecializationConstant_t
	{
		uint32 m_nId;

		// When set, the render device looks m_nId up as a key of the shader's specialization constant table;
		// the producer leaves it false, so m_nId is the constant id
		bool m_bIdIsKey;

		uint32 m_nValue;
	};

	// A static combo of a shader program that computing the renderable passes of a mode found missing
	struct ShaderCompileRequest_t
	{
		// "(unnamed)" when the mode has no material
		const char *m_pMaterialName;

		// The program of the requested shader type in the shader of the mode
		void *m_pShaderProgram;

		// Set by the compile, NULL when it failed
		void *m_pCompileResult;

		// 7 with "-shaderdebugging", 2 when "-shaderopt" is also given
		int m_nShaderDebugLevel;

		int m_nStaticComboId;
		uint64 m_nDynamicComboId;

		// mat_shader_cache, unless the shaders are force reloaded
		bool m_bUseShaderCache;

		// Requests with it set are skipped by the compile
		bool m_bSkipCompile;

		// The shader's specialization constants resolved for the material
		SpecializationConstant_t m_SpecializationConstants[ 16 ];
		int m_nSpecializationConstantCount;
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

	// Keyed by the shader file name
	CUtlHashtable< CUtlString, ShaderFileLocation_t > m_ShaderFileLocations;
	CAtomicMutex m_ShaderCompileQueueMutex;

	// Compiled by the dynamic shader compile
	CUtlVector< ShaderCompileRequest_t > m_ShaderCompileQueue;

	// Substrings of the shader names that dynamic shader compile reloads
	CUtlVector< CUtlString > m_DynamicShaderCompileFilters;

	// Held while the queued shaders are compiled
	CAtomicMutex m_ShaderCompileMutex;

	// "MaterialSystem2/RenderModes" of gameinfo, "Default" when it has none
	CUtlVector< ModeInfo_t > m_modeInfo;

	// From the mode token to its index in m_modeInfo
	CUtlAttributeList< int, 16 > m_modeAttributeList;

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

	// mat_forcereloadshaders, the shaders are compiled instead of loaded from the .vcs files until the reload ends
	bool m_bForceReloadShaders : 1;

	// "-shaderdebugging"
	bool m_bShaderDebugging : 1;

	// "-shaderopt", lowers the debug level of "-shaderdebugging"
	bool m_bShaderOpt : 1;

	// "-batchwarnings" and "-batcherrors"
	bool m_bBatchWarnings : 1;
	bool m_bBatchErrors : 1;

	// EnableExtendedMaterialInfo() and EnableHeadlessMode()
	bool m_bLoadExtendedMaterialInfo : 1;
	bool m_bHeadLessModeEnabled : 1;

	// "-shaderstats"
	bool m_bShaderStats : 1;

	// "-shaders_force_recompile", the .vcs files are not loaded
	bool m_bShadersForceRecompile : 1;

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

	// From the "materials" of the "-batcherrors" file: the resource id of a material to the mask of the indices
	// in m_modeInfo of the modes it is required to be batchable in
	CUtlHashtable< uint64, uint64 > m_BatchErrorModes;

	// AddShaderReloadListener() and RemoveShaderReloadListener()
	CUtlVector< IShaderReloadListener * > m_ShaderReloadListeners;

	// AddMaterialParamChangedListener() and RemoveMaterialParamChangedListener()
	CUtlVector< void * > m_MaterialParamChangedListeners;

	// "DefaultTextureBuffer", "DefaultUAVBuffer" and "DefaultUAV.vtex"
	RenderBufferHandle_t m_hDefaultTextureBuffer;
	RenderBufferHandle_t m_hDefaultUAVBuffer;
	CStrongHandleBase m_hDefaultUAVTexture;

	// SetCompileMissingShaderCombos(), computing the renderable passes of a mode then compiles its missing static
	// combos through the dynamic shader compile
	bool m_bCompileMissingShaderCombos;
};

#endif // IMATERIALSYSTEM2_H
