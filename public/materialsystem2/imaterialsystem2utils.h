#ifndef IMATERIALSYSTEM2UTILS_H
#define IMATERIALSYSTEM2UTILS_H

#ifdef _WIN32
	#pragma once
#endif

#include "ifontmanager.h"
#include "imaterial2.h"
#include "itextlayout.h"

#include <appframework/iappsystem.h>
#include <bitmap/imageformat_declarations.h>
#include <mathlib/vector.h>
#include <mathlib/vector2d.h>
#include <resourcefile/resourcetype.h>

#include <color.h>
#include <tier0/bufferstring.h>
#include <tier0/platform.h>
#include <tier0/wchartypes.h>

class IRenderContext;
class IMaterialMode;
class CRenderAttributes;
class IQueuedRenderable;
class InfoForResourceTypeCTextureBase;
struct MaterialRenderablePass_t;
struct MeshBuilderBuffers_t;
struct VertexColor_t;
struct RsStencilStateOverride_t;
struct RsDepthBiasStateOverride_t;
FORWARD_DECLARE_HANDLE( RenderInputLayout_t );

enum EGPUTextureCompressOptions_t : uint32
{
	GPU_COMPRESS_PERCEPTUAL_ERROR_METRIC = 0x1,
	GPU_COMPRESS_ENDPOINT_OPTIMIZATION = 0x2,
};

