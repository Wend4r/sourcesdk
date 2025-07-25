//========= Copyright 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef WINLITE_H
#define WINLITE_H
#pragma once

#ifdef _WIN32
// 
// Prevent tons of unused windows definitions
//
#define WIN32_LEAN_AND_MEAN
#define NOWINRES
#define NOSERVICE
#define NOMCX
#define NOIME
#define NOMINMAX
#if !defined( _X360 )
#pragma warning(push, 1)
#pragma warning(disable: 4005)
#include <windows.h>
#pragma warning(pop)
#endif
#undef GetObject
#undef DrawText
#undef CreateWindow
#undef CreateEvent
#undef SendMessage
#undef PostMessage
#undef MessageBox
#undef GetClassName
#undef LoadImage
#undef LoadIcon
#undef CreateDialog
#undef SetProp
#undef RegisterClass
#undef UnregisterClass
#undef CreateFont
#undef CreateBitmap
#undef CreatePen
#undef CreateBrush
#undef CreateDC
#undef DeleteObject
#undef GetMessage
#undef GetModuleHandle
#undef GetModuleFileName
#undef SetWindowLong
#undef GetWindowLong
#undef SetWindowText
#undef GetWindowText
#undef GetWindowRect
#undef SetTimer
#undef GetTimer
#undef CreateMutex
#undef OpenEvent
#undef WaitForSingleObject

#endif // WIN32
#endif // WINLITE_H
