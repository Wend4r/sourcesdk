#ifndef SERVERENTITYSUBCLASSUTILS_H
#define SERVERENTITYSUBCLASSUTILS_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/gameappsystem.h"
#include "game/server/iserverentitysubclassutils.h"

//-----------------------------------------------------------------------------
// Answers the entity subclass (vdata) queries for the other modules; defined
// next to the entity subclass game system, it has no state of its own
//-----------------------------------------------------------------------------
class CServerEntitySubclassUtils : public CGameAppSystem< IServerEntitySubclassUtils >
{
};

#endif // SERVERENTITYSUBCLASSUTILS_H
