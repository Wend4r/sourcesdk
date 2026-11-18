#ifndef ISCENEANIMATABLEPROCEDURALBONETRANSFORMS_H
#define ISCENEANIMATABLEPROCEDURALBONETRANSFORMS_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CSceneAnimatableObject;

//-----------------------------------------------------------------------------
// Implemented by objects that write procedural bone transforms into an animatable scene object
// (the game's skeleton instance, the model resource). A single virtual.
//-----------------------------------------------------------------------------
abstract_class ISceneAnimatableProceduralBoneTransforms
{
public:
	virtual void SetBoneTransformsInSceneObject( CSceneAnimatableObject *pSceneObject, uint8 nLOD ) = 0;
};

#endif // ISCENEANIMATABLEPROCEDURALBONETRANSFORMS_H
