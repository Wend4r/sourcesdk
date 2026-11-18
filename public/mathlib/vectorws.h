#ifndef VECTORWS_H
#define VECTORWS_H

#ifdef _WIN32
#pragma once
#endif

#include "vector.h"

#include "schemacompiler/schemaon.h"

// AMNOTE: Mostly a stub over a real VectorWS,
// most likely meaning of it is world space vector
schema class VectorWS : public Vector
{
public:
	using Vector::Vector;
};

class QuaternionWS : public Quaternion
{
public:
	using Quaternion::Quaternion;
};

#include "schemacompiler/schemaoff.h"

#endif // VECTORWS_H
