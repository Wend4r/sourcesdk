#ifndef BARNLIGHT_H
#define BARNLIGHT_H

#pragma once

#include "../shared/basemodelentity.h"

//-----------------------------------------------------------------------------
// CBarnLight adds 6 methods to CBaseModelEntity: Linux slots 272-277, Windows 271-276.
//-----------------------------------------------------------------------------
template < class TBase = CBaseModelEntityVTable<> >
abstract_class CBarnLightVTable : public TBase
{
public:
	// Returns pi divided by a function of m_vSizeParams, m_flRange, m_vShear, m_nLuminaireShape and m_flLuminaireSize, a solid angle style normalisation of the luminaire; overridden by COmniLight and CRectLight
	virtual float Unk_271() = 0;
	// Returns false in every class
	virtual bool Stub_272() = 0;
	// Fills a light description from the fields of the barn light; overridden by COmniLight and CRectLight
	virtual void Unk_273( void *pLightDesc, int nUnknown, double flUnknown ) = 0;
	// Empty in every class
	virtual void Stub_274() = 0;
	// Builds or updates the light object of the scene system, in the secondary scene world; overridden by COmniLight and CRectLight
	virtual void Unk_275( void *pLightDesc, void *pSceneWorld, bool bUnknown ) = 0;
	// Returns m_Color (or the colour of m_flColorTemperature) times exp2(m_flBrightness) * m_flBrightnessScale, modulated by the light style since m_flLightStyleStartTime; 0 when !m_bEnabled. Named by what it does
	virtual Vector GetLinearColorIntensity() = 0;
};

#endif // BARNLIGHT_H