abstract_class IMaterialSystem2Utils : public IAppSystem
{
public:
	// Used by the text layout whenever a caller passes font 0
	virtual HFont GetDefaultFont( bool bOutline ) = 0;

	virtual bool SetMaterialModeState( IRenderContext *pRenderContext, const IMaterialMode *pMode, const CRenderAttributes *pAttributes, RenderInputLayout_t hInputLayout, const RsStencilStateOverride_t *pOverrideStencilState, const RsDepthBiasStateOverride_t *pOverrideDepthBiasState, int nRequiredMipSize, uint32 nRenderStateSetNotAllowed ) = 0;

	// Clears three cached pointers of the render context state
	virtual void ResetMaterialCache( IRenderContext *pRenderContext ) = 0;
	virtual bool SetDefaultMaterial( IRenderContext *pRenderContext, bool bForce ) = 0;
	virtual bool SetTexturedMaterial( IRenderContext *pRenderContext, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, bool bForce ) = 0;
	virtual bool SetMaterialMode( IRenderContext *pRenderContext, const IMaterialMode *pMode, const CRenderAttributes *pAttributes, uint64 nKey, bool bForce ) = 0;

	virtual void QueueResetMaterialCache( uint32 nQueue, float flSortKey ) = 0;

	// All three queue a set material or texture renderable
	virtual void QueueSetDefaultMaterial( uint32 nQueue, bool bForce, float flTimeToLive ) = 0;
	virtual void QueueSetTexturedMaterial( uint32 nQueue, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, bool bForce, float flTimeToLive ) = 0;
	virtual void QueueSetMaterialMode( uint32 nQueue, const IMaterialMode *pMode, const CRenderAttributes *pAttributes, uint64 nKey, bool bForce, float flTimeToLive ) = 0;

	virtual void RenderBox( IRenderContext *pRenderContext, const Vector &vecOrigin, const QAngle &angles, const Vector &vecMins, const Vector &vecMaxs, Color color, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, bool bInsideOut ) = 0;

	// Draws a dest rectangle of the screen with a rectangle of a src texture, diced into nXDice by nYDice quads at depth flZ
	virtual void DrawScreenSpaceRectangle( IRenderContext *pRenderContext, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, int nDestX, int nDestY, int nWidth, int nHeight, float flSrcTextureX0, float flSrcTextureY0, float flSrcTextureX1, float flSrcTextureY1, int nSrcTextureWidth, int nSrcTextureHeight, int nXDice, int nYDice, float flZ, VertexColor_t *pVertexColors, const RsStencilStateOverride_t *pOverrideStencilState ) = 0;

	virtual void DrawNDCSpaceUntexturedPolygon( IRenderContext *pRenderContext, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, int nVertexCount, Vector2D *pScreenSpaceCoordinates ) = 0;

	virtual bool RenderText( IRenderContext *pRenderContext, const char *pText, const Vector2D &vecPos, Color color, HFont hFont, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType ) = 0;
	virtual bool RenderText( IRenderContext *pRenderContext, const uchar16 *pText, const Vector2D &vecPos, Color color, HFont hFont, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType ) = 0;
	virtual bool RenderText( IRenderContext *pRenderContext, const uchar32 *pText, const Vector2D &vecPos, Color color, HFont hFont, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType ) = 0;
	// Returns the size of the rendered text
	virtual Vector2D RenderText( IRenderContext *pRenderContext, CTextLayoutInfo *pTextLayoutInfo ) = 0;
	virtual bool RenderText( IRenderContext *pRenderContext, const CUtlVector< CUtlVector< CCharRenderGeometry > > &geometry, const Vector2D &vecOffset, const CRenderAttributes *pAttributes ) = 0;

	virtual void Render2DLine( IRenderContext *pRenderContext, float x0, float y0, float x1, float y1, const Color &color, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType ) = 0;

	virtual bool RenderRect( IRenderContext *pRenderContext, const Vector2D &vecPos, const Vector2D &vecSize, Color color, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;
	virtual bool RenderRect( IRenderContext *pRenderContext, const Vector2D &vecPos, const Vector2D &vecSize, const Vector2D &vecUV0, const Vector2D &vecUV1, Color color, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;
	// Renders a quad of four vertices through RenderQuads()
	virtual bool RenderRect( IRenderContext *pRenderContext, const Vector *pVertices, const Color *pColors, const Vector2D *pTexCoords, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;

	virtual bool RenderOutlinedRect( IRenderContext *pRenderContext, const Vector2D &vecPos, const Vector2D &vecSize, float flThickness, Color color, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;
	virtual void RenderMeshWithMode( IRenderContext *pRenderContext, const MeshBuilderBuffers_t *pMesh, const IMaterialMode *pMode, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;
	virtual bool RenderQuads( IRenderContext *pRenderContext, const Vector *pVertices, const Color *pColors, const Vector2D *pTexCoords, int nCount, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;
	virtual bool RenderConvexGeometry( IRenderContext *pRenderContext, const Vector *pVertices, const Color *pColors, const Vector2D *pTexCoords, int nCount, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;

	virtual void QueueRenderText( uint32 nQueue, const char *pText, const Vector2D &vecPos, Color color, HFont hFont, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRenderText( uint32 nQueue, const uchar16 *pText, const Vector2D &vecPos, Color color, HFont hFont, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRenderText( uint32 nQueue, const uchar32 *pText, const Vector2D &vecPos, Color color, HFont hFont, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRenderText( uint32 nQueue, const CTextLayoutInfo &textLayoutInfo, const CRenderAttributes *pAttributes, float flTimeToLive ) = 0;
	virtual void QueueRenderText( Vector2D *pOutSize, uint32 nQueue, CTextLayoutInfo &textLayoutInfo, const CRenderAttributes *pAttributes, float flTimeToLive ) = 0;

	virtual void QueueRender2DLine( uint32 nQueue, float x0, float y0, float x1, float y1, const Color &color, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRender2DLines( uint32 nQueue, const CUtlVector< Vector2D > *pLines, const Color &color, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;

	// Queued counterparts of the RenderRect overloads
	virtual void QueueRenderRect( uint32 nQueue, const Vector2D &vecPos, const Vector2D &vecSize, const Color &color, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRenderRect( uint32 nQueue, const Vector2D &vecPos, const Vector2D &vecSize, const Vector2D &vecUV0, const Vector2D &vecUV1, const Color &color, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRenderRect( uint32 nQueue, const Vector *pVertices, const Color *pColors, const Vector2D *pTexCoords, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;

	// Takes ownership of the mesh buffers and the attributes
	virtual void QueueAndOwnRenderMeshWithMode( uint32 nQueue, const MeshBuilderBuffers_t *pMesh, const IMaterialMode *pMode, CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;

	// Copies the mesh buffers and the attributes, then queues the copy as QueueAndOwnRenderMeshWithMode does
	virtual void QueueRenderMeshWithMode( uint32 nQueue, const MeshBuilderBuffers_t *pMesh, const IMaterialMode *pMode, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;

	virtual void QueueRenderConvexGeometry( uint32 nQueue, const Vector *pVertices, const Color *pColors, const Vector2D *pTexCoords, int nCount, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRenderQuads( uint32 nQueue, const Vector *pVertices, const Color *pColors, const Vector2D *pTexCoords, int nCount, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;

	virtual void RenderFontTexture( IRenderContext *pRenderContext, int nTextureId, int nXPos, int nYPos, const CRenderAttributes *pAttributes ) = 0;

	virtual void CacheFontTextures( const CTextLayoutInfo *pTextLayoutInfo ) = 0;

	// True when the queue nQueue holds renderables
	virtual bool HasQueuedRender( uint32 nQueue ) = 0;

	// Renders every renderable of the queue nQueue
	virtual void RenderFrame( IRenderContext *pRenderContext, uint32 nQueue ) = 0;

	// Size of pText in hFont, the default font for 0
	virtual Vector2D GetRawSize( const char *pText, HFont hFont ) = 0;

	// Releases every queued renderable
	virtual void unk056() = 0;

	virtual void GenerateMipMaps( IRenderContext *pRenderContext, void *pTexture ) = 0;

	// Creates "<name>_intermediate_<n>.vtex" textures
	virtual bool AllocateCompressionTexturesGPU( ImageFormat nFormat, int nWidth, int nHeight, int nNumMipLevels, const char *pDebugName, CStrongHandle< InfoForResourceTypeCTextureBase > *pCompressedOutputTexture, CUtlVector< CStrongHandleCopyable< InfoForResourceTypeCTextureBase > > &intermediateOutputTextures ) = 0;
	virtual bool CompressTextureGPU( EGPUTextureCompressOptions_t nOptions, IRenderContext *pRenderContext, CWeakHandle< InfoForResourceTypeCTextureBase > hSrcTexture, CWeakHandle< InfoForResourceTypeCTextureBase > hDestTexture, const CUtlVector< CStrongHandleCopyable< InfoForResourceTypeCTextureBase > > &intermediateOutputTextures, int nNumMipLevels ) = 0;

	// Shader name and combo values of a shader type of the pass
	virtual CBufferString unk060( const MaterialRenderablePass_t &renderablePass, int nShaderType ) = 0;

	virtual ~IMaterialSystem2Utils() {}
};

#endif // IMATERIALSYSTEM2UTILS_H
