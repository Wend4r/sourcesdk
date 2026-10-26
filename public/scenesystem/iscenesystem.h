//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Scene system interface
//
//===========================================================================//

#ifndef ISCENESYSTEM_H
#define ISCENESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <materialsystem2/imaterial2.h>
#include <mathlib/vector.h>
#include <mathlib/fltx4.h>
#include <resourcesystem/resourcehandle.h>
#include <resourcesystem/resourcehandletypes.h>
#include <tier0/utlstringtoken.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <bitmap/imageformat_declarations.h>
#include <scenesystem/scenesystem_flags.h>

#include <functional>

class CSceneObject;
class CSceneLightObject;
class CSceneBarnLightObject;
class CSceneLightProbeVolumeObject;
class CEnvMapSceneObject;
class CSceneDebugTextObject;
class CSceneAnimatableObject;
class CTextLayoutInfo;
class Vector2D;
class ISceneObjectDesc;
class ISceneWorld;
class ISceneView;
class IRenderDevice;
class IRenderContext;
class ISceneLayer;
class CBufferString;
class IRayTraceSceneWorld;
class ISceneViewDebugOverlays;
class ISparseShadowTree;
class ISceneLightListener;
class ISceneObjectListener;
class CViewId;
class CFrustum;
struct AABB_t;
struct OBB_t;
class FourVectors;
class VMatrix;
struct RenderViewport_t;
struct LightDesc_t;
struct MeshBuilderBuffers_t;
struct SceneObjectBuffers_t;
class CRenderAttributes;
struct SceneSystemPerFrameStats_t;
struct RenderInputLayoutField_t;
struct PipelineRenderTargetInfo_t;
enum RenderMultisampleType_t : uint8;
enum RenderPrimitiveType_t : int32;
enum InputLayoutVariation_t : uint8;

FORWARD_DECLARE_HANDLE( SwapChainHandle_t );
FORWARD_DECLARE_HANDLE( VertexBufferHandle_t );
FORWARD_DECLARE_HANDLE( IndexBufferHandle_t );
FORWARD_DECLARE_HANDLE( RenderBufferHandle_t );

class CTextureBase;

class InfoForResourceTypeCTextureBase
{
public:
	using RuntimeClass_t = CTextureBase;
};

using HRenderTexture = CWeakHandle< InfoForResourceTypeCTextureBase >;

//-----------------------------------------------------------------------------
// Resources the scene system creates at init, by ISceneSystem::GetWellKnown*
// index. The names follow the debug names the resources are created with.
//-----------------------------------------------------------------------------
enum WellKnownVertexBuffer_t
{
	WELL_KNOWN_VERTEX_BUFFER_QUAD = 0,					// Four positions from -1 to 1
	WELL_KNOWN_VERTEX_BUFFER_64X64_TRI_STRIPPED = 2,
	WELL_KNOWN_VERTEX_BUFFER_128X128_TRI_STRIPPED,
	WELL_KNOWN_VERTEX_BUFFER_256X256_TRI_STRIPPED,
	WELL_KNOWN_VERTEX_BUFFER_INSTANCE_ID,				// "instanceidbuffer": uint32 instance indices
	WELL_KNOWN_VERTEX_BUFFER_INSTANCE_ID_FLOAT,			// "instanceidbuffer2": float instance indices
	WELL_KNOWN_VERTEX_BUFFER_12X12_TRI_STRIPPED,
};

enum WellKnownIndexBuffer_t
{
	WELL_KNOWN_INDEX_BUFFER_QUAD = 0,					// "scene_quadindexbuffer": two triangles
	WELL_KNOWN_INDEX_BUFFER_64X64_TRI_LIST,				// "tesidx": 63 x 63 quads
	WELL_KNOWN_INDEX_BUFFER_64X64_TRI_STRIPPED,			// "nxnstripped"
	WELL_KNOWN_INDEX_BUFFER_128X128_TRI_STRIPPED,
	WELL_KNOWN_INDEX_BUFFER_256X256_TRI_STRIPPED,
	WELL_KNOWN_INDEX_BUFFER_12X12_TRI_LIST,				// "tesidx": 11 x 11 quads
	WELL_KNOWN_INDEX_BUFFER_MANY_QUADS,					// "manyquads": 16384 quads
};

enum WellKnownInputLayout_t
{
	WELL_KNOWN_INPUT_LAYOUT_UV_TEST = 0,
	WELL_KNOWN_INPUT_LAYOUT_COLOR_UV_TEST,
	WELL_KNOWN_INPUT_LAYOUT_WIREFRAME_3D,
	WELL_KNOWN_INPUT_LAYOUT_POSITION,
	WELL_KNOWN_INPUT_LAYOUT_POSITION_DUMMY_VB1,
	WELL_KNOWN_INPUT_LAYOUT_POST_PROCESS,
	WELL_KNOWN_INPUT_LAYOUT_UV_TANGENT,
	WELL_KNOWN_INPUT_LAYOUT_POS_2D_TEX_2D,
	WELL_KNOWN_INPUT_LAYOUT_UNUSED,
	WELL_KNOWN_INPUT_LAYOUT_CMT_MORPH_COMPOSITING,
	WELL_KNOWN_INPUT_LAYOUT_WIREFRAME_2D,
	WELL_KNOWN_INPUT_LAYOUT_POS_COLOR_UV_NORMAL_TANGENT,
	WELL_KNOWN_INPUT_LAYOUT_POSITION_2D,
	WELL_KNOWN_INPUT_LAYOUT_COLOR_POSITION,
	WELL_KNOWN_INPUT_LAYOUT_QUAD,
};

