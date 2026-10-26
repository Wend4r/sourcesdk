//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: A 3x3 matrix stored by columns
//
//===========================================================================//

#ifndef MATRIX3X3_H
#define MATRIX3X3_H

#ifdef _WIN32
#pragma once
#endif

#include "mathlib/vector.h"

#include "schemacompiler/schemaon.h"

schema class Matrix3x3
{
public:
	Vector c1;
	Vector c2;
	Vector c3;
};

#include "schemacompiler/schemaoff.h"

#endif // MATRIX3X3_H
