#ifndef MATERIAL2_H
#define MATERIAL2_H

#ifdef _WIN32
#pragma once
#endif

#include "materialsystem2/imaterial2.h"
#include <mathlib/vector.h>
#include <mathlib/vector2d.h>
#include <tier0/utlstringtoken.h>

//-----------------------------------------------------------------------------
// Implementation of IMaterial2. Members past the copy source are kept as
// padding where their types are not declared in the SDK.
//-----------------------------------------------------------------------------
class CMaterial2 : public IMaterial2
{
public:
	// Returned by GetCopySource()
	HMaterialWeak m_hCopySource;

	// Depth of EnterOverrideScope() / LeaveOverrideScope()
	int m_nUnk038;

	// Taken from a global counter when the material is created
	uint32 m_nUnk03C;

	uint8 m_unk040[ 0x30 ];

	// Render attributes, returned by GetAttributes()
	uint8 m_unk070[ 0x3A0 ];

	// Modes: count, allocation and six inline 160-byte modes
	uint8 m_unk410[ 0x3C8 ];

	uint8 m_unk7D8[ 0x8 ];
};

#endif // MATERIAL2_H
