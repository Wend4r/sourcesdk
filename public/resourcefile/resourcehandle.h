#ifndef RESOURCEHANDLE_H
#define RESOURCEHANDLE_H

#ifdef _WIN32
	#pragma once
#endif

#include "resourcetype.h"

//-----------------------------------------------------------------------------
// Typed resource handles, reconstructed from the s&box DWARF dump
//
// The resource system keeps one ResourceBinding_t per resource. Handles are 8-byte wrappers
// around a pointer to that binding — they do NOT derive from it:
//
//   ResourceBinding_t<T> : ResourceBindingBase_t   — the refcounted binding (one per resource)
//   CWeakHandle<T>   { m_pBinding }                — observes; never touches m_nRefCount.
//                                                    The binding outlives the data, so the
//                                                    holder must check IsLoaded() before
//                                                    dereferencing (m_pData can be unloaded).
//   CStrongHandle<T> { m_pBinding }                — owns a reference; Init() interlocked-
//                                                    increments m_nRefCount, Shutdown()
//                                                    decrements it (the engine unloads the
//                                                    resource data when it reaches zero).
//   CStrongHandleCopyable<T>                       — CStrongHandle whose copy operations
//                                                    AddRef, so it can live in containers.
//
//-----------------------------------------------------------------------------

template <typename T>
struct ResourceBinding_t : ResourceBindingBase_t
{
	using RuntimeClass_t = typename T::RuntimeClass_t;

	const RuntimeClass_t* GetData() const { return static_cast<const RuntimeClass_t*>( m_pData ); }
};

class CWeakHandleBase
{
protected:
	const ResourceBindingBase_t* m_pBinding = nullptr;
};

template <class T>
class CWeakHandle : public CWeakHandleBase
{
public:
	using RuntimeClass_t = typename T::RuntimeClass_t;
	typedef const ResourceBinding_t<T>* ResourceHandleTyped_t;

	CWeakHandle() = default;
	explicit CWeakHandle( ResourceHandleTyped_t hResource ) { m_pBinding = hResource; }

	void Init( ResourceHandleTyped_t hResource ) { m_pBinding = hResource; }
	void InitUntypedUnchecked( ResourceHandle_t hUntypedResource ) { m_pBinding = hUntypedResource; }

	static CWeakHandle FromUntypedHandleUnchecked( ResourceHandle_t hUntypedResource )
	{
		CWeakHandle h;
		h.InitUntypedUnchecked( hUntypedResource );
		return h;
	}

	ResourceHandleTyped_t GetBinding() const { return static_cast<ResourceHandleTyped_t>( m_pBinding ); }

	bool HasHandle() const { return m_pBinding != nullptr; }
	bool HasIdentity() const { return m_pBinding && m_pBinding->m_Name != nullptr; }
	bool HasData() const { return m_pBinding && m_pBinding->m_pData != nullptr; }
	bool IsError() const { return m_pBinding && ( m_pBinding->m_nFlags & RESOURCE_BINDING_ERROR ) != 0; }

	bool IsLoaded() const { return m_pBinding && m_pBinding->m_nRefCount > 0 && m_pBinding->m_pData && !IsError(); }

	int GetRefCount() const { return m_pBinding ? (int)const_cast<ResourceBindingBase_t*>( m_pBinding )->m_nRefCount : 0; }
	int GetReloadCounter() const { return m_pBinding ? m_pBinding->m_nReloadCounter : 0; }

	const RuntimeClass_t* GetData() const
	{
		return m_pBinding ? static_cast<const RuntimeClass_t*>( m_pBinding->m_pData ) : nullptr;
	}

	const RuntimeClass_t* operator->() const { return GetData(); }
	operator ResourceHandle_t() const { return m_pBinding; }
	explicit operator bool() const { return HasData(); }

	bool operator==( const CWeakHandle& other ) const { return m_pBinding == other.m_pBinding; }
	bool operator!=( const CWeakHandle& other ) const { return m_pBinding != other.m_pBinding; }
};

class CStrongHandleBase
{
public:
	const ResourceBindingBase_t* GetResourceBindingBase() const { return m_pBinding; }

protected:
	const ResourceBindingBase_t* m_pBinding = nullptr;
};

template <class T>
class CStrongHandle : public CStrongHandleBase
{
public:
	using RuntimeClass_t = typename T::RuntimeClass_t;
	typedef const ResourceBinding_t<T>* ResourceHandleTyped_t;

	CStrongHandle() = default;
	explicit CStrongHandle( ResourceHandleTyped_t hResource ) { Init( hResource ); }
	~CStrongHandle() { Shutdown(); }

	CStrongHandle( const CStrongHandle& ) = delete;
	CStrongHandle& operator=( const CStrongHandle& ) = delete;

	CStrongHandle( CStrongHandle&& other ) noexcept
	{
		m_pBinding = other.m_pBinding;
		other.m_pBinding = nullptr;
	}

	void Init( ResourceHandleTyped_t hResource )
	{
		Shutdown();
		m_pBinding = hResource;
		if ( m_pBinding )
			++const_cast<ResourceBindingBase_t*>( m_pBinding )->m_nRefCount;
	}

	void Shutdown()
	{
		if ( m_pBinding )
		{
			--const_cast<ResourceBindingBase_t*>( m_pBinding )->m_nRefCount;
			m_pBinding = nullptr;
		}
	}

	ResourceHandleTyped_t GetBinding() const { return static_cast<ResourceHandleTyped_t>( m_pBinding ); }

	bool HasHandle() const { return m_pBinding != nullptr; }
	bool HasData() const { return m_pBinding && m_pBinding->m_pData != nullptr; }
	bool IsError() const { return m_pBinding && ( m_pBinding->m_nFlags & RESOURCE_BINDING_ERROR ) != 0; }
	bool IsLoaded() const { return HasData() && !IsError(); }

	int GetRefCount() const { return m_pBinding ? (int)const_cast<ResourceBindingBase_t*>( m_pBinding )->m_nRefCount : 0; }
	int GetReloadCounter() const { return m_pBinding ? m_pBinding->m_nReloadCounter : 0; }

	const RuntimeClass_t* GetData() const
	{
		return m_pBinding ? static_cast<const RuntimeClass_t*>( m_pBinding->m_pData ) : nullptr;
	}
	const RuntimeClass_t* GetDataUnchecked() const { return static_cast<const RuntimeClass_t*>( m_pBinding->m_pData ); }

	const RuntimeClass_t* operator->() const { return GetData(); }
	operator ResourceHandle_t() const { return m_pBinding; }
	explicit operator bool() const { return HasData(); }
};

template <class T>
class CStrongHandleCopyable : public CStrongHandle<T>
{
public:
	using BaseClass = CStrongHandle<T>;
	using typename BaseClass::ResourceHandleTyped_t;

	CStrongHandleCopyable() = default;
	explicit CStrongHandleCopyable( ResourceHandleTyped_t hResource ) { this->Init( hResource ); }

	CStrongHandleCopyable( const CStrongHandleCopyable& other ) { this->Init( other.GetBinding() ); }
	CStrongHandleCopyable& operator=( const CStrongHandleCopyable& other )
	{
		if ( this != &other )
			this->Init( other.GetBinding() );
		return *this;
	}
};

#endif // RESOURCEHANDLE_H
