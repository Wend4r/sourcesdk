//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: Occlusion queries on top of the render device
//
//===========================================================================//

#ifndef IRENDERUTILS_H
#define IRENDERUTILS_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"

class IRenderContext;

// 1-based index of an occlusion query object, 0 is invalid
FORWARD_DECLARE_HANDLE( OcclusionQueryObjectHandle_t );
#define INVALID_OCCLUSION_QUERY_OBJECT_HANDLE ( (OcclusionQueryObjectHandle_t)0 )

abstract_class IRenderUtils : public IAppSystem
{
public:
	virtual OcclusionQueryObjectHandle_t CreateOcclusionQueryObject() = 0;
	virtual void DestroyOcclusionQueryObject( OcclusionQueryObjectHandle_t hQuery ) = 0;

	virtual int OcclusionQuery_GetNumPixelsRendered( OcclusionQueryObjectHandle_t hQuery ) = 0;

	virtual void ResetOcclusionQueryObject( OcclusionQueryObjectHandle_t hQuery ) = 0;

	// Returns false when no device query is free yet
	virtual bool BeginOcclusionQueryDrawing( OcclusionQueryObjectHandle_t hQuery, IRenderContext *pRenderContext ) = 0;
	virtual void EndOcclusionQueryDrawing( OcclusionQueryObjectHandle_t hQuery, IRenderContext *pRenderContext ) = 0;
};

#endif // IRENDERUTILS_H
