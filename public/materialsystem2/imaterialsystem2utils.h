#ifndef IMATERIALSYSTEM2UTILS_H
#define IMATERIALSYSTEM2UTILS_H

#ifdef _WIN32
	#pragma once
#endif

#include "ifontmanager.h"

#include <appframework/iappsystem.h>
#include <mathlib/vector2d.h>
#include <resourcefile/resourcetype.h>

#include <color.h>
#include <tier0/bufferstring.h>
#include <tier0/platform.h>
#include <tier0/wchartypes.h>

class IRenderContext;

abstract_class IMaterialSystem2Utils : public IAppSystem
{
public:
	// Used by the text layout whenever a caller passes font 0
	virtual HFont GetDefaultFont( bool bUnk ) = 0;

	virtual bool unk012( void *p ) = 0;

	// Clears three cached pointers of the block at p
	virtual void unk013( void *p ) = 0;
	virtual bool unk014( void *p, bool bUnk ) = 0;
	virtual bool unk015( void *p ) = 0;
	virtual bool unk016( void *p ) = 0;

	virtual void QueueResetMaterialCache( uint32 nQueue, float flSortKey ) = 0;

	// All three queue a set material or texture renderable
	virtual void unk018( void *p ) = 0;
	virtual void unk019( void *p ) = 0;
	virtual void unk020( void *p ) = 0;

	virtual void unk021( void *p ) = 0;
	virtual void unk022( void *p ) = 0;
	virtual void unk023( void *p ) = 0;

	virtual bool RenderText( IRenderContext *pRenderContext, const char *pText, const Vector2D &vecPos, Color color, HFont hFont, int nMode, int nScaleMode ) = 0;
	virtual bool RenderText( IRenderContext *pRenderContext, const uchar16 *pText, const Vector2D &vecPos, Color color, HFont hFont, int nMode, int nScaleMode ) = 0;
	virtual bool RenderText( IRenderContext *pRenderContext, const uchar32 *pText, const Vector2D &vecPos, Color color, HFont hFont, int nMode, int nScaleMode ) = 0;
	// Returns the size of the rendered text
	virtual Vector2D RenderText( IRenderContext *pRenderContext, const void *pTextDesc ) = 0;
	virtual bool RenderText( IRenderContext *pRenderContext, const void *pLayout, void *p, int nMode ) = 0;

	virtual void unk029( void *p ) = 0;

	virtual bool RenderQuad( IRenderContext *pRenderContext, const Vector2D &vecPos, const Vector2D &vecSize, Color color, void *p5, void *p6, void *p7, int n8 ) = 0;
	virtual bool RenderQuad( IRenderContext *pRenderContext, const Vector2D &vecPos, const Vector2D &vecSize, const Vector2D &vecUV0, const Vector2D &vecUV1, Color color, void *p7, void *p8, void *p9, int n10 ) = 0;
	virtual bool RenderQuad( IRenderContext *pRenderContext, const void *pVertices, const Color *pColors, const void *pTexCoords, int nCount, void *p6, void *p7, void *p8, int n9 ) = 0;

	virtual bool RenderOutlinedRect( IRenderContext *pRenderContext, const Vector2D &vecPos, const Vector2D &vecSize, float flThickness, Color color, void *p6, void *p7, void *p8, int n9 ) = 0;
	virtual void RenderMesh( void *p ) = 0;
	virtual bool RenderQuads( IRenderContext *pRenderContext, const void *pVertices, const Color *pColors, const void *pTexCoords, int nCount, void *p6, void *p7, void *p8, int n9 ) = 0;
	virtual bool RenderConvexGeometry( IRenderContext *pRenderContext, const void *pVertices, const Color *pColors, const void *pTexCoords, int nCount, void *p6, void *p7, void *p8, int n9 ) = 0;

	virtual void QueueRenderText( uint32 nQueue, const char *pText, const Vector2D &vecPos, Color color, HFont hFont, int nMode, int n7, float flSortKey ) = 0;
	virtual void QueueRenderText( uint32 nQueue, const uchar16 *pText, const Vector2D &vecPos, Color color, HFont hFont, int nMode, int n7, float flSortKey ) = 0;
	virtual void QueueRenderText( uint32 nQueue, const uchar32 *pText, const Vector2D &vecPos, Color color, HFont hFont, int nMode, int n7, float flSortKey ) = 0;
	virtual void QueueRenderText( uint32 nQueue, const void *p2, void *p3, float flSortKey ) = 0;
	virtual void QueueRenderText( Vector2D *pOutSize, uint32 nQueue, void *p3, void *p4, float flSortKey ) = 0;

	// Queue a line and a line batch renderable
	virtual void unk042( void *p ) = 0;
	virtual void unk043( void *p ) = 0;

	// Queued counterparts of the RenderQuad overloads
	virtual void QueueRenderQuad( uint32 nQueue, const Vector2D &vecPos, const Vector2D &vecSize, Color color, void *p5, void *p6, void *p7, int n8 ) = 0;
	virtual void QueueRenderQuad( uint32 nQueue, const Vector2D &vecPos, const Vector2D &vecSize, const Vector2D &vecUV0, const Vector2D &vecUV1, Color color, void *p7, void *p8, void *p9, int n10 ) = 0;
	virtual void QueueRenderQuad( uint32 nQueue, const void *pVertices, const Color *pColors, const void *pTexCoords, void *p5, void *p6, void *p7, int n8 ) = 0;

	virtual void QueueRenderMesh( void *p ) = 0;

	virtual void unk048( void *p ) = 0;

	virtual void QueueRenderConvexGeometry( uint32 nQueue, const void *pVertices, const Color *pColors, const void *pTexCoords, int nCount, void *p6, void *p7, void *p8, int n9 ) = 0;
	virtual void QueueRenderQuads( uint32 nQueue, const void *pVertices, const Color *pColors, const void *pTexCoords, int nCount, void *p6, void *p7, void *p8, int n9 ) = 0;

	virtual void unk051( void *p ) = 0;

	virtual void unk052( const void *pTextDesc ) = 0;

	// True when the queue nQueue holds renderables
	virtual bool unk053( uint32 nQueue ) = 0;

	// Renders every renderable of the queue nQueue
	virtual void unk054( IRenderContext *pRenderContext, uint32 nQueue ) = 0;

	// Size of pText in hFont, the default font for 0
	virtual Vector2D unk055( const char *pText, HFont hFont ) = 0;

	// Releases every queued renderable
	virtual void unk056() = 0;

	virtual void GenerateMipMaps( IRenderContext *pRenderContext, void *pTexture ) = 0;

	// Creates "<name>_intermediate_<n>.vtex" textures
	virtual bool unk058( void *p ) = 0;
	virtual bool unk059( void *p ) = 0;

	virtual CBufferString unk060( ResourceHandle_t hMaterial, int nMode ) = 0;

	virtual ~IMaterialSystem2Utils() {}
};

#endif // IMATERIALSYSTEM2UTILS_H
