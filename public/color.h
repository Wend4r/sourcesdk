//========= Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef COLOR_H
#define COLOR_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/basetypes.h"

//-----------------------------------------------------------------------------
// Purpose: Basic handler for an rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
class Color
{
public:
	union
	{
		struct { unsigned char m_r, m_g, m_b, m_a; };
		int m_rawColor;
		unsigned char m_array[4];
	};

	// default constructor
	Color(const Color &) = default;
	Color(Color &&) = default;
	Color &operator=(const Color &) = default;
	Color &operator=(Color &&) = default;
	Color(int color = 0)
		: m_rawColor(color) {}

	// initialize from components
	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0)
		: m_r(r), m_g(g), m_b(b), m_a(a) {}

	// set the color
	void SetColor(int r, int g, int b, int a = 0)
	{
		m_r = static_cast<unsigned char>(r);
		m_g = static_cast<unsigned char>(g);
		m_b = static_cast<unsigned char>(b);
		m_a = static_cast<unsigned char>(a);
	}

	void GetColor(int &r, int &g, int &b, int &a) const
	{
		r = m_r;
		g = m_g;
		b = m_b;
		a = m_a;
	}

	void SetRawColor(int color) { m_rawColor = color; }
	int GetRawColor() const { return m_rawColor; }

	unsigned char &operator[](int index) { return m_array[index]; }
	const unsigned char &operator[](int index) const { return m_array[index]; }

	bool operator==(const Color &rhs) const { return m_rawColor == rhs.m_rawColor; }
	bool operator!=(const Color &rhs) const { return m_rawColor != rhs.m_rawColor; }

	// inline component access
	int r() const { return m_r; }
	int g() const { return m_g; }
	int b() const { return m_b; }
	int a() const { return m_a; }

	// conversions
	color32 ToColor32() const
	{
		color32 c;
		c.r = m_r;
		c.g = m_g;
		c.b = m_b;
		c.a = m_a;
		return c;
	}

	colorVec4 ToVector4D() const
	{
		colorVec4 v;
		v.r = static_cast<float>(m_r);
		v.g = static_cast<float>(m_g);
		v.b = static_cast<float>(m_b);
		v.a = static_cast<float>(m_a);
		return v;
	}

	void FromVector4D(const colorVec4& v)
	{
		m_r = static_cast<unsigned char>(v.r);
		m_g = static_cast<unsigned char>(v.g);
		m_b = static_cast<unsigned char>(v.b);
		m_a = static_cast<unsigned char>(v.a);
	}

	void FromColor32(const color32& c)
	{
		m_r = c.r;
		m_g = c.g;
		m_b = c.b;
		m_a = c.a;
	}
};

#endif // COLOR_H
