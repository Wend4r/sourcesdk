//========== Copyright � 2008, Valve Corporation, All rights reserved. ========
//
// Purpose:
//
//=============================================================================

// This exists to allow projects that aren't built with memoverride.cpp to still link
// even if USE_MEMDEBUG is enabled [12/1/2009 tom]

const char* GetModuleName()
{
    return "memoverride not present";
}