enum WellKnownBuffer_t
{
	WELL_KNOWN_BUFFER_SCRAMBLED_HALTON_PERMUTATIONS = 0,
	WELL_KNOWN_BUFFER_MESHLETS,							// "meshlet buffer" of AllocateFrameScopedMeshlets
};

enum WellKnownTexture_t
{
	WELL_KNOWN_TEXTURE_WHITE = 0,
	WELL_KNOWN_TEXTURE_BLACK,
	WELL_KNOWN_TEXTURE_TRANSPARENT_BLACK,
	WELL_KNOWN_TEXTURE_TRANSPARENT_BLACK_3D,
	WELL_KNOWN_TEXTURE_WHITE_ARRAY,
	WELL_KNOWN_TEXTURE_CHECKER,
	WELL_KNOWN_TEXTURE_BLACK_3D,
	WELL_KNOWN_TEXTURE_BLACK_CUBE,
	WELL_KNOWN_TEXTURE_BLACK_CUBE_ARRAY,
	WELL_KNOWN_TEXTURE_TRANSPARENT_BLACK_ARRAY,
	WELL_KNOWN_TEXTURE_OPAQUE_BLACK_3D,
	WELL_KNOWN_TEXTURE_ONE_16BIT,
	WELL_KNOWN_TEXTURE_ONE_16BIT_ARRAY,
	WELL_KNOWN_TEXTURE_WHITE_3D,
	WELL_KNOWN_TEXTURE_ZERO_UINT,
	WELL_KNOWN_TEXTURE_DEPTH_D32_WHITE,
	WELL_KNOWN_TEXTURE_DEPTH_D32_BLACK,
	WELL_KNOWN_TEXTURE_RTX_RANDOM_SAMPLES,				// Created when ray tracing is supported
	WELL_KNOWN_TEXTURE_BRDF_LUT,
	WELL_KNOWN_TEXTURE_BRDF_MULTISCATTER_LUT,
	WELL_KNOWN_TEXTURE_GREY,
	WELL_KNOWN_TEXTURE_GREY_3D,
};

enum WellKnownMaterial_t
{
	WELL_KNOWN_MATERIAL_COPY_TO_BACKBUFFER = 0,
	WELL_KNOWN_MATERIAL_DOWNSAMPLE,
	WELL_KNOWN_MATERIAL_DEBUG_SOLID,
	WELL_KNOWN_MATERIAL_DEBUG_WIREFRAME,
	WELL_KNOWN_MATERIAL_DEBUG_WIREFRAME_2D,
	WELL_KNOWN_MATERIAL_DEBUG_SHOW_TEXTURE,
	WELL_KNOWN_MATERIAL_DEBUG_UI,
	WELL_KNOWN_MATERIAL_MORPH_COMPOSITE,
	WELL_KNOWN_MATERIAL_OCCLUSION_PROXY,
	WELL_KNOWN_MATERIAL_OCCLUSION_PROXY_COUNT_DRAW,
	WELL_KNOWN_MATERIAL_OCCLUSION_PROXY_DEBUG_DRAW,

	// Two separately loaded instances of each of the next two materials.
	WELL_KNOWN_MATERIAL_PRIMARY_WHITE,
	WELL_KNOWN_MATERIAL_PRIMARY_WHITE_2,
	WELL_KNOWN_MATERIAL_PRIMARY_WHITE_TRANS,
	WELL_KNOWN_MATERIAL_PRIMARY_WHITE_TRANS_2,

	WELL_KNOWN_MATERIAL_REFLECTIVITY_90,
	WELL_KNOWN_MATERIAL_GENERAL_FILTER,
	WELL_KNOWN_MATERIAL_LUMINAIRE,
	WELL_KNOWN_MATERIAL_RTX_BIN_LIGHTS,
	WELL_KNOWN_MATERIAL_RTX_HITGROUP_DEFAULT_OPAQUE,
	WELL_KNOWN_MATERIAL_RTX_HITGROUP_DEFAULT_TRANSPARENT,
	WELL_KNOWN_MATERIAL_RTX_GENERATE_SCRAMBLED_HALTON_SAMPLES,
	WELL_KNOWN_MATERIAL_CS_PARALLEL_SORT,
	WELL_KNOWN_MATERIAL_CS_PARALLEL_SORT_ONESWEEP,
};

enum WellKnownShape_t
{
	WELL_KNOWN_SHAPE_DODECAHEDRON = 0,
	WELL_KNOWN_SHAPE_CONE,
	WELL_KNOWN_SHAPE_BOX,
	WELL_KNOWN_SHAPE_PYRAMID,
};

// Format of a scratch render target
enum WellKnownRenderTarget_t
{
	WELL_KNOWN_RENDER_TARGET_RGBA8888 = 0,				// "scratch_rt_result"
	WELL_KNOWN_RENDER_TARGET_R11G11B10F,
	WELL_KNOWN_RENDER_TARGET_DEPTH,						// "rendertarget_dscratch"
	WELL_KNOWN_RENDER_TARGET_RGBA16161616F_MIPS,

