#ifndef IMATERIALSYSTEM2UTILS_H
#define IMATERIALSYSTEM2UTILS_H

#ifdef _WIN32
	#pragma once
#endif

#include "ifontmanager.h"
#include "imaterial2.h"

#include <appframework/iappsystem.h>
#include <bitmap/imageformat_declarations.h>
#include <mathlib/vector2d.h>
#include <resourcefile/resourcetype.h>
#include <resourcesystem/resourcehandletypes.h>

#include <color.h>
#include <tier0/bufferstring.h>
#include <tier0/platform.h>
#include <tier0/utlstringtoken.h>
#include <tier0/utlvector.h>
#include <tier0/wchartypes.h>

class IRenderContext;
class CRenderAttributes;
class CTextLayoutInfo;
class IMaterialMode;
struct CCharRenderGeometry;
struct MeshBuilderBuffers_t;
struct RsStencilStateOverride_t;
struct RsDepthBiasStateOverride_t;
struct RenderInputLayout_t__;
struct VertexColor_t;

typedef RenderInputLayout_t__ *RenderInputLayout_t;

enum LayoutPositionType_e : int;
enum EGPUTextureCompressOptions_t : int;

//-----------------------------------------------------------------------------
// Immediate and queued 2D rendering helpers of the material system.
//
// Names and arguments from the s&box DWARF of IMaterialSystem2Utils where the
// CS2 body matches. CS2 has no 3D sphere/box/axes/line/triangle helpers, no
// RenderPrimitives / QueueRenderPrimitives and no PrepareForRender, and has
// GenerateMipMaps and GetMaterialModeShaderName that s&box does not.
//-----------------------------------------------------------------------------
abstract_class IMaterialSystem2Utils : public IAppSystem
{
public:
	// Used by the text layout whenever a caller passes font 0
	virtual HFont GetDefaultDebugFont( bool bOutline ) = 0;

	virtual bool SetMaterialModeState( IRenderContext *pRenderContext, const IMaterialMode *pMode, const CRenderAttributes *pAttributes, RenderInputLayout_t hInputLayout, const RsStencilStateOverride_t *pOverrideStencilState, const RsDepthBiasStateOverride_t *pOverrideDepthBiasState, int nRequiredMipSize, uint32 nRenderStateSetNotAllowed ) = 0;

	// Clears the three cached state pointers of the render context
	virtual void ResetMaterialCache( IRenderContext *pRenderContext ) = 0;
	virtual bool SetDefaultMaterial( IRenderContext *pRenderContext, bool bForce ) = 0;
	virtual bool SetTexturedMaterial( IRenderContext *pRenderContext, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, bool bForce ) = 0;
	virtual bool SetMaterialMode( IRenderContext *pRenderContext, const IMaterialMode *pMode, const CRenderAttributes *pAttributes, uint64 nKey, bool bForce ) = 0;

	virtual void QueueResetMaterialCache( CUtlStringToken viewportName, float flTimeToLive ) = 0;
	virtual void QueueSetDefaultMaterial( CUtlStringToken viewportName, bool bForce, float flTimeToLive ) = 0;
	virtual void QueueSetTexturedMaterial( CUtlStringToken viewportName, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, bool bForce, float flTimeToLive ) = 0;
	virtual void QueueSetMaterialMode( CUtlStringToken viewportName, const IMaterialMode *pMode, const CRenderAttributes *pAttributes, uint64 nKey, bool bForce, float flTimeToLive ) = 0;

	virtual void RenderQuad( IRenderContext *pRenderContext, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, float flX0, float flY0, float flX1, float flY1, float flZ, float flU0, float flV0, float flU1, float flV1, const Color &color ) = 0;
	virtual void DrawScreenSpaceRectangle( IRenderContext *pRenderContext, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, int nDestX, int nDestY, int nWidth, int nHeight, float flSrcTextureX0, float flSrcTextureY0, float flSrcTextureX1, float flSrcTextureY1, int nSrcTextureWidth, int nSrcTextureHeight, int nXDice, int nYDice, float flDepth, VertexColor_t *pVertexColors ) = 0;
	virtual void DrawNDCSpaceUntexturedPolygon( IRenderContext *pRenderContext, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, int nPointCount, Vector2D *pPoints ) = 0;

	virtual bool RenderText( IRenderContext *pRenderContext, const char *pText, const Vector2D &vecPos, Color color, HFont hFont, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType ) = 0;
	virtual bool RenderText( IRenderContext *pRenderContext, const uchar16 *pText, const Vector2D &vecPos, Color color, HFont hFont, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType ) = 0;
	virtual bool RenderText( IRenderContext *pRenderContext, const uchar32 *pText, const Vector2D &vecPos, Color color, HFont hFont, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType ) = 0;
	// Returns the size of the rendered text
	virtual Vector2D RenderText( IRenderContext *pRenderContext, CTextLayoutInfo *pTextLayoutInfo, const CRenderAttributes *pAttributes ) = 0;
	virtual bool RenderText( IRenderContext *pRenderContext, const CUtlVector< CUtlVector< CCharRenderGeometry > > &renderGeometryLists, const Vector2D &vecOffset, const CRenderAttributes *pAttributes ) = 0;

	virtual void Render2DLine( IRenderContext *pRenderContext, float flX0, float flY0, float flX1, float flY1, const Color &color, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType ) = 0;

	virtual bool RenderRect( IRenderContext *pRenderContext, const Vector2D &vecPos, const Vector2D &vecSize, Color color, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;
	virtual bool RenderRect( IRenderContext *pRenderContext, const Vector2D &vecPos, const Vector2D &vecSize, const Vector2D &vecUV0, const Vector2D &vecUV1, Color color, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;
	virtual bool RenderRect( IRenderContext *pRenderContext, const Vector *pPoints, const Color *pColors, const Vector2D *pUVs, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;

	virtual bool RenderOutlinedRect( IRenderContext *pRenderContext, const Vector2D &vecPos, const Vector2D &vecSize, float flThickness, Color color, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;
	virtual void RenderMeshWithMode( IRenderContext *pRenderContext, const MeshBuilderBuffers_t *pMesh, const IMaterialMode *pMode, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;
	virtual bool RenderQuads( IRenderContext *pRenderContext, const Vector *pPoints, const Color *pColors, const Vector2D *pUVs, int nNumPoints, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;
	virtual bool RenderConvexGeometry( IRenderContext *pRenderContext, const Vector *pPoints, const Color *pColors, const Vector2D *pUVs, int nNumPoints, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType ) = 0;

	virtual void QueueRenderText( CUtlStringToken viewportName, const char *pText, const Vector2D &vecPos, Color color, HFont hFont, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRenderText( CUtlStringToken viewportName, const uchar16 *pText, const Vector2D &vecPos, Color color, HFont hFont, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRenderText( CUtlStringToken viewportName, const uchar32 *pText, const Vector2D &vecPos, Color color, HFont hFont, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRenderText( CUtlStringToken viewportName, const CTextLayoutInfo &textLayoutInfo, const CRenderAttributes *pAttributes, float flTimeToLive ) = 0;
	virtual void QueueRenderText( Vector2D *pOutSize, CUtlStringToken viewportName, CTextLayoutInfo &textLayoutInfo, const CRenderAttributes *pAttributes, float flTimeToLive ) = 0;

	virtual void QueueRender2DLine( CUtlStringToken viewportName, float flX0, float flY0, float flX1, float flY1, const Color &color, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRender2DLines( CUtlStringToken viewportName, const CUtlVector< Vector2D > *pLineList, const Color &color, const CRenderAttributes *pAttributes, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;

	virtual void QueueRenderRect( CUtlStringToken viewportName, const Vector2D &vecPos, const Vector2D &vecSize, const Color &color, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRenderRect( CUtlStringToken viewportName, const Vector2D &vecPos, const Vector2D &vecSize, const Vector2D &vecUV0, const Vector2D &vecUV1, const Color &color, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRenderRect( CUtlStringToken viewportName, const Vector *pPoints, const Color *pColors, const Vector2D *pUVs, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;

	// The first keeps pAttributes, the second copies the mesh buffers
	virtual void QueueAndOwnRenderMeshWithMode( CUtlStringToken viewportName, const MeshBuilderBuffers_t *pMesh, const IMaterialMode *pMode, CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRenderMeshWithMode( CUtlStringToken viewportName, const MeshBuilderBuffers_t *pMesh, const IMaterialMode *pMode, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;

	// Queued primitive type 7 and 6
	virtual void QueueRenderConvexGeometry( CUtlStringToken viewportName, const Vector *pPoints, const Color *pColors, const Vector2D *pUVs, int nNumPoints, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;
	virtual void QueueRenderQuads( CUtlStringToken viewportName, const Vector *pPoints, const Color *pColors, const Vector2D *pUVs, int nNumPoints, const IMaterial2 *pMaterial, const CRenderAttributes *pAttributes, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, LayoutPositionType_e nPositionType, float flTimeToLive ) = 0;

	virtual void RenderFontTexture( IRenderContext *pRenderContext, int nTextureId, int nX, int nY, const CRenderAttributes *pAttributes ) = 0;
	virtual void CacheFontTextures( const CTextLayoutInfo *pTextLayoutInfo ) = 0;

	virtual bool HasQueuedRender( CUtlStringToken viewportName ) = 0;
	virtual void RenderFrame( IRenderContext *pRenderContext, CUtlStringToken viewportName ) = 0;

	// Size of pText in hFont, the default font for 0
	virtual Vector2D GetRawSize( const char *pText, HFont hFont ) = 0;

	// Hands every queued renderable to the views and releases the expired ones
	virtual void AddToView( float flAbsTime ) = 0;

	virtual void GenerateMipMaps( IRenderContext *pRenderContext, void *pTexture ) = 0;

	// Creates "<name>_intermediate_<n>.vtex" textures
	virtual bool AllocateCompressionTexturesGPU( ImageFormat nFormat, int nWidth, int nHeight, int nMipCount, const char *pName, CStrongHandle< InfoForResourceTypeCTextureBase > *pOutTexture, CUtlVector< CStrongHandleCopyable< InfoForResourceTypeCTextureBase > > &intermediateTextures ) = 0;
	virtual bool CompressTextureGPU( EGPUTextureCompressOptions_t nOptions, IRenderContext *pRenderContext, CWeakHandle< InfoForResourceTypeCTextureBase > hSource, CWeakHandle< InfoForResourceTypeCTextureBase > hDest, const CUtlVector< CStrongHandleCopyable< InfoForResourceTypeCTextureBase > > &intermediateTextures, int nMip ) = 0;

	// Named by what it does: a lower-cased string that the material's resource data holds for
	// the layer nLayer + 1
	virtual CBufferString GetMaterialLayerString( HMaterialWeak hMaterial, int nLayer ) = 0;

	virtual ~IMaterialSystem2Utils() {}
};

#endif // IMATERIALSYSTEM2UTILS_H
