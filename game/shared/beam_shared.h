#ifndef BEAM_SHARED_H
#define BEAM_SHARED_H

#pragma once

#include "basemodelentity.h"

//-----------------------------------------------------------------------------
// CBeam adds 1 method to CBaseModelEntity: Linux slot 272, Windows 271.
//-----------------------------------------------------------------------------
template < class TBase = CBaseModelEntityVTable<> >
abstract_class CBeamVTable : public TBase
{
public:
	virtual const char *GetDecalName() = 0;
};

#endif // BEAM_SHARED_H