	// The format SceneSystem/FrameBufferCopyFormat picks ("8888" or "R11G11B10F").
	WELL_KNOWN_RENDER_TARGET_FRAMEBUFFER_COPY_FORMAT = 5,
};

// Size class of a scratch render target
enum WellKnownRenderTargetSize_t
{
	WELL_KNOWN_RENDER_TARGET_SIZE_128 = 0,
	WELL_KNOWN_RENDER_TARGET_SIZE_512,

	// The size SetMinimumFrameBufferRenderTargetSize sets.
	WELL_KNOWN_RENDER_TARGET_SIZE_FRAMEBUFFER,

	// Returned by BestRenderTargetSize when no size class fits.
	WELL_KNOWN_RENDER_TARGET_SIZE_NONE = 4,
};

// Texture sets of ISceneSystem::AddTextureToSet
enum SceneTextureSet_t
{
	SCENE_TEXTURE_SET_LIGHT_COOKIES = 0,				// Loaded from SceneSystem/WellKnownLightCookies
};

enum SceneSystemBoxCullMode_t : uint32
{
	SCENESYSTEM_BOX_CULL_INSIDE = 0,
	SCENESYSTEM_BOX_CULL_OUTSIDE = 1,
};

enum SceneSystemGPUProfilerMode_t : uint32
{
	SCENE_GPU_PROFILER_DISABLE = 0,
	SCENE_GPU_PROFILER_TIMESTAMP_ONLY = 1,
	SCENE_GPU_PROFILER_FULL = 2,
};

struct SceneSystemTimestampFrameData_t
{
	CUtlString m_sGroupName;
	CUtlStringToken m_nGroupNameToken;
	float m_flDurationInMilliseconds;
};

enum SetupPerObjectLighting_t : uint32
{
	SETUP_PER_OBJECT_LIGHTING_DIFFUSE = 0x1,
	SETUP_PER_OBJECT_LIGHTING_SPECULAR = 0x2,
};

