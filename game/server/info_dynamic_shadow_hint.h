#ifndef INFO_DYNAMIC_SHADOW_HINT_H
#define INFO_DYNAMIC_SHADOW_HINT_H

#pragma once

#include "baseentity.h"

//-----------------------------------------------------------------------------
// CInfoDynamicShadowHint adds 2 methods to CPointEntity (through CPointEntity, which adds no method): Linux slots 239-240, Windows 238-239.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CInfoDynamicShadowHintVTable : public TBase
{
public:
	// |vecPos - origin| < m_flRange. Named by what it does
	virtual bool IsWithinRange( const Vector &vecPos ) = 0;
	// Draws the hint range as debug overlay. Named by what it does
	virtual void DrawDebugOverlay() = 0;
};

#endif // INFO_DYNAMIC_SHADOW_HINT_H
