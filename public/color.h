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
	// constructors
	Color(const Color& copyFrom) = default;
	Color(Color&& moveFrom) = default;

	Color(int color = 0)
	{
		*((int *)this) = color;
	}
	Color(int _r,int _g,int _b)
	{
		SetColor(_r, _g, _b, 0);
	}
	Color(int _r,int _g,int _b,int _a)
	{
		SetColor(_r, _g, _b, _a);
	}
	explicit Color(const colorVec4& vector)
	{
		SetColor(static_cast<int>(vector.r), static_cast<int>(vector.g), static_cast<int>(vector.b), static_cast<int>(vector.a));
	}
	
	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	void SetColor(int _r, int _g, int _b, int _a = 0)
	{
		_color[0] = static_cast<unsigned char>(_r);
		_color[1] = static_cast<unsigned char>(_g);
		_color[2] = static_cast<unsigned char>(_b);
		_color[3] = static_cast<unsigned char>(_a);
	}

	void GetColor(int &_r, int &_g, int &_b, int &_a) const
	{
		_r = _color[0];
		_g = _color[1];
		_b = _color[2];
		_a = _color[3];
	}

	void SetRawColor( int color )
	{
		*((int *)this) = color;
	}

	int GetRawColor() const
	{
		return *((int *)this);
	}

	inline int r() const	{ return _color[0]; }
	inline int g() const	{ return _color[1]; }
	inline int b() const	{ return _color[2]; }
	inline int a() const	{ return _color[3]; }
	
	unsigned char &operator[](int index)
	{
		return _color[index];
	}

	const unsigned char &operator[](int index) const
	{
		return _color[index];
	}

	bool operator == (const Color &rhs) const
	{
		return ( *((int *)this) == *((int *)&rhs) );
	}

	bool operator != (const Color &rhs) const
	{
		return !(operator==(rhs));
	}

	Color &operator=( const Color &rhs )
	{
		SetRawColor( rhs.GetRawColor() );
		return *this;
	}

	Color &operator=( const color32 &rhs )
	{
		_color[0] = rhs.r;
		_color[1] = rhs.g;
		_color[2] = rhs.b;
		_color[3] = rhs.a;
		return *this;
	}

	color32 ToColor32() const
	{
		color32 newColor;
		newColor.r = _color[0];
		newColor.g = _color[1];
		newColor.b = _color[2];
		newColor.a = _color[3];
		return newColor;
	}

	colorVec4 ToVector4D() const
	{
		colorVec4 newColor;
		newColor.r = static_cast<float>(_color[0]);
		newColor.g = static_cast<float>(_color[1]);
		newColor.b = static_cast<float>(_color[2]);
		newColor.a = static_cast<float>(_color[3]);
		return newColor;
	}

private:
	unsigned char _color[4];
};


#endif // COLOR_H