abstract_class ISceneSystem : public IAppSystem
{
public:
	virtual CSceneObject *CreateSceneObject( ISceneObjectDesc *pDesc, ESceneObjectTypeFlags nTypeFlags, ESceneObjectFlags nFlags, ISceneWorld *pWorld ) = 0;

	// The renderable flags come from the material (opaque, translucent or opaque with shadows).
	virtual CSceneObject *CreateSceneObjectFromBuffers( ISceneWorld *pWorld, VertexBufferHandle_t hVertexBuffer, IndexBufferHandle_t hIndexBuffer, HMaterialWeak hMaterial, int nVertices, int nIndices, const Vector &vecBoundsMin, const Vector &vecBoundsMax, RenderPrimitiveType_t nPrimitiveType, const char *pszDescName, bool bOwnVBAndIB, ESceneObjectTypeFlags nTypeFlags ) = 0;

	virtual CSceneObject *CreateSceneObjectFromBuffers( ESceneObjectTypeFlags nTypeFlags, ESceneObjectFlags nFlags, ISceneWorld *pWorld, VertexBufferHandle_t hVertexBuffer, IndexBufferHandle_t hIndexBuffer, HMaterialWeak hMaterial, int nVertices, int nIndices, const Vector &vecBoundsMin, const Vector &vecBoundsMax, RenderPrimitiveType_t nPrimitiveType, int16 nDrawPrimitiveFlags, const char *pszDescName, bool bOwnVBAndIB ) = 0;

	// Creates a CMeshBuilderSceneObjectDesc object.
	virtual CSceneObject *CreateSceneObjectFromStaticBuffers( ESceneObjectTypeFlags nTypeFlags, ESceneObjectFlags nFlags, ISceneWorld *pWorld, const MeshBuilderBuffers_t &buffers, HMaterialWeak hMaterial, bool bDeleteAtFrameEnd ) = 0;

	virtual CSceneAnimatableObject *CreateAnimatableSceneObjectFromBuffers( ISceneWorld *pWorld, HMaterialWeak hMaterial, SceneObjectBuffers_t &data, ESceneObjectTypeFlags nTypeFlags, ESceneObjectFlags nFlags, bool bOwnVBAndIB ) = 0;

	virtual void DeleteSceneObject( CSceneObject *pObject ) = 0;

	virtual ISceneObjectDesc *FindRenderableType( const char *pszName ) = 0;

	virtual void AddRenderableType( const char *pszName, ISceneObjectDesc *pDesc ) = 0;
	virtual void RemoveRenderableType( const char *pszName ) = 0;
	virtual void BeginRenderingViews( IRenderDevice *pRenderDevice, bool bUpdateStats ) = 0;
	virtual ISceneView *AddView( const char *pszViewName, const CViewId &viewId, const CFrustum &frustum, SwapChainHandle_t hSwapChain, ISceneWorld *pWorld, const RenderViewport_t &viewport, const uint32 *pVis, int nPriority ) = 0;
	virtual ISceneView *AddDynamicView( const char *pszViewName, const ISceneView *pParentView, const CFrustum &frustum, SwapChainHandle_t hSwapChain, ISceneWorld *pWorld, const RenderViewport_t &viewport, const uint32 *pVis, int nPriority, int nViewProducerIndex ) = 0;
	virtual void BeginRenderingDynamicView( ISceneView *pView ) = 0;
	virtual IRenderContext *RenderEmbeddedView( ISceneView *pView, ISceneLayer *pLayer ) = 0;
	virtual void FinishRenderingViews() = 0;
	virtual bool WaitForRenderingToComplete() = 0;
	virtual bool IsRenderingBusy() = 0;
	virtual void SetSceneObjectBounds( CSceneObject *pObject, const Vector &vecMins, const Vector &vecMaxs ) = 0;
	virtual void SetObjectBoundsInfinite( CSceneObject *pObject ) = 0;

	// Returns FLT_MAX extents when the object has no bounds yet.
	virtual void GetSceneObjectBounds( CSceneObject *pObject, Vector *pMins, Vector *pMaxs ) = 0;

	// Returns the shadow LOD index for the view (convar override, then the view, then the object) and writes 1 << index to pLODMask.
	virtual int GetShadowLODIndex( CSceneObject *pObject, ISceneView *pView, uint8 *pLODMask ) = 0;

	virtual void SetPVSFromClusterMembership( CSceneObject *pObject, const uint32 *pClusters, int nClusterCount ) = 0;

	// Recompute the PVS data of the object from bounds or an oriented box in its world, unless PVS_NO_RECOMPUTE is set on it.
	// Overloads: MSVC places the oriented box one first.
	virtual void RecomputePVSFromBounds( CSceneObject *pObject, const AABB_t &bounds, bool bIsStatic ) = 0;
	virtual void RecomputePVSFromBounds( CSceneObject *pObject, const OBB_t &bounds, bool bIsStatic ) = 0;

	// Makes the object visible from every cluster of its world.
	virtual void ForceVisibleEverywhere( CSceneObject *pObject ) = 0;

	virtual CSceneDebugTextObject *CreateTextObject( Vector2D *pEndPos, ISceneWorld *pWorld, const CTextLayoutInfo *pTextInfo, bool bPositionIn3DWorldSpace, const Vector &vLocal3DPosition ) = 0;

	// Overloads: MSVC places the barn light one first.
	virtual CSceneLightObject *CreateLight( const LightDesc_t &desc, ISceneWorld *pWorld ) = 0;
	virtual CSceneBarnLightObject *CreateLight( const void *pBarnLightDesc, ISceneWorld *pWorld ) = 0;

	virtual CSceneObject *CreateSkyBox( HMaterialWeak hMaterial, ISceneWorld *pWorld ) = 0;
	virtual CSceneLightProbeVolumeObject *CreateLightProbeVolume( ISceneWorld *pWorld ) = 0;
	// Behavioural name: true when a light or barn light of one of the worlds of the view has the int render attribute token 0xEDC61ED4 set
	// (the light descriptors read it as the switch for the float attribute 0x266C41FB; the strings are in no shipped binary or shader).
	virtual bool HasFlaggedLightInView( ISceneView *pView ) = 0;
	virtual void MarkLightProbeVolumeObjectUpdated( CSceneLightProbeVolumeObject *pLightProbe ) = 0;
	virtual void MarkEnvironmentMapObjectUpdated( CEnvMapSceneObject *pEnvMap ) = 0;
	virtual ISceneWorld *CreateWorld( const char *pszDebugName ) = 0;
	virtual void DestroyWorld( ISceneWorld *pWorld ) = 0;
	virtual VertexBufferHandle_t GetWellKnownVertexBuffer( WellKnownVertexBuffer_t nBuffer ) = 0;
	virtual IndexBufferHandle_t GetWellKnownIndexBuffer( WellKnownIndexBuffer_t nBuffer ) = 0;
	virtual void *GetWellKnownInputLayout( WellKnownInputLayout_t nLayout, InputLayoutVariation_t nVariation ) = 0;
	virtual RenderInputLayoutField_t *GetWellKnownInputLayoutFields( WellKnownInputLayout_t nLayout, int *pFieldCount ) = 0;
	virtual RenderBufferHandle_t GetWellKnownBuffer( WellKnownBuffer_t nBuffer ) = 0;
	virtual HRenderTexture GetWellKnownTexture( WellKnownTexture_t nTexture ) = 0;
	virtual IMaterial2 *GetWellKnownMaterial( WellKnownMaterial_t nMaterial ) = 0;
	virtual HMaterialWeak GetWellKnownMaterialHandle( WellKnownMaterial_t nMaterial ) = 0;
	virtual void *GetWellKnownShape( WellKnownShape_t nShape ) = 0;
	virtual HRenderTexture GetWellKnownRenderTarget( WellKnownRenderTarget_t nRenderTarget, WellKnownRenderTargetSize_t nSize ) = 0;
	// Takes the smallest size class that holds nWidth x nHeight.
	virtual HRenderTexture GetWellKnownRenderTarget( WellKnownRenderTarget_t nRenderTarget, int nWidth, int nHeight ) = 0;
	virtual void GetRenderTargetSize( WellKnownRenderTargetSize_t nSize, int *pWidth, int *pHeight ) = 0;
	virtual WellKnownRenderTargetSize_t BestRenderTargetSize( int nWidth, int nHeight ) = 0;
	virtual void SetMinimumFrameBufferRenderTargetSize( int nWidth, int nHeight ) = 0;
	// Texture sets, guarded by one global mutex.
	virtual bool AddTextureToSet( HRenderTexture hTexture, SceneTextureSet_t nSet ) = 0;
	virtual void RemoveTextureFromSet( HRenderTexture hTexture, SceneTextureSet_t nSet ) = 0;
	virtual HRenderTexture GetTextureInSet( int nIndex, SceneTextureSet_t nSet ) = 0;
	virtual int FindTextureInSet( HRenderTexture hTexture, SceneTextureSet_t nSet ) = 0;
	// Finds a texture by the token of its key in the game info section the set was loaded from ("SceneSystem/WellKnownLightCookies").
	virtual int FindTextureInSetByName( CUtlStringToken name, SceneTextureSet_t nSet ) = 0;
	// The array texture the textures of the set are copied into, and its depth.
	virtual HRenderTexture GetTextureSetArrayTexture( SceneTextureSet_t nSet ) = 0;
	virtual uint16 GetTextureSetArrayDepth( SceneTextureSet_t nSet ) = 0;

	virtual ISceneViewDebugOverlays *CreateSceneViewDebugOverlayList( int nTimingType, const char *pszName ) = 0;
	virtual void DestroySceneViewDebugOverlayList( ISceneViewDebugOverlays *pOverlays ) = 0;

	// Overlay lists without a "SceneViewDebugOverlays World", kept in a second list that no view renders.
	virtual ISceneViewDebugOverlays *CreateWorldlessSceneViewDebugOverlayList() = 0;
	virtual void DestroyWorldlessSceneViewDebugOverlayList( ISceneViewDebugOverlays *pOverlays ) = 0;

	// Behavioural name: CDebugOverlayGameSystem::OnClientPollInput passes whether its int at +104 equals 2 on every input poll;
	// the value goes to a global that nothing in libscenesystem reads.
	virtual void SetDebugOverlayPollState( bool bState ) = 0;

	virtual void InitObject( CSceneObject *pObject, ESceneObjectTypeFlags nTypeFlags, ESceneObjectFlags nFlags, ISceneWorld *pWorld ) = 0;

	virtual void MoveObjectToWorld( CSceneObject *pObject, ISceneWorld *pWorld ) = 0;

	// bAdvanceGPUProfilerFrame reads back the GPU timestamp queries and moves to the next profiler frame while a profiler mode is set
	virtual void FrameUpdate( bool bAdvanceGPUProfilerFrame ) = 0;
	virtual void AllocateScratchRenderTargets( void *p ) = 0;

	// bUseLargestMonitorSize takes the size from IRenderDevice::GetLargestMonitorDimensions instead of the swap chain.
	virtual void ComputePipelineRenderTargetInfo( SwapChainHandle_t hSwapChain, bool bUseLargestMonitorSize, PipelineRenderTargetInfo_t *pInfo ) = 0;

	virtual void ComputePipelineRenderTargetInfo( int nWidth, int nHeight, RenderMultisampleType_t nMultisampleType, bool bUseLargestMonitorSize, PipelineRenderTargetInfo_t *pInfo ) = 0;
	virtual void SetMainSwapChain( SwapChainHandle_t hSwapChain ) = 0;
	virtual void SetCanAllocateScratchRenderTargetsOnDemand( bool bCanAllocate ) = 0;
	virtual void *GetSceneSystemStats() = 0;
	virtual void *GetSceneSystemStatsMutex() = 0;
	virtual void GetStatsSummaryString( CBufferString *pOut ) = 0;
	virtual void PrintPerFrameStats( CBufferString *pOut, const char *pszHeader ) = 0;
	virtual SceneSystemPerFrameStats_t &GetPerFrameStats() = 0;
	virtual uint32 GetFinishRenderingViewsCounter() const = 0;
	virtual uint32 GetFrameUpdateCounter() const = 0;
	// bSingleInstanceID binds the instance ID buffer with stride 0, so every instance reads the ID at nTransformSlotIndex.
	virtual void BindTransformTextureAndInstanceIDBuffer( IRenderContext *pRenderContext, int nVBSlot, int nTransformSlotIndex, bool bSingleInstanceID ) = 0;
	// Binds the transform buffer at offset 0.
	virtual void BindTransformBuffer( IRenderContext *pRenderContext, int nVBSlot ) = 0;

	virtual void AllocateFrameScopedTransforms( int nCount, int *pFirstIndex, void **ppTransforms ) = 0;

	// Also allocates nInstanceCount 32 byte instance records that point at the transforms (one each when the counts match, else all at
	// the first); pFirstIndex receives the index of the first instance record.
	virtual void AllocateFrameScopedTransforms( int nInstanceCount, int nCount, int *pFirstIndex, void **ppTransforms ) = 0;
	virtual void *AllocateFrameScopedMeshlets( int nCount, int *pFirstIndex ) = 0;
	virtual void *AllocateFrameScopedParticleData( int nCount, int *pFirstIndex ) = 0;
	// Returns the index of a 32 byte vertex layout key built from the input layout fields of a mesh, adding it when it is missing;
	// the table is uploaded to a GPU buffer when it changes.
	virtual int FindOrAddVertexLayoutIndex( const uint32 *pVertexLayoutKey ) = 0;
	virtual ISceneView *CreateTemplateView( const char *pszName, ISceneWorld *pWorld, const uint32 *pVis ) = 0;
	virtual CViewId CreateViewId( int nPlayer ) = 0;
	virtual void DeleteSceneObjectAtFrameEnd( CSceneObject *pObject ) = 0;
	virtual void GetAllActiveSceneWorlds( CUtlVector< ISceneWorld * > &worlds ) = 0;
	virtual uint8 FindOrCreateSceneObjectClass( const char *pszName ) = 0;
	virtual void SetObjectClass( CSceneObject *pObject, uint8 nClass ) = 0;
	virtual void GetObjectClassName( uint8 nClass, char *pszBuffer, int nBufferSize ) = 0;
	virtual void ConstructSceneObject( CSceneObject *pObject ) = 0;

	// Called from the CSceneObject destructor.
	virtual void DestructSceneObject( CSceneObject *pObject ) = 0;

	virtual void SetSceneObjectTransformsFast( CSceneObject **pObjectList, int nNumObjects, const FourVectors *pXYZOrigins, const fltx4 *pRadii, const FourVectors *pTints, const fltx4 *pRotations, const fltx4 *pYaws, const fltx4 *pPitches, float flRadiusScale, const fltx4 *pXAxisScale, const fltx4 *pYAxisScale, const fltx4 *pZAxisScale ) = 0;

	virtual ISceneView *AddViewFromTemplate( const char *pszViewTemplateName, const char *pszViewName, const ISceneView *pParentView, const CFrustum &frustum, SwapChainHandle_t hSwapChain, ISceneWorld *pWorld, const RenderViewport_t &viewport, const uint32 *pVis, const CRenderAttributes *pAttributes ) = 0;

	virtual void InstallTemplateViewFactory( void *p ) = 0;
	virtual void RemoveTemplateViewFactory( const char *pszName ) = 0;
	virtual void QueueSceneObjectUpdateJob( std::function< void() > &&job ) = 0;
	virtual void ExecuteQueuedSceneObjectUpdateJobs() = 0;
	virtual void InstallMaterialDependentSceneObjectUpdater( const std::function< void( CSceneObject * ) > &updater ) = 0;
	// Add and remove a listener of the global per-frame descriptor set: its first virtual runs once the set exists (at once when it
	// already does), its second after each update of the set's bindings.
	virtual void AddGlobalDescriptorSetListener( void *pListener ) = 0;
	virtual void RemoveGlobalDescriptorSetListener( void *pListener ) = 0;
	virtual void SetIgnoreAlphaFade( bool bIgnoreAlphaFade ) = 0;
	virtual void SetupPerObjectLighting( CRenderAttributes &renderAttributes, CSceneObject *pSceneObject, const ISceneLayer *pSceneLayerInterface, SetupPerObjectLighting_t flags ) = 0;

	// Destroys every sparse shadow tree, then loads them again when nCount is positive.
	virtual void ReloadSparseShadowTrees( int nCount ) = 0;

	// Destroys every sparse shadow tree.
	virtual void DestroyAllSparseShadowTrees() = 0;

	virtual void LoadSparseShadowTreeManifest() = 0;
	virtual void UnloadSparseShadowTreeManifest() = 0;
	virtual ISparseShadowTree *GetSparseShadowTree( int nIndex, bool bOnlyIfEnabled ) = 0;
	// pBuildCallbacks is kept by the tree and its CBuildSparseShadowTree helper; the tree calls its slot 1 when a build starts and polls its
	// slot 0 until it returns false.
	virtual void CreateSparseShadowTree( void *pBuildCallbacks, int nIndex ) = 0;
	virtual void DestroySparseShadowTree( int nIndex ) = 0;

	// Finds the sky box object named by a string attribute of pObject among the sky box objects of the worlds of the view.
	virtual CSceneObject *FindSkyBoxSceneObject( ISceneView *pView, CSceneObject *pObject ) = 0;

	// Sets the per class settings byte that GetObjectSettings returns.
	virtual void SetObjectClassSettings( uint8 nClass, uint8 nSettings ) = 0;

	virtual void SceneObject_ScheduleDirtyUpdate( CSceneObject *pObject ) = 0;
	virtual void SceneObject_UnscheduleDirtyUpdate( CSceneObject *pObject ) = 0;
	virtual void SceneObject_AttachExtraData( CSceneObject *pObject ) = 0;
	virtual void SceneObject_AttachLightingCache( CSceneObject *pObject ) = 0;
	virtual void SceneObject_UpdateFlagsBasedOnMaterial( CSceneObject *pObject ) = 0;
	// Stores a 76 byte stencil description in the extra data of the object (interned by hash): two lists of eight stencil bit name
	// tokens, then the stencil operations and an optional reference value.
	virtual void SceneObject_SetStencilState( CSceneObject *pObject, const void *pStencilDesc ) = 0;
	// Expands that description into the 8 byte render stencil state, allocating the named stencil bits in the view; returns pStencilState,
	// or NULL when the object has none.
	virtual void *SceneObject_GetStencilState( CSceneObject *pObject, void *pStencilState, ISceneView *pView ) = 0;

	// Holds a strong reference to the material; the s&box declaration adds a layer attribute token and value.
	virtual void SceneObject_SetMaterialOverride( CSceneObject *pObj, HMaterialWeak hOverride ) = 0;

	// nPlayerIndex -1 applies to every player.
	// bUnused is not read by the implementation.
	virtual void SceneObject_SetVisibilityToPlayerIndex( CSceneObject *pObject, int nPlayerIndex, bool bUnused, bool bVisible ) = 0;

	virtual void SceneObject_SetSplitScreenMask( CSceneObject *pObject, uint32 nMask ) = 0;
	virtual void SceneObject_AddChild( CSceneObject *pParent, CUtlStringToken token, CSceneObject *pChild, uint32 nFlags ) = 0;
	virtual void SceneObject_RemoveChild( CSceneObject *pParent, CSceneObject *pChild ) = 0;
	virtual CSceneObject *SceneObject_FindChild( const CSceneObject *pParent, CUtlStringToken token ) = 0;
	virtual void SceneObject_MirrorTransformToChildSceneObjects( const CSceneObject *pObject ) = 0;
	virtual void SceneObject_MirrorVisibilityToChildSceneObjects( const CSceneObject *pObject ) = 0;
	virtual void SceneObject_SetForceLayerID( CSceneObject *pObject, CUtlStringToken layerId ) = 0;
	virtual void SceneObject_PostFlagsUpdate( CSceneObject *pObject ) = 0;
	virtual int RegisterExtraDataAllocation( const char *pszName ) = 0;
	virtual float GetCurrentRenderTime() = 0;
	virtual void AdvanceNonRenderedFrame() = 0;
	virtual uint8 GetObjectSettings( const CSceneObject *pObject ) const = 0;
	// Matrices of a view frustum, from one of two matrix sets chosen by a scene system flag. Unless sc_force_translation_in_projection
	// is set, the view projection matrix takes the translation of the view matrix and pCameraOffset receives the negated camera position.
	virtual VMatrix GetViewProjectionMatrix( const CFrustum *pFrustum, Vector *pCameraOffset ) = 0;
	virtual VMatrix GetViewMatrix( const CFrustum *pFrustum ) = 0;
	// Fills the per-view constants from the view and the previous view frustum (matrices through GetViewProjectionMatrix, viewport and
	// clip values); the parameters are not recovered.
	virtual void ComputeViewConstants( void *p ) = 0;

	// Gates ISceneUtils::CreateVolumetricFog.
	virtual bool VolumetricFogEnabled() = 0;

	// Gates ISceneUtils::CreateWindController.
	virtual bool IsWindEnabled() = 0;

	virtual bool NonTexturedGradientFogEnabled() = 0;
	virtual bool CubemapFogEnabled() = 0;

	// Gates ISceneUtils::CreateCharacterDecalRenderer.
	virtual bool IsCharacterDecalRenderingEnabled() = 0;

	// Add and remove a callback, called with pContext as its last argument after the render attributes of a view are set up.
	virtual void AddViewAttributesCallback( void *pfnCallback, void *pContext ) = 0;
	virtual void RemoveViewAttributesCallback( void *pfnCallback, void *pContext ) = 0;
	virtual ISceneView *FindView( const char *pszName ) = 0;
	virtual ImageFormat GetPrimaryRenderTargetFormat() = 0;
	virtual ImageFormat GetBackBufferFormat() = 0;
	virtual CSceneObject *CreateAOProxySceneObject( ESceneObjectTypeFlags nTypeFlags, ESceneObjectFlags nFlags, ISceneWorld *pWorld ) = 0;

	// Reads ao_proxy_capsule_list and ao_proxy_box_list.
	virtual void LoadAOProxies( void *p ) = 0;

	virtual void UpdateAOProxySceneObject( void *p ) = 0;
	virtual uint32 AddCullingBox( ISceneWorld *pWorld, SceneSystemBoxCullMode_t nCullMode, const Vector &vOrigin, const QAngle &vAngles, Vector vExtents ) = 0;
	virtual void RemoveCullingBox( ISceneWorld *pWorld, uint32 nBoxId ) = 0;

	// Draw flags are 16 bit here, int in the s&box declaration.
	virtual void SceneObject_GetMaterialsForObject( CSceneObject *pObject, CUtlVectorFixedGrowable< HMaterialWeak, 2 > &materials, CUtlVectorFixedGrowable< int16, 2 > &drawFlags ) = 0;

	// Add and remove a listener of the global light list, which the light descriptors notify when a light is destroyed.
	virtual void RegisterSceneLightListener( ISceneLightListener *pListener ) = 0;
	virtual void UnregisterSceneLightListener( ISceneLightListener *pListener ) = 0;

	// Add and remove a listener of the global scene object list, which is notified when a scene object is deleted.
	virtual void RegisterSceneObjectListener( ISceneObjectListener *pListener ) = 0;
	virtual void UnregisterSceneObjectListener( ISceneObjectListener *pListener ) = 0;

	// The mode is kept per requester; GetGPUProfilerMode returns the union of all requested modes.
	virtual void SetGPUProfilerMode( CUtlStringToken requester, SceneSystemGPUProfilerMode_t eGPUProfilerMode ) = 0;
	virtual SceneSystemGPUProfilerMode_t GetGPUProfilerMode() = 0;

	// A request lasts half a second; while any is active, the GPU culling counters are read back into the per frame stats.
	virtual void SetGPUCullingStatsEnabled( CUtlStringToken requester, bool bEnable ) = 0;
	virtual bool IsGPUCullingStatsEnabled() = 0;

	// Walks the timestamp tree down to nMaxDepth levels and sums the entries of the same name below it; 0 or less sums everything.
	virtual void GetMostRecentTimestampFrameSummary( SwapChainHandle_t hSwapChain, CUtlVector< SceneSystemTimestampFrameData_t > &timestampFrameSummaryOut, int nMaxDepth ) = 0;
	// ORs the ImGui state of the overlay lists created by CreateSceneViewDebugOverlayList: bit 0 when pMouseCursor is set
	// (-1 hides the cursor), bit 1 while they take mouse input; the client picks its cursor and consumes mouse events from it.
	virtual int GetImGuiInputState( int *pMouseCursor ) = 0;
	// bTLASFlag40 adds flag 0x40 to the "SceneTLAS" creation flags (the Vulkan device does not read it); an IRayTraceSceneWorld setter sets
	// the same flag later.
	virtual IRayTraceSceneWorld *CreateRayTraceWorld( const char *pszDebugName, int nMaxRayTypes, bool bTLASFlag40 ) = 0;
	virtual void DestroyRayTraceWorld( IRayTraceSceneWorld *pWorld ) = 0;
	// Queues a 192 byte BLAS geometry update keyed by a 64 bit id, merged with a pending update of the same id.
	virtual void QueueBLASUpdate( const void *pUpdate ) = 0;
	// Compacts the BLASes of the pending scene objects that IRenderDevice::IsReadyToCompact reports ready, and drops the handled objects.
	virtual void CompactPendingBLASes( IRenderContext *pRenderContext ) = 0;
	// Queues a 56 byte request for the "ProceduralGeometry Updates" pass.
	virtual void QueueProceduralGeometryUpdate( const void *pRequest ) = 0;
	// Distance field scene, keyed by model. Adding a model that is present adds a reference; each returns the model's version, -1 without
	// a distance field scene.
	virtual int AddModelToDistanceFieldScene( CWeakHandle< InfoForResourceTypeCModel > hModel ) = 0;
	virtual void ReleaseModelFromDistanceFieldScene( CWeakHandle< InfoForResourceTypeCModel > hModel ) = 0;
	// Rebuilds the entry of a model that is present.
	virtual void UpdateModelInDistanceFieldScene( CWeakHandle< InfoForResourceTypeCModel > hModel ) = 0;
	// Fields of a distance field instance (an index from GetModelDistanceFieldInstances) taken from the model's distance field data.
	// Behavioural name: the index that the model's token table (model +304) gives the instance's token, -1 when it has none.
	virtual int GetDistanceFieldInstanceTokenIndex( int nInstance ) = 0;
	// Behavioural name: the two ints at +16 and +20 of the model's instance data.
	virtual void GetDistanceFieldInstanceValues( int nInstance, int *pValue1, int *pValue2 ) = 0;
	// Behavioural names: flag bit 0 of the instance (instance data byte +37), and bits 0 or 1 (bytes +37 or +38).
	virtual bool IsDistanceFieldInstanceFlagged( int nInstance ) = 0;
	virtual bool HasDistanceFieldInstanceFlags( int nInstance ) = 0;
	// Copies the distance field instance indices of a model when its version differs from nKnownVersion; returns the version.
	virtual int GetModelDistanceFieldInstances( CWeakHandle< InfoForResourceTypeCModel > hModel, int nKnownVersion, CUtlVector< int > &instances ) = 0;
	// Replaces SCENEOBJECTTYPEFLAG_HAS_PVS_DATA with SCENEOBJECTTYPEFLAG_DISTANCE_FIELD_INSTANCE in nTypeFlags.
	virtual CSceneObject *CreateDistanceFieldInstanceSceneObject( ESceneObjectTypeFlags nTypeFlags, ESceneObjectFlags nFlags, ISceneWorld *pWorld ) = 0;
	// Returns the distance field scene.
	virtual void *GetDistanceFieldScene() = 0;

	// Resizes the hair simulation buffers.
	virtual void ResizeHairBuffers( int nCount ) = 0;
	// Render device memory pool for the "CMTatlastexturePooled_" morph target atlas and the dynamic BLAS "vertex cache" buffers;
	// ignored unless SceneSystem/ComputeShaderSkinning is set.
	virtual void SetResourceMemoryPool( void *pPool ) = 0;
	// The "vertex cache" buffers of the frame ("Resizing Vertex Cache Buffer").
	virtual void *GetVertexCacheBuffer( int nIndex ) = 0;
	virtual void AcquireImGuiLock() = 0;
	// bCompleteDeferredRendering finishes the rendering that FinishRenderingViews left pending because this thread held the lock
	virtual void ReleaseImGuiLock( bool bCompleteDeferredRendering ) = 0;
	virtual bool IsImGuiLocked() = 0;
	virtual bool IsImGuiLockedByCurrentThread() = 0;
	// The function runs in WaitForRenderingToComplete.
	virtual void QueueRenderingCompleteJob( std::function< void() > &&fn ) = 0;
	// Incremented by each AllocateScratchRenderTargets call.
	virtual int GetScratchRenderTargetAllocationCount() = 0;
	// Queues the view once; the next BeginRenderingViews renders its debug overlays ("Render3DDebugOverlays", "Render2DDebugOverlays")
	// unless the overlay list reports them handled.
	virtual void QueueViewDebugOverlayRendering( ISceneView *pView ) = 0;
};

#endif // ISCENESYSTEM_H
