//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Insert this file into all projects using the memory system
// It will cause that project to use the shader memory allocator
//
// $NoKeywords: $
//=============================================================================//
#if !defined(STEAM) && !defined(NO_MALLOC_OVERRIDE)

#include "tier0/dbg.h"
#include "tier0/memalloc.h"
#include "memdbgoff.h"

const char* GetModuleName()
{
	static char szModule[MAX_PATH]{};
	if (szModule[0] == '\0') Plat_GetModuleFilename(szModule, sizeof(szModule));
	return szModule;
}

//-----------------------------------------------------------------------------
// Prevents us from using an inappropriate new or delete method,
// ensures they are here even when linking against debug or release static libs
//-----------------------------------------------------------------------------
#ifndef NO_MEMOVERRIDE_NEW_DELETE

FORCEINLINE void *AllocUnattributed( size_t nSize )
{
#if !defined(USE_LIGHT_MEM_DEBUG) && !defined(USE_MEM_DEBUG)
	return MemAlloc_Alloc(nSize);
#else
	return MemAlloc_Alloc(nSize, GetModuleName(), 0);
#endif
}

// ==== operator new (may return nullptr) ====
void* operator new(size_t size)
{
	return AllocUnattributed(size);
}

void* operator new[](size_t size)
{
	return AllocUnattributed(size);
}

// ==== operator new (nothrow overloads are noexcept) ====
void* operator new(size_t size, const std::nothrow_t&) noexcept
{
	return AllocUnattributed(size);
}

void* operator new[](size_t size, const std::nothrow_t&) noexcept
{
	return AllocUnattributed(size);
}

// ==== operator delete (must be noexcept) ====
void operator delete(void* ptr) noexcept
{
	if (ptr) g_pMemAlloc->Free(ptr);
}

void operator delete[](void* ptr) noexcept
{
	if (ptr) g_pMemAlloc->Free(ptr);
}

// ==== sized delete (must be noexcept) ====
void operator delete(void* ptr, size_t) noexcept
{
	if (ptr) g_pMemAlloc->Free(ptr);
}

void operator delete[](void* ptr, size_t) noexcept
{
	if (ptr) g_pMemAlloc->Free(ptr);
}

// ==== aligned new/delete (aligned new can throw, so not noexcept) ====
void* operator new(size_t size, std::align_val_t)
{
	return AllocUnattributed(size);
}

void* operator new[](size_t size, std::align_val_t)
{
	return AllocUnattributed(size);
}

// ==== aligned delete (must be noexcept) ====
void operator delete(void* ptr, std::align_val_t) noexcept
{
	if (ptr) g_pMemAlloc->Free(ptr);
}

void operator delete[](void* ptr, std::align_val_t) noexcept
{
	if (ptr) g_pMemAlloc->Free(ptr);
}

void operator delete(void* ptr, size_t, std::align_val_t) noexcept
{
	if (ptr) g_pMemAlloc->Free(ptr);
}

void operator delete[](void* ptr, size_t, std::align_val_t) noexcept
{
	if (ptr) g_pMemAlloc->Free(ptr);
}

#endif

#endif