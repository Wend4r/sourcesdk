#ifndef TEXTLAYOUT_H
#define TEXTLAYOUT_H

#ifdef _WIN32
#pragma once
#endif

#include "materialsystem2/itextlayout.h"
#include <tier2/tier2.h>

//-----------------------------------------------------------------------------
// Implementation of ITextLayout, without members of its own
//-----------------------------------------------------------------------------
class CTextLayout : public CTier2AppSystem< ITextLayout >
{
};

#endif // TEXTLAYOUT_H
