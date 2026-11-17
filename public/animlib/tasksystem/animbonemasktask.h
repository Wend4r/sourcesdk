//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Bone mask tasks: the recipe a bone mask value node hands to the
//          task system instead of a computed mask
//
//===========================================================================//

#ifndef ANIMBONEMASKTASK_H
#define ANIMBONEMASKTASK_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlleanvector.h"

struct NmBoneMaskTask_t
{
	enum class Type_t : uint8
	{
		// Use the mask set m_maskSetIdx of the skeleton
		Mask = 0,
		// A mask with every bone weight set to m_weight
		GenerateMask,
		// Blend the masks of two earlier tasks by m_weight
		Blend,
		// Scale the mask of an earlier task by m_weight
		Scale,
		// Multiply the masks of two earlier tasks
		Combine,
	};

	float m_weight;
	// Indices of earlier tasks in the list, -1 when unused
	int8 m_sourceTaskIdx;
	int8 m_targetTaskIdx;
	uint8 m_maskSetIdx;
	Type_t m_type;
};

// The last task of the list produces the mask
class CNmBoneMaskTaskList
{
public:
	CUtlLeanVectorFixedGrowable< NmBoneMaskTask_t, 10, int16 > m_tasks;
};

#endif // ANIMBONEMASKTASK_H
