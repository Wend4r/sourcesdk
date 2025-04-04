//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TOGGLETEXTUREPROXY
#define TOGGLETEXTUREPROXY

#include "materialsystem/IMaterialProxy.h"

class IMaterial;
class IMaterialVar;

#ifdef _MSC_VER
#pragma warning (disable : 4100)
#endif

class CBaseToggleTextureProxy : public IMaterialProxy
{
public:
	CBaseToggleTextureProxy();
	virtual ~CBaseToggleTextureProxy();

	virtual bool Init( IMaterial *pMaterial, KeyValues *pKeyValues );
	virtual void OnBind( void *pC_BaseEntity );
	virtual void Release( void ) { delete this; }
	virtual IMaterial *GetMaterial();

private:
	void Cleanup();

	C_BaseEntity *BindArgToEntity( void *pArg );

	IMaterialVar *m_TextureVar;
	IMaterialVar *m_TextureFrameNumVar;
	bool m_WrapAnimation;
};

#endif // TOGGLETEXTUREPROXY